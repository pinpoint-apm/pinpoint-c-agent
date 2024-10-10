////////////////////////////////////////////////////////////////////////////////
// Copyright 2020 NAVER Corp
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations under
// the License.
////////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "pinpoint_define.h"
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <thread>
#include <time.h>

thread_local NodeID local_node_id = E_ROOT_NODE;
const char *app_id = "cd.dev.test.cpp";
const char *app_name = "cd.dev.test.cpp";

std::string get_sid() { return std::to_string(rand() % 100000000l); }

std::string get_tid() {
  std::string sid = "";
  return sid + app_id + "^" + std::to_string(pinpoint_start_time()) + "^" +
         std::to_string(generate_unique_id());
}

std::string span_id_ = get_sid();
std::string transcation_id_ = get_tid();

void random_sleep() {
  int32_t delay = rand() % 100;
  std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

void test_httpclient() {
  local_node_id = pinpoint_start_trace(local_node_id);
  pinpoint_add_clue(local_node_id, PP_INTERCEPTOR_NAME, "httpclient",
                    E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_DESTINATION, "www.pinpoint-wonderful.com",
                    E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_SERVER_TYPE, PP_C_CPP_REMOTE_METHOD,
                    E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_NEXT_SPAN_ID, get_sid().c_str(),
                    E_LOC_CURRENT);
  pinpoint_add_clues(local_node_id, PP_HTTP_URL, "/support/c-cpp-php-python",
                     E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_ADD_EXCEPTION, "test this exception",
                    E_LOC_CURRENT);
  pinpoint_add_clues(local_node_id, PP_HTTP_STATUS_CODE, "300", E_LOC_CURRENT);

  random_sleep();

  local_node_id = pinpoint_end_trace(local_node_id);
}

void test_mysql() {
  local_node_id = pinpoint_start_trace(local_node_id);
  pinpoint_add_clue(local_node_id, PP_INTERCEPTOR_NAME, "mysql::excute",
                    E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_SERVER_TYPE, PP_MYSQL, E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_SQL_FORMAT, "select 1*3;", E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_DESTINATION, "localhost:3307",
                    E_LOC_CURRENT);
  random_sleep();
  local_node_id = pinpoint_end_trace(local_node_id);
}

void test_func() {
  local_node_id = pinpoint_start_trace(local_node_id);
  pinpoint_add_clue(local_node_id, PP_INTERCEPTOR_NAME, "test_func",
                    E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_SERVER_TYPE, PP_C_CPP_METHOD,
                    E_LOC_CURRENT);
  pinpoint_add_clues(local_node_id, PP_ARGS, "I'm the parameters",
                     E_LOC_CURRENT);
  random_sleep();
  local_node_id = pinpoint_end_trace(local_node_id);
}

void test_kafka() {
  local_node_id = pinpoint_start_trace(local_node_id);
  pinpoint_add_clue(local_node_id, "name", "kafka", E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, "stp", PP_KAFKA, E_LOC_CURRENT);
  pinpoint_add_clues(local_node_id, "140", "xxxxx", E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, "dst", "xxxx", E_LOC_CURRENT);
  local_node_id = pinpoint_end_trace(local_node_id);
}

static void test_async() {
  local_node_id = pinpoint_start_trace(local_node_id);

  pinpoint_add_clue(local_node_id, PP_INTERCEPTOR_NAME, "test_async",
                    E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_SERVER_TYPE, PP_INVOCATION_CALL_TYPE,
                    E_LOC_CURRENT);

  // generate async_id
  char async_id_str[32] = {0};
  int32_t async_id = random() % 99999;
  sprintf(async_id_str, "%d", async_id);
  pinpoint_add_clue(local_node_id, PP_ASYNC_CALL_ID, async_id_str,
                    E_LOC_CURRENT);
  // get sequence
  int32_t sequence_id = pinpoint_get_sequence_id(local_node_id);

  auto pinpoint_invocation_wrap_func =
      [=](std::function<void(void)> user_func) {
        local_node_id = pinpoint_start_trace(local_node_id);
        pinpoint_add_clue(local_node_id, PP_APP_NAME, "cpp_app", E_LOC_CURRENT);
        pinpoint_add_clue(local_node_id, PP_APP_ID, "CPP_APP", E_LOC_CURRENT);
        pinpoint_add_clue(local_node_id, PP_TRANSCATION_ID,
                          transcation_id_.c_str(), E_LOC_CURRENT);
        pinpoint_add_clue(local_node_id, PP_SPAN_ID, span_id_.c_str(),
                          E_LOC_CURRENT);

        pinpoint_add_clue(local_node_id, PP_SERVER_TYPE, PP_C_CPP_METHOD,
                          E_LOC_CURRENT);
        pinpoint_set_async_ctx(local_node_id, async_id, sequence_id);

        user_func();

        local_node_id = pinpoint_end_trace(local_node_id);
      };
  // test_func is the target function
  // while, you can use std::bind magic supporting any kind of function
  // eg: std::bind(func,"a",3,4) ;
  // std::bind(&MyClass::print, &obj, "bbc");
  // more question: https://en.cppreference.com/w/cpp/utility/functional/bind
  // or `dl_cd_pinpoint@navercorp.com`
  std::thread async_call(pinpoint_invocation_wrap_func, test_func);

  local_node_id = pinpoint_end_trace(local_node_id);
  // joins async_call thread after local_node_id stopped
  async_call.join();
}

void test_req() {
  local_node_id = pinpoint_start_trace(local_node_id);
  pinpoint_add_clue(local_node_id, PP_REQ_URI, "test_url", E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_REQ_CLIENT, "127.0.0.1", E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_REQ_SERVER, "HTTP_HOST", E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_SERVER_TYPE, PP_C_CPP, E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_INTERCEPTOR_NAME, "C_CPP Request",
                    E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_APP_NAME, "cpp_app", E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_APP_ID, "CPP_APP", E_LOC_CURRENT);

  random_sleep();

  test_func();
  test_mysql();
  test_httpclient();
  test_kafka();
  // make a asynchronous (by thread) call
  test_async();

  pinpoint_add_clue(local_node_id, PP_TRANSCATION_ID, transcation_id_.c_str(),
                    E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_SPAN_ID, span_id_.c_str(), E_LOC_CURRENT);

  pinpoint_add_clues(local_node_id, PP_HTTP_STATUS_CODE, "200", E_LOC_CURRENT);
  catch_error(local_node_id, "msg", __FILE__, 100);
  local_node_id = pinpoint_end_trace(local_node_id);
}

int main(int argc, char const *argv[]) {
  pinpoint_set_agent("tcp:127.0.0.1:10000", 10, -1, 1300);
  register_logging_cb(nullptr, 1);
  srand(time(nullptr));
  int i = 0;
  for (; i < 3; i++) {
    test_req();
  }
  return 0;
}
