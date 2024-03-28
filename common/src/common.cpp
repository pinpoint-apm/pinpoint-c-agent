/*******************************************************************************
 * Copyright 2019 NAVER Corp
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations under
 * the License.
 ******************************************************************************/
//
// Created by eeliu on 1/3/2020.
//

#include "common.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <stdarg.h>
#include <functional>
#include <chrono>
#include "NodePool/PoolManager.h"
#include "ConnectionPool/SpanConnectionPool.h"
#include "header.h"
#include "Cache/State.h"
#include "Cache/NodeTreeWriter.h"

using std::chrono::milliseconds;
using std::chrono::system_clock;
using std::chrono::time_point;
using std::chrono::time_point_cast;

namespace PP {
namespace Json = AliasJson;
using Cache::NodeTreeWriter;
using ConnectionPool::TransLayerPtr;
using NodePool::TraceNode;
using NodePool::WrapperTraceNodePtr;

typedef struct {
  int agent_type;
} PPAgentT;

static std::function<void(const char*)> _raw_span_watcher_cb = {nullptr};
class Agent {
public:
  Agent() = delete;
  Agent(const Agent&) = delete;
  Agent(Agent&&) = delete;

public:
  Agent(const char* cl_host, long timeout_ms, long trace_limit, int agent_type)
      : span_timeout_(timeout_ms), agent_type_(agent_type),
        connection_pool_(cl_host,
                         {{0, std::bind(&Agent::HandleHelloMsg, this, std::placeholders::_1,
                                        std::placeholders::_2, std::placeholders::_3)}}) {
    statePtr_ = std::unique_ptr<ProcessState>(new ProcessState(trace_limit));
    builder["collectComments"] = false;
    pp_trace("init agent:%s timeout:%ld trace_limit:%ld agent_type:%d", cl_host, timeout_ms,
             trace_limit, agent_type_);
  }
  ~Agent() {}

private:
  void TrySendSpan(const std::string& span, int span_timeout) {
    TransLayerPtr trans = connection_pool_.get();

    if (!trans->copy_into_send_buffer(span)) {
      pp_trace("drop current span as it's too heavy! size:%lu", span.length());
    }
    if (span_timeout > 0) {
      trans->SyncSendAll(span_timeout);
    } else {
      trans->PoolEventOnce(0);
    }
    // if network not ready, span will send in next time.
    connection_pool_.free(trans);
  }
  void SendSpanTrace(NodeID root, int timeout) {
    Json::Value& trace = nodePool_.ExpandTraceTreeNodes(root);
    std::string spanStr = node_tree_to_string(trace);
    pp_trace("this span:(%s)", spanStr.c_str());
    TrySendSpan(spanStr, timeout);
    if (unlikely(rawSpanHandler_ != nullptr)) {
      rawSpanHandler_(spanStr.c_str());
    }
  }

  void HandleHelloMsg(int type, const char* buf, size_t len) {
    if (statePtr_->IsReady()) {
      return;
    }

    Json::Value root;
    (void)type;

    using Json::parseFromStream;
    std::istringstream in_stream(std::string(buf, len));
    bool ok = parseFromStream(builder, in_stream, &root, nullptr);
    if (!ok) {
      pp_trace("Receive invalid msg: %.*s from Collector-agent", (int)len, buf);
    }

    if (root["version"] || root["version"].isString()) {
      std::string version = root["version"].asString();
      std::string lowest_version = "v0.5.0";
      if (version < lowest_version) {
        pp_trace("collector-agent should be >=v0.5.0, please upgrade it");
        return;
      }
    }

    if (root["time"] && root["time"].isString()) {
      statePtr_->SetStartTime(std::stoll(root["time"].asString()));
    }
  }

public:
  NodeID StartTrace(NodeID id, const char* opt = nullptr, va_list* args = nullptr) {
    if (id <= E_INVALID_NODE) {
      throw std::out_of_range("invalid node id");
    } else if (id == E_ROOT_NODE) {
      TraceNode& trace = nodePool_.NewNode();
      trace.StartTimer();
      // HACK, set :FT into agent,as only agent knowns.
      trace.AddTraceDetail(":FT", agent_type_);
      return trace.id_;
    } else {
      WrapperTraceNodePtr parent = nodePool_.ReferNode(id);

      // get root node
      WrapperTraceNodePtr root = nodePool_.ReferNode(parent->root_id_);

      // check subnode limit
      root->updateRootSubTraceSize();
      // HACK: a new node only exist in one thread
      TraceNode& new_trace = nodePool_.NewNode();
      new_trace.StartTimer();
      parent->AddChildTraceNode(new_trace);
      // pass opt
      if (opt != nullptr) {
        new_trace.setOpt(opt, args);
      }
      return new_trace.id_;
    }
  }
  NodeID EndTrace(NodeID ID, int timeout = 0) {
    // HACK use cpp scope management
    {
      WrapperTraceNodePtr w_trace = nodePool_.ReferNode(ID);
      if (w_trace->IsRootNode()) {
        if (w_trace->limit & E_TRACE_PASS) {
          w_trace->EndTimer();
          w_trace->EndSpan();
          if (timeout == 0) {
            timeout = span_timeout_;
          }
          SendSpanTrace(ID, timeout);
        } else if (w_trace->limit & E_TRACE_BLOCK) {
          pp_trace("current [%d] span dropped,due to TRACE_BLOCK", w_trace->getId());
        } else {
          pp_trace("current [%d] span dropped,due to limit=%ld", w_trace->getId(), w_trace->limit);
        }
      } else {
        w_trace->EndTimer();
        w_trace->EndSpanEvent();
        return w_trace->parent_id_;
      }
    }
    // it already is a full trace, every thing is done.
    nodePool_.FreeNodeTree(ID);
    return E_ROOT_NODE;
  }

  bool IsRootTrace(NodeID id) {
    WrapperTraceNodePtr w_node = nodePool_.ReferNode(id);
    return w_node->IsRootNode();
  }
  uint64_t ChangeTraceStatus(NodeID id, int status) {
    WrapperTraceNodePtr w_node = nodePool_.ReferNode(id);
    WrapperTraceNodePtr w_root = nodePool_.ReferNode(w_node->root_id_);
    pp_trace("change current [%d] status, before:%lld,now:%d", w_root->getId(), w_root->limit,
             status);
    w_root->limit = status;
    return w_root->limit;
  }
  void AnnotateTrace_V1(NodeID id, const char* key, const char* value, E_NODE_LOC flag) {
    NotInternalKey(key);

    WrapperTraceNodePtr w_node = GetWrapperTraceNode(id, flag);
    w_node->AddTraceDetail(key, value);
    pp_trace(" [%d] add clue key:%s value:%s", id, key, value);
  }
  void AnnotateTrace_V2(NodeID id, const char* key, const char* value, E_NODE_LOC flag) {
    NotInternalKey(key);

    WrapperTraceNodePtr w_node = GetWrapperTraceNode(id, flag);
    std::string cvalue = "";
    cvalue += key;
    cvalue += ':';
    cvalue += value;
    w_node->appendNodeValue("clues", cvalue.c_str());
    pp_trace(" [%d] add clues:%s:%s", id, key, value);
  }
  void AnnotateErrorTrace(NodeID id, const char* msg, const char* error_filename,
                          uint32_t error_lineno) {
    WrapperTraceNodePtr w_root = GetWrapperTraceNode(id, E_LOC_ROOT);
    Json::Value eMsg;
    eMsg["msg"] = msg;
    eMsg["file"] = error_filename;
    eMsg["line"] = error_lineno;
    w_root->AddTraceDetail("ERR", eMsg);
  }
  void AnnotateExceptionTrace(NodeID id, const char* exception) {
    WrapperTraceNodePtr w_root = GetWrapperTraceNode(id, E_LOC_CURRENT);
    w_root->AddTraceDetail("EXP", exception);
    w_root->set_exp_ = true;
    pp_trace(" [%d] add exp value:%s", id, exception);
  }

  void AddTraceContext(NodeID id, const char* key, const char* value) {
    WrapperTraceNodePtr w_node = GetWrapperTraceNode(id, E_LOC_ROOT);
    w_node->setContext(key, value);
  }
  int GetTraceContext(NodeID id, const char* key, char* pbuf, int buf_size) {
    WrapperTraceNodePtr w_node = GetWrapperTraceNode(id, E_LOC_ROOT);
    std::string value;
    w_node->getContext(key, value);
    if (pbuf != nullptr && buf_size > (int)value.size()) {
      strncpy(pbuf, value.c_str(), buf_size);
      return (int)value.size();
    } else {
      pp_trace(" [%d] get context key:%s failed. buffer is not enough", id, key);
      return -1;
    }
  }

  uint64_t GetStartTime() { return statePtr_->GetStartTime(); }

  int64_t GetUniqueID() { return statePtr_->GetUniqueId(); }

  void ResetUniqueID() { statePtr_->ResetUniqueID(); }

  bool CheckTraceLimit(int64_t timestamp) { return statePtr_->CheckTraceLimit(timestamp); }

  using SpanHandler = void (*)(const char*);
  void RegisterRawSpanHandler(SpanHandler user_handler) { rawSpanHandler_ = user_handler; }

  std::string GetNodePoolStatus() { return nodePool_.Status(); }
  void DebugNodeId(NodeID id) {
    try {
      WrapperTraceNodePtr w_node = nodePool_.ReferNode(id);
      fprintf(stderr, "nodeid [%d]: { value:%s }", id, w_node->ToString().c_str());
    } catch (const std::exception& ex) {
      pp_trace(" debug_nodeid: [%d] Reason: %s", id, ex.what());
    }
  }

private:
  static inline void NotInternalKey(const char* key) {
    if (key == nullptr || key[0] == ':') {
      std::string reason = "key:";
      reason += key;
      reason += "is invalid";
      throw std::invalid_argument(reason);
    }
  }

  std::string node_tree_to_string(const Json::Value& value) { return _writer.write(value); }

  inline WrapperTraceNodePtr GetWrapperTraceNode(NodeID id, E_NODE_LOC flag) {
    WrapperTraceNodePtr w_node = nodePool_.ReferNode(id);
    if (flag == E_LOC_ROOT) {
      return nodePool_.ReferNode(w_node->root_id_);
    } else {
      return w_node;
    }
  }

private:
  int span_timeout_;
  int agent_type_;
  ConnectionPool::SpanConnectionPool connection_pool_;
  NodePool::PoolManager nodePool_;
  StatePtr statePtr_;
  SpanHandler rawSpanHandler_ = {nullptr};
  Json::CharReaderBuilder builder;

public:
  static thread_local NodeID per_thread_current_id;
  NodeTreeWriter _writer;
};

thread_local NodeID Agent::per_thread_current_id = E_ROOT_NODE;

using AgentPtr = std::unique_ptr<Agent>;
AgentPtr _agentPtr = {nullptr};

} // namespace PP

using PP::_agentPtr;
using PP::Agent;
using PP::AgentPtr;

NodeID pinpoint_get_per_thread_id() { return Agent::per_thread_current_id; }

void pinpoint_update_per_thread_id(NodeID id) { Agent::per_thread_current_id = id; }

bool pinpoint_set_agent(const char* collector_agent_address, long timeout_ms, long trace_limit,
                        int agent_type) {
#ifdef UTEST
  _agentPtr = AgentPtr(new Agent(collector_agent_address, timeout_ms, trace_limit, agent_type));
  return true;
#else
  if (_agentPtr == nullptr) {
    _agentPtr = AgentPtr(new Agent(collector_agent_address, timeout_ms, trace_limit, agent_type));
    return true;
  }
  return false;
#endif
}

void pinpoint_stop_agent() { _agentPtr = nullptr; }

NodeID pinpoint_start_trace(NodeID parentId) { return pinpoint_start_traceV1(parentId, nullptr); }

NodeID pinpoint_start_traceV1(NodeID parentId, const char* opt, ...) {
  if (_agentPtr) {
    try {
      va_list args;
      va_start(args, opt);
      NodeID child = _agentPtr->StartTrace(parentId, opt, &args);
      pp_trace(" [%d] pinpoint_start child  [%d]", parentId, child);
      va_end(args);
      return child;
    } catch (const std::out_of_range& ex) {
      pp_trace(" start_trace [%d] failed with %s", parentId, ex.what());
    } catch (const std::runtime_error& ex) {
      pp_trace(" start_trace [%d] failed with %s", parentId, ex.what());
    } catch (const std::exception& ex) {
      pp_trace(" start_trace [%d] failed with %s", parentId, ex.what());
    }
  }
  return E_INVALID_NODE;
}

void pinpoint_add_exception(NodeID id, const char* exp) {
  if (_agentPtr) {
    try {
      return _agentPtr->AnnotateExceptionTrace(id, exp);
    } catch (const std::out_of_range& ex) {
      pp_trace(" %s [%d] failed. Reason: %s,parameters:%s", __func__, id, ex.what(), exp);
    } catch (const std::runtime_error& ex) {
      pp_trace(" %s [%d] failed. Reason: %s,parameters:%s", __func__, id, ex.what(), exp);
    } catch (const std::exception& ex) {
      pp_trace(" %s [%d] failed. Reason: %s,parameters:%s", __func__, id, ex.what(), exp);
    }
  }
}

ParentNodeId pinpoint_end_trace(NodeID id) {
  if (_agentPtr) {
    try {
      NodeID ret = _agentPtr->EndTrace(id);
      pp_trace(" [%d] pinpoint_end_trace Done!", id);
      return ret;
    } catch (const std::out_of_range& ex) {
      pp_trace("end_trace %d out_of_range exception: %s", id, ex.what());
    } catch (const std::runtime_error& ex) {
      pp_trace("end_trace %d runtime_error: %s", id, ex.what());
    } catch (const std::exception& ex) {
      pp_trace("end_trace  [%d] end trace failed. %s", id, ex.what());
    }
  }
  return E_INVALID_NODE;
}

int pinpoint_trace_is_root(NodeID id) {
  if (_agentPtr) {
    try {
      if (id == E_ROOT_NODE) {
        return -1;
      }
      return _agentPtr->IsRootTrace(id) ? (1) : (0);
    } catch (const std::out_of_range&) {
      pp_trace("not found node=%d ", id);
    } catch (const std::exception& ex) {
      pp_trace(" node=%d end trace failed: %s", id, ex.what());
    }
  }
  return -1;
}

int pinpoint_force_end_trace(NodeID id, int32_t timeout) {
  if (_agentPtr) {
    try {
      while (id != E_ROOT_NODE) {
        id = _agentPtr->EndTrace(id, timeout);
        if (id == E_INVALID_NODE)
          break;
      }
      pp_trace(" [%d] pinpoint_end_trace Done!", id);
      return E_ROOT_NODE;
    } catch (const std::out_of_range& ex) {
      pp_trace("end_trace %d out_of_range exception: %s", id, ex.what());
    } catch (const std::runtime_error& ex) {
      pp_trace("end_trace %d runtime_error: %s", id, ex.what());
    } catch (const std::exception& ex) {
      pp_trace("end_trace  [%d] end trace failed. %s", id, ex.what());
    }
  }
  return E_INVALID_NODE;
}

void pinpoint_add_clues(NodeID id, const char* key, const char* value, E_NODE_LOC flag) {
  if (_agentPtr) {

    try {
      _agentPtr->AnnotateTrace_V2(id, key, value, flag);
    } catch (const std::out_of_range& ex) {
      pp_trace(" %s [%d] failed. Reason %s,parameters:%s:%s", __func__, id, ex.what(), key, value);
    } catch (const std::runtime_error& ex) {
      pp_trace(" %s [%d] failed. Reason %s,parameters:%s:%s", __func__, id, ex.what(), key, value);
    } catch (const std::exception& ex) {
      pp_trace(" %s [%d] failed. Reason %s,parameters:%s:%s", __func__, id, ex.what(), key, value);
    }
  }
}

void pinpoint_add_clue(NodeID id, const char* key, const char* value, E_NODE_LOC flag) {
  if (_agentPtr) {
    try {
      _agentPtr->AnnotateTrace_V1(id, key, value, flag);
    } catch (const std::out_of_range& ex) {
      pp_trace(" %s [%d] failed. Reason: %s,parameters:%s:%s", __func__, id, ex.what(), key, value);
    } catch (const std::runtime_error& ex) {
      pp_trace(" %s [%d] failed. Reason: %s,parameters:%s:%s", __func__, id, ex.what(), key, value);
    } catch (const std::exception& ex) {
      pp_trace(" %s [%d] failed. Reason: %s,parameters:%s:%s", __func__, id, ex.what(), key, value);
    }
  }
}

void pinpoint_set_context_key(NodeID id, const char* key, const char* value) {
  if (_agentPtr) {
    try {
      _agentPtr->AddTraceContext(id, key, value);
    } catch (const std::out_of_range& ex) {
      pp_trace(" %s [%d] failed with out_of_range. %s,parameters:%s:%s", __func__, id, ex.what(),
               key, value);
    } catch (const std::runtime_error& ex) {
      pp_trace(" %s [%d] failed with runtime_error. %s,parameters:%s:%s", __func__, id, ex.what(),
               key, value);
    } catch (const std::exception& ex) {
      pp_trace(" %s [%d] failed with %s,parameters:%s:%s", __func__, id, ex.what(), key, value);
    }
  }
}

int pinpoint_get_context_key(NodeID id, const char* key, char* pbuf, int buf_size) {
  if (_agentPtr) {
    try {
      return _agentPtr->GetTraceContext(id, key, pbuf, buf_size);
    } catch (const std::out_of_range& ex) {
      pp_trace(" %s [%d] failed with %s, parameters:%s", __func__, id, ex.what(), key);
    } catch (const std::runtime_error& ex) {
      pp_trace(" %s [%d] failed with %s, parameters:%s", __func__, id, ex.what(), key);
    } catch (const std::exception& ex) {
      pp_trace(" %s [%d] failed with %s, parameters:%s", __func__, id, ex.what(), key);
    }
  }
  return 0;
}

int check_trace_limit(int64_t timestamp) {
  if (_agentPtr) {
    return _agentPtr->CheckTraceLimit(timestamp) ? 1 : 0;
  } else {
    return 0;
  }
}

int check_tracelimit(int64_t timestamp) { return check_trace_limit(timestamp); }

uint64_t change_trace_status(NodeID id, int status) {
  if (_agentPtr) {
    try {
      return _agentPtr->ChangeTraceStatus(id, status);
    } catch (const std::out_of_range& ex) {
      pp_trace(" %s [%d] failed with %s", __func__, id, ex.what());
    } catch (const std::runtime_error& ex) {
      pp_trace(" %s [%d] failed with %s", __func__, id, ex.what());
    } catch (...) {
      pp_trace(" %s [%d] failed with unkonw reason", __func__, id);
    }
  }
  return 0;
}

uint64_t mark_current_trace_status(NodeID id, int status) {
  return change_trace_status(id, status);
}

int64_t generate_unique_id(void) {
  if (_agentPtr) {
    return _agentPtr->GetUniqueID();
  } else {
    return 0;
  }
}

uint64_t pinpoint_start_time(void) {
  if (_agentPtr && _agentPtr->GetStartTime() != 0) {
    return _agentPtr->GetStartTime();
  }
  return get_unix_time_ms();
}

void catch_error(NodeID id, const char* msg, const char* error_filename, uint32_t error_lineno) {
  if (_agentPtr) {
    try {
      _agentPtr->AnnotateErrorTrace(id, msg, error_filename, error_lineno);
    } catch (const std::out_of_range& ex) {
      pp_trace(" %s [%d] failed with %s", __func__, id, ex.what());
    } catch (const std::runtime_error& ex) {
      pp_trace(" %s [%d] failed with %s", __func__, id, ex.what());
    } catch (const std::exception& ex) {
      pp_trace(" %s [%d] failed with %s", __func__, id, ex.what());
    }
  }
}

void register_span_handler(void (*handler)(const char*)) {
  if (_agentPtr) {
    _agentPtr->RegisterRawSpanHandler(handler);
  }
}

void reset_unique_id(void) {
  if (_agentPtr) {
    _agentPtr->ResetUniqueID();
  }
}

void show_status(void) {
  if (_agentPtr) {
    fprintf(stderr, "%s\n", _agentPtr->GetNodePoolStatus().c_str());
  }
}

void debug_nodeid(NodeID id) {
  if (_agentPtr) {
    _agentPtr->DebugNodeId(id);
  }
}

const char* pinpoint_agent_version() {
#ifdef AGENT_VERSION
  static const char* _version_ = AGENT_VERSION;
#else
  static const char* _version_ = PINPOINT_C_AGENT_API_VERSION;
#endif
  return _version_;
}

uint64_t get_unix_time_ms() {
  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  time_point<system_clock, milliseconds> current = time_point_cast<std::chrono::milliseconds>(now);
  return current.time_since_epoch().count();
}