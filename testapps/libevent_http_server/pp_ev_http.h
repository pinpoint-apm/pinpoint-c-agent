#pragma once

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/keyvalq_struct.h>
#include <event2/listener.h>

#include "pinpoint_define.h"
#include <linux/limits.h>
#include <pinpoint/common.h>
#ifdef _WIN32
#include <event2/thread.h>
#endif /* _WIN32 */

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

typedef void (*ev_http_request_cb)(struct evhttp_request *, void *);

// magic number always means some thing
#define CTX_MATIC_NUM 0x1357325l
typedef struct {
  int magic_num_;
  NodeID current_id_;
  void *ori_arg_;
  void (*on_complete_cb_origin_)(struct evhttp_request *, void *);
  void *on_complete_cb_arg_origin_;

  void (*http_client_request_cb_)(struct evhttp_request *, void *);
  void *http_client_request_done_cb_arg_;
} pp_request_context_t;

typedef void (*parent_req_continue_cb_t)(pp_http_client_t *client,
                                         struct evhttp_request *, void *);

int pp_http_get(const char *url, struct event_base *base,
                parent_req_continue_cb_t parent_req_continue_cb,
                struct evhttp_request *parent_req, void *parent_arg);

void pp_free_http_client(pp_http_client_t *client);

const char *pp_get_app_id();
const char *pp_get_app_name();

const char *gen_span_id();
const char *gen_tid();

void pp_request_complete(struct evhttp_request *req, void *args);

void pinpoint_set_agent_helper(const char *app_name, const char *app_id,
                               const char *collector_agent_address,
                               long timeout_ms, long trace_limit);

#define MAKE_PP_ENTRY(func)                                                    \
  static void pp_##func(struct evhttp_request *req, void *arg) {               \
    NodeID node = pinpoint_start_trace(E_ROOT_NODE);                           \
    pinpoint_add_clue(node, PP_REQ_URI, evhttp_request_get_uri(req),           \
                      E_LOC_CURRENT);                                          \
    pinpoint_add_clue(node, PP_REQ_CLIENT, req->remote_host, E_LOC_CURRENT);   \
    pinpoint_add_clue(node, PP_REQ_SERVER, evhttp_request_get_host(req),       \
                      E_LOC_CURRENT);                                          \
    pinpoint_add_clue(node, PP_SERVER_TYPE, PP_C_CPP, E_LOC_CURRENT);          \
    pinpoint_add_clue(node, PP_INTERCEPTOR_NAME, "C_CPP Request",              \
                      E_LOC_CURRENT);                                          \
    pinpoint_add_clue(node, PP_APP_NAME, pp_get_app_name(), E_LOC_CURRENT);    \
    pinpoint_add_clue(node, PP_APP_ID, pp_get_app_id(), E_LOC_CURRENT);        \
    const char *tid = gen_tid();                                               \
    pinpoint_add_clue(node, PP_TRANSCATION_ID, tid, E_LOC_CURRENT);            \
    pinpoint_set_context_key(node, PP_TRANSCATION_ID, tid);                    \
    const char *sid = gen_span_id();                                           \
    pinpoint_add_clue(node, PP_SPAN_ID, sid, E_LOC_CURRENT);                   \
    pinpoint_set_context_key(node, PP_SPAN_ID, sid);                           \
    pp_request_context_t *ctx = malloc(sizeof(pp_request_context_t));          \
    if (ctx) {                                                                 \
      ctx->magic_num_ = CTX_MATIC_NUM;                                         \
      ctx->ori_arg_ = arg;                                                     \
      ctx->current_id_ = node;                                                 \
      func(req, ctx);                                                          \
      ctx->on_complete_cb_origin_ = req->on_complete_cb;                       \
      ctx->on_complete_cb_arg_origin_ = req->on_complete_cb_arg;               \
      evhttp_request_set_on_complete_cb(req, pp_request_complete, ctx);        \
    } else {                                                                   \
      func(req, arg);                                                          \
      pinpoint_end_trace(node);                                                \
    }                                                                          \
  }
