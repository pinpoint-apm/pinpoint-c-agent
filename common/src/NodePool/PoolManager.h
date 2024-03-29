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

#include "common.h"
#include "TraceNode.h"
#include "json/value.h"
#include <vector>
#include <mutex>
namespace PP {
namespace NodePool {
class PoolManager {
private:
  TraceNode& getUsedNode(NodeID id);

  TraceNode& getReadyNode(void) noexcept;

  TraceNode& _take(NodeID id);

  bool _restore(NodeID id, NodeID& child_id, NodeID& next_id, bool force);

  Json::Value& getRootNodeValue(WrapperTraceNodePtr& node);

  // void gatherChildrenValues(WrapperTraceNodePtr& parent, NodeID child_header);

public:
  /**
   * @brief take is not safe, you should avoid use under MT
   * note: try PoolManager::getInstance().GetWrapperNode
   * @param id
   * @return TraceNode&
   */
  DEPRECATED("") inline TraceNode& Take(NodeID id = E_ROOT_NODE) {
    std::lock_guard<std::mutex> _safe(this->_lock);
    return this->_take(id);
  }

  inline TraceNode& NewNode() {
    std::lock_guard<std::mutex> _safe(this->_lock);
    return this->getReadyNode();
  }

  inline WrapperTraceNodePtr ReferNode(NodeID id) {
    std::lock_guard<std::mutex> _safe(this->_lock);
    TraceNode& e = this->getUsedNode(id);
    return WrapperTraceNodePtr(e);
  }

  DEPRECATED("") inline WrapperTraceNodePtr GetWrapperNode(NodeID id = E_ROOT_NODE) {
    std::lock_guard<std::mutex> _safe(this->_lock);
    TraceNode& e = this->_take(id);
    return WrapperTraceNodePtr(e);
  }
  /**
   * @brief restore id->traceNode to pool
   *
   * @param id
   * @param child_id child id of `id`, if exist
   * @param next_id next id of `id`, if exist
   * @return true
   * @return false
   */
  bool ReturnNode(NodeID id, NodeID& child_id, NodeID& next_id);

  inline bool ReturnNode(const TraceNode& node) {
    NodeID node1, node2;
    return this->ReturnNode(node.getId(), node1, node2);
  }

  uint32_t totalNodesCount() {
    std::lock_guard<std::mutex> _safe(this->_lock);
    return (uint32_t)nodeIndexVec.size() * CELL_SIZE;
  }

  uint32_t freeNodesCount() {
    std::lock_guard<std::mutex> _safe(this->_lock);
    return (uint32_t)this->_freeNodeList.size();
  }

  void foreachAliveNode(std::function<void(TraceNode& node)> func) {
    std::lock_guard<std::mutex> _safe(this->_lock);
    for (int32_t index = 0; index < this->maxId; index++) {
      if (this->indexInUsedVec(index)) {
        func(this->getUsedNode((NodeID)(index + 1)));
      }
    }
  }

  Json::Value& ExpandTraceTreeNodes(NodeID id) {
    WrapperTraceNodePtr root = ReferNode(id);
    return ExpandTraceTreeNodes(root);
  }

  Json::Value& ExpandTraceTreeNodes(WrapperTraceNodePtr& w_root) {
    return getRootNodeValue(w_root);
  }
  Json::Value& ExpandTraceTreeNodes(TraceNode& root) {
    WrapperTraceNodePtr w_root(root);
    return getRootNodeValue(w_root);
  }

  void FreeNodeTree(NodeID nodeId);

  std::string Status() {
    Json::Value status;
    status["pool_total_node"] = totalNodesCount();
    status["pool_free_node"] = freeNodesCount();
    status["common_libary_version"] = pinpoint_agent_version();

    auto add_alive_node_fun = [&status](TraceNode& node) {
      status["pool_alive_nodes"].append(node.id_);
    };

    foreachAliveNode(std::bind(add_alive_node_fun, std::placeholders::_1));
    return status.toStyledString();
  }

#ifdef COMMON_DEBUG
  inline bool NoNodeLeak() {
    std::lock_guard<std::mutex> _safe(this->_lock);
    return this->_freeNodeList.size() == nodeIndexVec.size() * CELL_SIZE;
  }
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
  std::mutex _lock;
  // std::set<NodeID> _aliveNodeSet;
  std::vector<bool> usedNodeSet_;
  std::vector<bool> readyNodeSet_;
  int32_t maxId;
  std::stack<int32_t> _freeNodeList;
  static const int CELL_SIZE = 128;
  std::vector<std::unique_ptr<TraceNode[]>> nodeIndexVec;
};
} // namespace NodePool
} // namespace PP
#endif /* COMMON_SRC_NODEPOOL_POOLMANAGER_H_ */
