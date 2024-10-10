#include "common.h"
#include "httplib.h"
#include "pinpoint_define.h"
#include "pinpoint_helper.h"
#include <functional>
#include <future>
#include <string>

const char *app_id = "cd.dev.test.cpp";
const char *app_name = "cd.dev.test.cxx";

using Server = httplib::Server;
using StatusCode = httplib::StatusCode;

class PinpointServer : public Server {
private:
  std::string genSpanId() { return std::to_string(rand() % 100000000l); }

  std::string genTid() {
    std::string tid = app_id;
    return tid + "^" + std::to_string(pinpoint_start_time()) + "^" +
           std::to_string(generate_unique_id());
  }

protected:
  void entry(const httplib::Request &request, httplib::Response &response,
             Handler origin_handler) {

    local_node_id = pinpoint_start_trace(local_node_id);
    pinpoint_add_clue(local_node_id, PP_REQ_URI, request.path.c_str(),
                      E_LOC_CURRENT);
    pinpoint_add_clue(local_node_id, PP_REQ_CLIENT, request.remote_addr.c_str(),
                      E_LOC_CURRENT);
    pinpoint_add_clue(local_node_id, PP_REQ_SERVER, request.local_addr.c_str(),
                      E_LOC_CURRENT);
    pinpoint_add_clue(local_node_id, PP_SERVER_TYPE, PP_C_CPP, E_LOC_CURRENT);
    pinpoint_add_clue(local_node_id, PP_INTERCEPTOR_NAME, "C_CPP Request",
                      E_LOC_CURRENT);
    pinpoint_add_clue(local_node_id, PP_APP_NAME, app_name, E_LOC_CURRENT);
    pinpoint_add_clue(local_node_id, PP_APP_ID, app_id, E_LOC_CURRENT);
    pinpoint_set_context_key(local_node_id, PP_APP_NAME, app_name);
    pinpoint_set_context_key(local_node_id, PP_APP_ID, app_id);

    std::string tid = genTid();
    std::string span_id = genSpanId();
    pinpoint_add_clue(local_node_id, PP_TRANSCATION_ID, tid.c_str(),
                      E_LOC_CURRENT);
    pinpoint_set_context_key(local_node_id, PP_TRANSCATION_ID, tid.c_str());

    pinpoint_add_clue(local_node_id, PP_SPAN_ID, span_id.c_str(),
                      E_LOC_CURRENT);
    pinpoint_set_context_key(local_node_id, PP_SPAN_ID, span_id.c_str());

    origin_handler(request, response);

    std::string status_str = std::to_string(response.status);
    pinpoint_add_clues(local_node_id, PP_HTTP_STATUS_CODE, status_str.c_str(),
                       E_LOC_CURRENT);
    local_node_id = pinpoint_end_trace(local_node_id);
  }

public:
  Server &Get(const std::string &pattern, Handler handler) {

    return Server::Get(pattern, std::bind(&PinpointServer::entry, this,
                                          std::placeholders::_1,
                                          std::placeholders::_2, handler));
  };
};

int main(void) {
  pinpoint_set_agent("tcp:dev-collector:10000", 10, -1, 1300);
  register_logging_cb(nullptr, 1);

  PinpointServer p_svr;

  p_svr.Get("/hi", [](const httplib::Request &, httplib::Response &res) {
    res.status = StatusCode::OK_200;
    res.set_content("Hello pinpoint-c-agent !", "text/plain");
  });

  p_svr.Get("/create_async_task",
            [](const httplib::Request &, httplib::Response &res) {
              auto task = []() -> int {
                pinpoint::pin_func("sleep", sleep, 2);
                return 0;
              };
              auto async_res = pinpoint::async(task);
              pinpoint::pin_func("sleep", sleep, 2);
              pp_trace("async_task: %d", async_res.get());

              res.status = StatusCode::OK_200;
              res.set_content("sleep 5s!", "text/plain");
            });

  pp_trace(" server start at: 8080");
  p_svr.listen("0.0.0.0", 8080);
}