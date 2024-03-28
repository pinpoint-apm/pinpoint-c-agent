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
 * TraceNode.h
 *
 *  Created on: Aug 19, 2020
 *      Author: eeliu
 */

#ifndef COMMON_SRC_TRACENODE_H_
#define COMMON_SRC_TRACENODE_H_
#include "common.h"
#include "Context/ContextType.h"
#include "json/json.h"
#include <atomic>
#include <functional>
#include <map>
#include <mutex>
#include <stdarg.h>
#include <memory>
#include <vector>
#include <inttypes.h>
#include <vector>
#include <assert.h>
namespace PP {
namespace NodePool {
namespace Json = AliasJson;
using Context::ContextType;
using Context::LongContextType;
using Context::StringContextType;
class WrapperTraceNodePtr;
const static int MAX_SUB_TRACE_NODES_LIMIT = 2048;
typedef std::shared_ptr<ContextType> _ContextType_;
using EndTraceCallBackFunc = std::function<bool()>;
using EndTraceCallBackFuncVec = std::vector<EndTraceCallBackFunc>;
class TraceNode {
public:
  /**                                 sibling_id_
   * current_node       <--------------------------------------------- sibling_node
   *         |-------------------------------------|
   *                                                v
   *     child_head_id_ <- next <-next <- ...<-last_child_id_
   */

  NodeID sibling_id_;    // sibling_id_ is brother node id; it's a next_ptr to child_list
  NodeID last_child_id_; // last_child_id_ is the end of child_list

  NodeID parent_id_; // parent Id [end_trace] avoiding re-add
  NodeID root_id_;   // highway to root node
  NodeID id_;

  uint64_t start_time;
  uint64_t fetal_error_time;
  uint64_t limit;
  uint64_t cumulative_time;
  uint64_t root_start_time;
  uint64_t parent_start_time;
  bool set_exp_;

public:
  void StartTimer();
  void EndTimer();

  DEPRECATED("") void WakeUpTimer();

  void EndSpanEvent();
  void EndSpan();

public:
  void AddChildTraceNode(TraceNode& child);
  DEPRECATED("") void AddChildTraceNode(WrapperTraceNodePtr& child);
  inline bool IsRootNode() const { return this->root_id_ == id_; }

public:
  TraceNode() {
    this->id_ = E_INVALID_NODE;
    this->root_id_ = E_INVALID_NODE;
    this->resetRelative();
    this->resetStatus();
    this->ref_count_ = 0;
    this->font_type_ = 0;
    error_.set = false;
  }

  virtual ~TraceNode();

  TraceNode& reset(NodeID id) {
    std::lock_guard<std::mutex> _safe(this->mlock);
    this->clearAttach();
    this->initId(id);
    this->resetStatus();
    this->resetRelative();
    this->ref_count_ = 0;
    return *this;
  }

  NodeID getId() const { return this->id_; }

  void getContext(const char* key, std::string& value) {
    std::lock_guard<std::mutex> _safe(this->mlock);
    _ContextType_& ctx = this->context_.at(key);
    value = ctx->asStringValue();
  }

  void getContext(const char* key, long& value) {
    std::lock_guard<std::mutex> _safe(this->mlock);
    _ContextType_& ctx = this->context_.at(key);
    value = ctx->asLongValue();
  }

  void setContext(const char* key, const char* buf) {
    std::lock_guard<std::mutex> _safe(this->mlock);
    _ContextType_ context(std::make_shared<StringContextType>(buf));
    this->context_[key] = context;
  }

  void setContext(const char* key, long l) {
    std::lock_guard<std::mutex> _safe(this->mlock);
    _ContextType_ context(std::make_shared<LongContextType>(l));
    this->context_[key] = context;
  }

public:
  TraceNode& operator=(const TraceNode&) = delete;
  TraceNode(const TraceNode&) = delete;

  bool operator==(TraceNode const& _node) const { return this->id_ == _node.id_; }

  bool operator!=(TraceNode const& _node) const { return this->id_ != _node.id_; }

private:
  int font_type_;

  struct Error {
    std::string message;
    std::string file_name;
    uint32_t line_nu;
    bool set;
  } error_;

public:
  void SetFontType(int type) { font_type_ = type; }

  void SetErrorInfo(std::string&& msg, std::string&& filename, uint32_t line) {
    error_.message = std::move(msg);
    error_.file_name = std::move(filename);
    error_.line_nu = line;
    error_.set = true;
  }

public:
  void AddTraceDetail(const char* key, const char* v) {
    std::lock_guard<std::mutex> _safe(this->mlock);
    this->_value[key] = v;
  }

  void AddTraceDetail(const char* key, int v) {
    std::lock_guard<std::mutex> _safe(this->mlock);
    this->_value[key] = v;
  }

  void AddTraceDetail(const char* key, uint64_t v) {
    std::lock_guard<std::mutex> _safe(this->mlock);
    this->_value[key] = v;
  }

  void AddTraceDetail(const char* key, const Json::Value& v) {
    std::lock_guard<std::mutex> _safe(this->mlock);
    this->_value[key] = v;
  }

  void appendNodeValue(const char* key, Json::Value v) {
    std::lock_guard<std::mutex> _safe(this->mlock);
    this->_value[key].append(v);
  }

  void appendNodeValue(const char* key, const char* v) {
    std::lock_guard<std::mutex> _safe(this->mlock);
    this->_value[key].append(v);
  }

public:
  void setOpt(const char* opt, va_list* args);
  bool checkOpt();

private:
  void parseOpt(std::string key, std::string value);

  void clearAttach();

  void initId(const NodeID& id);

  inline void resetRelative() {
    this->sibling_id_ = E_INVALID_NODE;
    this->last_child_id_ = E_INVALID_NODE;
    this->parent_id_ = E_ROOT_NODE;
    this->root_id_ = id_;
    this->_subTraceNodeMaxSize = MAX_SUB_TRACE_NODES_LIMIT;
  }

  inline void resetStatus() {
    this->fetal_error_time = 0;
    this->root_start_time = 0;
    this->parent_start_time = 0;
    this->start_time = 0;
    this->limit = E_TRACE_PASS;
    this->cumulative_time = 0;
    this->set_exp_ = false;
    error_.set = false;
  }

public:
  // changes: expose _lock
  std::mutex mlock;

public:
  int addRef() {
    ref_count_++;
    return ref_count_.load();
  }

  int rmRef() {
    ref_count_--;
    return ref_count_.load();
  }

  bool checkZeroRef() { return ref_count_.load() == 0; }

public:
  std::string ToString() {
    std::lock_guard<std::mutex> _safe(this->mlock);
    char pbuf[1024] = {0};
    int len =
        snprintf(pbuf, 1024,
                 "sibling_id_:%d mChildListHeaderId:%d parent_id_:%d root_id_:%d id_:%d \n"
                 "start_time:%" PRIu64 ",fetal_error_time:%" PRIu64 ",limit:%" PRIu64
                 ",cumulative_time:%" PRIu64 " \n"
                 "root_start_time:%" PRIu64 ",set_exp_:%d \n"
                 "ref_count_:%d \n"
                 "_value:%s \n"
                 "context_ size:%" PRIu64 ",_endTraceCallback:%" PRIu64 " \n",
                 (int)this->sibling_id_, (int)this->last_child_id_, (int)this->parent_id_,
                 (int)this->root_id_, (int)this->id_, this->start_time, this->fetal_error_time,
                 this->limit, this->cumulative_time, this->root_start_time, this->set_exp_,
                 this->ref_count_.load(), this->_value.toStyledString().c_str(),
                 this->context_.size(), this->_endTraceCallback.size());
    return std::string(pbuf, len);
  }

private:
  std::atomic<int> ref_count_;
  int _subTraceNodeMaxSize;

public:
  // note: not tls, not a force limitation
  inline void updateRootSubTraceSize() {
    if (this->_subTraceNodeMaxSize < 0) {
      throw std::out_of_range("current span reached max sub node limitation");
    } else {
      this->_subTraceNodeMaxSize--;
    }
  }

public:
  Json::Value& EncodeProtocol() { return _value; }

private:
  Json::Value _value;
  std::map<std::string, _ContextType_> context_;
  EndTraceCallBackFuncVec _endTraceCallback;
};

class WrapperTraceNodePtr {
public:
  WrapperTraceNodePtr() = delete;
  WrapperTraceNodePtr(const WrapperTraceNodePtr& other) = delete;
  WrapperTraceNodePtr(WrapperTraceNodePtr& other) = delete;
  WrapperTraceNodePtr(WrapperTraceNodePtr&& other) : traceNode_(other.traceNode_) {
    traceNode_.addRef();
  }

  WrapperTraceNodePtr& operator=(const WrapperTraceNodePtr& other) = delete;

  WrapperTraceNodePtr(TraceNode& node) : traceNode_(node) { traceNode_.addRef(); }
  TraceNode* operator->() { return &traceNode_; }
  ~WrapperTraceNodePtr() { traceNode_.rmRef(); }

private:
  TraceNode& traceNode_;
};
} // namespace NodePool
} // namespace PP
#endif /* COMMON_SRC_TRACENODE_H_ */
