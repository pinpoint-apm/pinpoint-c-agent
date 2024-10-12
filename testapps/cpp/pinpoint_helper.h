#pragma once

////////////////////////////////////////////////////////////////////////////////
// Copyright 2024 NAVER Corp
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

#include "pinpoint_define.h"
#include <pinpoint/common.h>

#include <functional>
#include <future>
#include <string>
#include <utility>

// per-thread context id
static thread_local NodeID local_node_id = E_ROOT_NODE;

namespace pinpoint {

// inspired by https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h
// https://en.cppreference.com/w/cpp/thread/packaged_task
template <typename F, typename... Args>
auto pin_func(const char *name, F &&f, Args &&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {

  local_node_id = pinpoint_start_trace(local_node_id);
  pinpoint_add_clue(local_node_id, PP_INTERCEPTOR_NAME, name, E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_SERVER_TYPE, PP_C_CPP_METHOD,
                    E_LOC_CURRENT);

  using return_type = typename std::result_of<F(Args...)>::type;

  auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task->get_future();

  (*task)();

  local_node_id = pinpoint_end_trace(local_node_id);
  return res;
}
template <typename F, typename... Args>
inline std::future<typename std::result_of<F(Args...)>::type>
async(F &&f, Args &&... args) {
  local_node_id = pinpoint_start_trace(local_node_id);

  pinpoint_add_clue(local_node_id, PP_INTERCEPTOR_NAME, "pinpoint::async",
                    E_LOC_CURRENT);
  pinpoint_add_clue(local_node_id, PP_SERVER_TYPE, PP_INVOCATION_CALL_TYPE,
                    E_LOC_CURRENT);
  // a random number for avoiding ID collision
  int32_t async_id = random() % 99999;
  std::string async_id_str = std::to_string(async_id);
  pinpoint_add_clue(local_node_id, PP_ASYNC_CALL_ID, async_id_str.c_str(),
                    E_LOC_CURRENT);
  int32_t sequence_id = pinpoint_get_sequence_id(local_node_id);

  using return_type = typename std::result_of<F(Args...)>::type;

  auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  char buf[128];
  int size = 0;
  size = pinpoint_get_context_key(local_node_id, PP_TRANSCATION_ID, buf, 128);
  std::string tid(buf, size);

  size = pinpoint_get_context_key(local_node_id, PP_SPAN_ID, buf, 128);
  std::string sid(buf, size);
  // get the root trace tid and span id
  size = pinpoint_get_context_key(local_node_id, PP_APP_NAME, buf, 128);
  std::string app_name(buf, size);
  size = pinpoint_get_context_key(local_node_id, PP_APP_ID, buf, 128);
  std::string app_id(buf, size);
  // wrapper user function (F) with  pinpoint_wrapper_func
  auto pinpoint_wrapper_func = [=]() -> return_type {
    local_node_id = pinpoint_start_trace(local_node_id);
    pinpoint_add_clue(local_node_id, PP_APP_NAME, app_name.c_str(),
                      E_LOC_CURRENT);
    pinpoint_add_clue(local_node_id, PP_APP_ID, app_id.c_str(), E_LOC_CURRENT);

    pinpoint_add_clue(local_node_id, PP_TRANSCATION_ID, tid.c_str(),
                      E_LOC_CURRENT);
    pinpoint_add_clue(local_node_id, PP_SPAN_ID, sid.c_str(), E_LOC_CURRENT);

    pinpoint_add_clue(local_node_id, PP_SERVER_TYPE, PP_C_CPP_METHOD,
                      E_LOC_CURRENT);
    // mark current span as a asynchronous span
    pinpoint_set_async_ctx(local_node_id, async_id, sequence_id);
    (*task)();
    local_node_id = pinpoint_end_trace(local_node_id);
    return task->get_future().get();
  };

  auto async_res = std::async(std::launch::async, pinpoint_wrapper_func);
  // end current trace
  local_node_id = pinpoint_end_trace(local_node_id);
  return async_res;
}

} // namespace pinpoint

// @author eeliu