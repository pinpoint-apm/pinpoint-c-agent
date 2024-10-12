#pragma once

// clang-format off
#include <event2/http.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http_struct.h>
#include <event2/keyvalq_struct.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#ifdef _WIN32
#include <event2/thread.h>
#endif /* _WIN32 */
// clang-format on

typedef struct pp_http_client_t_ {
  struct event_base *base_;
  struct bufferevent *bev_;
  struct evhttp_connection *evcon_;
  struct evhttp_uri *http_uri_;
  uint16_t port_;
  const char *method;
  const char *scheme_;
  const char *host_;
  int status_;
  char *resp_body_;
} pp_http_client_t;

typedef void (*parent_req_continue_cb_t)(pp_http_client_t *client,
                                         struct evhttp_request *, void *);

int pp_http_get(const char *url, struct event_base *base,
                parent_req_continue_cb_t parent_req_continue_cb,
                struct evhttp_request *parent_req, void *parent_arg);

void pp_free_http_client(pp_http_client_t *client);