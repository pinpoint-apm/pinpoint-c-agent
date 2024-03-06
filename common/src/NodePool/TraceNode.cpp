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
 * TraceNode.cpp
 *
 *  Created on: Aug 19, 2020
 *      Author: eeliu
 */

#include "TraceNode.h"
#include "header.h"
namespace NodePool {
TraceNode::~TraceNode() {}

void TraceNode::clearAttach() {
  // empty the json value
  if (!this->_value.empty())
    this->_value.clear(); // Json::Value();

  if (!this->_context.empty())
    this->_context.clear();

  if (!this->_endTraceCallback.empty())
    this->_endTraceCallback.clear();
}

void TraceNode::initId(NodeID& id) { this->mPoolIndex = id; }

void TraceNode::addChild(WrapperTraceNode& child) {
  std::lock_guard<std::mutex> _safe(this->mlock);

  if (this->mChildHeadId != E_INVALID_NODE)
    child->mNextId = this->mChildHeadId;

  this->mChildHeadId = child->mPoolIndex;

  child->mParentId = this->mPoolIndex;
  child->mRootIndex = this->mRootIndex;
  child->root_start_time = this->root_start_time;
  child->parent_start_time = this->start_time;
}

void TraceNode::endTimer() {
  uint64_t end_time = get_unix_time_ms();

  this->cumulative_time += (end_time - this->start_time);
}

void TraceNode::wakeUp() { this->start_time = get_unix_time_ms(); }

void TraceNode::convertToSpan() {
  this->AddTraceDetail(":E", this->cumulative_time);
  this->AddTraceDetail(":S", this->start_time);
}

void TraceNode::convertToSpanEvent() {
  this->AddTraceDetail(":E", this->cumulative_time);
  this->AddTraceDetail(":S", this->start_time - this->parent_start_time);
}

void TraceNode::startTimer() {
  uint64_t time_in_ms = get_unix_time_ms();
  this->start_time = time_in_ms;
  this->root_start_time = time_in_ms;
}

void TraceNode::parseOpt(std::string key, std::string value) {
  pp_trace(" [%d] add opt: key:%s value:%s", mPoolIndex, key.c_str(), value.c_str());
  if (key == "TraceMinTimeMs") {
    int64_t min = std::stoll(value);
    auto cb = [=]() -> bool {
      pp_trace("checkOpt:  [%d] TraceMinTimeMs:%ld cumulative_time:%lu", this->mPoolIndex, min,
               this->cumulative_time);
      if ((int64_t)this->cumulative_time >= min)
        return true;
      return false;
    };
    this->_endTraceCallback.push_back(cb);
  } else if (key == "TraceOnlyException") {
    auto cb = [=]() -> bool { return this->mHasExp; };

    this->_endTraceCallback.push_back(cb);
  }
}

bool TraceNode::checkOpt() {
  bool ret = true;
  for (auto& cb : this->_endTraceCallback) {
    if ((ret = cb()) == true)
      return ret;
  }
  return ret;
}

void TraceNode::setOpt(const char* opt, va_list* args) {
  const char* var = opt;

  while (var != nullptr) {
    const char* delimit = strchr(var, ':');
    if (delimit == nullptr) {
      this->parseOpt(std::string(var), "");
    } else {
      std::string key(var, delimit - var);
      std::string value(delimit + 1);
      this->parseOpt(key, value);
    }
    var = va_arg(*args, const char*);
  }
}
} // namespace NodePool
