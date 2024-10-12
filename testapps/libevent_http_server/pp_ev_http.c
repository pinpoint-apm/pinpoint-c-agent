#include "pp_ev_http.h"
#include <malloc.h>
#include <pinpoint/common.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

typedef struct {
  pp_http_client_t *client_;
  parent_req_continue_cb_t parent_req_continue_cb_;
  struct evhttp_request *parent_req_;
  void *parent_arg_;
} sub_request_callback_t;

static void pp_sub_request_done(struct evhttp_request *req, void *ctx) {
  sub_request_callback_t *callback = ctx;

  pp_http_client_t *client = callback->client_;
  int chunk_count = 1;
  char *buf = NULL;
  struct evbuffer *ev_buf;
  int ev_buf_size = 0;
  // fill client
  if (req == NULL) {
    client->status_ = -1;
    goto ERROR;
  }

  client->status_ = evhttp_request_get_response_code(req);
  if (client->status_ == 0) {
    int errcode = EVUTIL_SOCKET_ERROR();
    pp_trace("errcode = %d", errcode);
    goto ERROR;
  }

  fprintf(stderr, "Response line: %d %s\n",
          evhttp_request_get_response_code(req),
          evhttp_request_get_response_code_line(req));

  ev_buf = evhttp_request_get_input_buffer(req);

  ev_buf_size = evbuffer_get_length(ev_buf);
  buf = malloc(ev_buf_size + 1);
  if (buf == NULL) {
    goto ERROR;
  } else {
    evbuffer_remove(ev_buf, buf, ev_buf_size);
    buf[ev_buf_size] = '\0';
    client->resp_body_ = buf;
  }

ERROR:

  if (callback->parent_req_continue_cb_) {
    callback->parent_req_continue_cb_(client, callback->parent_req_,
                                      callback->parent_arg_);
  }

  free(callback);
}

int pp_http_get(const char *url, struct event_base *base,
                parent_req_continue_cb_t parent_req_continue_cb,
                struct evhttp_request *parent_req, void *parent_arg) {
  pp_http_client_t *client =
      (pp_http_client_t *)malloc(sizeof(pp_http_client_t));
  memset(client, 0, sizeof(*client));

  if (client == NULL) {
    return -1;
  }

  client->base_ = base;

  const char *scheme, *host, *path, *query;
  int port;
  char uri[256];
  struct bufferevent *bev;
  struct evhttp_connection *evcon = NULL;
  struct evhttp_request *req;
  struct evkeyvalq *output_headers;
  struct evbuffer *output_buffer;
  int r = 0;

  struct evhttp_uri *http_uri = evhttp_uri_parse(url);
  if (http_uri == NULL) {
    goto ERROR;
  }

  scheme = evhttp_uri_get_scheme(http_uri);
  if (scheme == NULL || (strcasecmp(scheme, "http") != 0)) {
    pp_trace("url must be http ");
    goto ERROR;
  }
  client->scheme_ = scheme;

  port = evhttp_uri_get_port(http_uri);
  if (port == -1) {
    port = 80;
  }

  client->port_ = (uint16_t)port;

  host = evhttp_uri_get_host(http_uri);
  if (host == NULL) {
    pp_trace("url must have a host");
    goto ERROR;
  }
  client->host_ = host;

  path = evhttp_uri_get_path(http_uri);
  if (strlen(path) == 0) {
    path = "/";
  }

  query = evhttp_uri_get_query(http_uri);
  if (query == NULL) {
    snprintf(uri, sizeof(uri) - 1, "%s", path);
  } else {
    snprintf(uri, sizeof(uri) - 1, "%s?%s", path, query);
  }
  uri[sizeof(uri) - 1] = '\0';

  bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
  client->bev_ = bev;

  evcon = evhttp_connection_base_bufferevent_new(base, NULL, bev, host,
                                                 client->port_);

  evhttp_connection_set_family(evcon, AF_INET);

  sub_request_callback_t *sub_request_ctx =
      malloc(sizeof(sub_request_callback_t));

  sub_request_ctx->client_ = client;
  sub_request_ctx->parent_req_continue_cb_ = parent_req_continue_cb;
  sub_request_ctx->parent_req_ = parent_req;
  sub_request_ctx->parent_arg_ = parent_arg;

  req = evhttp_request_new(pp_sub_request_done, sub_request_ctx);

  output_headers = evhttp_request_get_output_headers(req);

  evhttp_add_header(output_headers, "Host", host);
  evhttp_add_header(output_headers, "Connection", "close");

  // todo add pinpoint header

  r = evhttp_make_request(evcon, req, EVHTTP_REQ_GET, uri);

  if (r != 0) {
    goto ERROR;
  }

  client->evcon_ = evcon;
  client->http_uri_ = http_uri;

  return 0;
ERROR:
  if (http_uri) {
    evhttp_uri_free(http_uri);
  }

  if (evcon) {
    evhttp_connection_free(evcon);
  }

  if (client) {
    free(client);
  }

  return -1;
}

void pp_free_http_client(pp_http_client_t *client) {
  if (client) {

    if (client->resp_body_) {
      free(client->resp_body_);
    }

    if (client->http_uri_) {
      evhttp_uri_free(client->http_uri_);
    }

    if (client->evcon_) {
      evhttp_connection_free(client->evcon_);
    }

    free(client);
  }
}
