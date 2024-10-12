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
 * @author eeliu
 * Aug 19, 2020 5:59:52 PM
 */

#ifndef COMMON_SRC_NODEPOOL_POOLMANAGER_H_
#define COMMON_SRC_NODEPOOL_POOLMANAGER_H_

#include "pinpoint/common.h"
#include "TraceNode.h"
#include "json/value.h"
#include <vector>
#include <mutex>
namespace PP {
namespace NodePool {
class PoolManager {
private:
  TraceNode& getUsedNode(NodeID id);

  TraceNode& getReadyNode(void);

protected:
  virtual bool returnNode(NodeID id, NodeID& next, bool force);

public:
  const Json::Value& EncodeTraceToJsonSpan(WrapperTraceNodePtr& node);

  virtual TraceNode& GetNode() { return this->getReadyNode(); }

  virtual WrapperTraceNodePtr ReferNode(NodeID id) {
    TraceNode& e = this->getUsedNode(id);
    return WrapperTraceNodePtr(e);
  }

  void AppendToRootTrace(WrapperTraceNodePtr& root, TraceNode& newNode);

  NodeID ReturnNode(NodeID id);

  virtual uint32_t totalNodesCount() { return (uint32_t)nodeIndexVec.size() * CELL_SIZE; }

  virtual uint32_t freeNodesCount() { return (uint32_t)this->_freeNodeList.size(); }

  bool NoNodesLeak() {
    if (freeNodesCount() == totalNodesCount()) {
      return true;
    } else {
      pp_trace("pool status: free:%u total:%u", freeNodesCount(), totalNodesCount());
      return false;
    }
  }

  virtual void foreachAliveNode(std::function<void(TraceNode& node)> func) {
    for (int32_t index = 0; index < this->maxId; index++) {
      if (this->indexInUsedVec(index)) {
        func(this->getUsedNode((NodeID)(index + 1)));
      }
    }
  }

  void FreeNodeTree(NodeID nodeId);

  std::string Status() {
    Json::Value status;
    status["pool_total_node"] = totalNodesCount();
    status["pool_free_node"] = freeNodesCount();
    status["common_library_version"] = pinpoint_agent_version();

    auto add_alive_node_fun = [&status](TraceNode& node) {
      status["pool_alive_nodes"].append(node.id_);
    };

    foreachAliveNode(std::bind(add_alive_node_fun, std::placeholders::_1));
    return status.toStyledString();
  }

#ifdef COMMON_DEBUG
  inline bool NoNodeLeak() { return this->_freeNodeList.size() == nodeIndexVec.size() * CELL_SIZE; }
#endif
  virtual ~PoolManager() {}

public:
  PoolManager() : maxId(E_ROOT_NODE) {
    this->readyNodeSet_.reserve(CELL_SIZE);
    for (int i = 0; i < CELL_SIZE; i++) {
      this->readyNodeSet_.push_back(false);
    }
    this->expandOnce();
  }

private:
  inline bool indexInUsedVec(int32_t index) {
    if (index >= 0 && index < this->maxId) {
      return this->usedNodeSet_.at(index);
    }
    return false;
  }

  void expandOnce();

private:
  // std::mutex _lock;
  std::vector<bool> usedNodeSet_;
  std::vector<bool> readyNodeSet_;
  int32_t maxId;
  std::stack<int32_t> _freeNodeList;
  static const int CELL_SIZE = 128;
  static const int POOL_MAX_NODES_LIMIT = CELL_SIZE * 100;
  std::vector<std::unique_ptr<TraceNode[]>> nodeIndexVec;
};

class ThreadSafePoolManager : public PoolManager {
public:
  void foreachAliveNode(std::function<void(TraceNode& node)> func) override {
    std::lock_guard<std::mutex> _safe(this->_lock);
    PoolManager::foreachAliveNode(func);
  }
  uint32_t freeNodesCount() override {
    std::lock_guard<std::mutex> _safe(this->_lock);
    return PoolManager::freeNodesCount();
  }

  uint32_t totalNodesCount() override {
    std::lock_guard<std::mutex> _safe(this->_lock);
    return PoolManager::totalNodesCount();
  }

  WrapperTraceNodePtr ReferNode(NodeID id) override {
    std::lock_guard<std::mutex> _safe(this->_lock);
    return PoolManager::ReferNode(id);
  }

  TraceNode& GetNode() override {
    std::lock_guard<std::mutex> _safe(this->_lock);
    return PoolManager::GetNode();
  }

  // private:
  bool returnNode(NodeID id, NodeID& next, bool force) override {
    std::lock_guard<std::mutex> _safe(this->_lock);
    return PoolManager::returnNode(id, next, force);
  }

private:
  std::mutex _lock;
};

} // namespace NodePool
} // namespace PP
#endif /* COMMON_SRC_NODEPOOL_POOLMANAGER_H_ */
