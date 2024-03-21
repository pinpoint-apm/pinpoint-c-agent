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
/*
 * SpanConnectionPool.h
 *
 *  Created on: Aug 28, 2020
 *      Author: eeliu
 */

#ifndef COMMON_SRC_CONNECTPOOL_SPANCONNECTIONPOOL_H_
#define COMMON_SRC_CONNECTPOOL_SPANCONNECTIONPOOL_H_
#include <stack>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include "TransLayer.h"

namespace ConnectionPool {

using RouteMapValueVec = std::vector<RouteMapValueType>;
class SpanConnectionPool {
public:
  SpanConnectionPool(SpanConnectionPool&) = delete;
  SpanConnectionPool& operator=(const SpanConnectionPool&) = delete;
  SpanConnectionPool(SpanConnectionPool&&) = delete;
  SpanConnectionPool& operator=(SpanConnectionPool&&) = delete;

public:
  SpanConnectionPool(const char* co_host, RouteMapValueVec&& value_vec)
      : co_host(co_host), con_counter(0), routeVec_(value_vec) {
    this->_pool.push(this->createConnection(routeVec_));
  }

  virtual ~SpanConnectionPool() {}

  TransLayerPtr get() {
    std::lock_guard<std::mutex> _safe(this->_lock);
    if (this->_pool.empty()) {
      return createConnection(routeVec_);
    } else {
      TransLayerPtr _con = std::move(_pool.top());
      _pool.pop();
      assert(_con);
      return _con;
    }
  }

  void free(TransLayerPtr& con) {
    std::lock_guard<std::mutex> _safe(this->_lock);
    this->_pool.emplace(std::move(con));
  }

private:
  TransLayerPtr createConnection(const RouteMapValueVec& value_vec) {
    TransLayerPtr _connect(new TransLayer(this->co_host));
    using namespace std::placeholders;

    for (auto& value : value_vec) {
      _connect->RegPeerMsgCallback(value.first, value.second);
    }

    this->con_counter++;

    return _connect;
  }
  // void _handleTransLayerState(int state);
private:
  std::string co_host;
  uint32_t con_counter;
  std::stack<TransLayerPtr> _pool;
  std::mutex _lock;
  RouteMapValueVec routeVec_;
};

} /* namespace ConnectionPool */

#endif /* COMMON_SRC_CONNECTPOOL_SPANCONNECTIONPOOL_H_ */
