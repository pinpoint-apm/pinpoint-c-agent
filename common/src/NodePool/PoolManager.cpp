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
 * NodePool.cpp
 *
 *  Created on: Aug 19, 2020
 *      Author: eeliu
 */

#include "PoolManager.h"

#include "common.h"
#include <cassert>
#include <thread>
#ifndef UINT32_MAX
#define UINT32_MAX (0xfffffff)
#endif
namespace PP {
namespace NodePool {
void PoolManager::FreeNodeTree(NodeID nodeId) {
  if (nodeId == E_INVALID_NODE || nodeId == E_ROOT_NODE) {
    return;
  }

  NodeID child_id, next_id;

  if (ReturnNode(nodeId, child_id, next_id)) {
    if (next_id != E_INVALID_NODE) {
      FreeNodeTree(next_id);
    }

    if (child_id != E_INVALID_NODE) {
      FreeNodeTree(child_id);
    }
  }
}
bool PoolManager::ReturnNode(NodeID id, NodeID& child_id, NodeID& next_id) {
  for (int i = 0; i < 1000; i++) {
    // this node was in using: ref is not zero
    if (this->_restore(id, child_id, next_id, false)) {
      return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  pp_trace("[🐛]Restore node failed:  [%d]; node restore forcefully", id);
  return this->_restore(id, child_id, next_id, true);
}

// avoiding `locking and waiting`
bool PoolManager::_restore(NodeID id, NodeID& child_id, NodeID& next_id, bool force) {
  std::lock_guard<std::mutex> _safe(this->_lock);

  int32_t index = (int32_t)id - 1;

  if (this->indexInUsedVec(index) == false) {
    pp_trace("%d not alive !!!", id);
    child_id = E_INVALID_NODE;
    next_id = E_INVALID_NODE;
    return true;
  }

  // check refcount
  TraceNode& node = this->getUsedNode(id);

  if (node.checkZeroRef() == false && force == false) {
    // DO NOT TOUCH THis Node
    return false;
  } else {
    this->usedNodeSet_[index] = false;
    child_id = node.last_child_id_;
    next_id = node.sibling_id_;
    this->_freeNodeList.push(index);
    return true;
  }
}

TraceNode& PoolManager::getUsedNode(NodeID id) {
  // assert(id != E_INVALID_NODE);
  if (id == E_ROOT_NODE) {
    throw std::out_of_range("id should not be 0");
  }

  int32_t index = int32_t(id) - 1;

  if (this->indexInUsedVec(index) == false) {
    std::string msg = "#";
    msg += std::to_string(id) + " is not alive";
    throw std::out_of_range(msg);
  }

  return this->nodeIndexVec[index / CELL_SIZE][index % CELL_SIZE];
}

TraceNode& PoolManager::getReadyNode() noexcept { // create a new node
  if (this->_freeNodeList.empty()) {
    this->expandOnce();
  }
  // as it holds a _lock, so no more _freeNodeList is empty
  int32_t index = this->_freeNodeList.top();
  this->_freeNodeList.pop();
  this->usedNodeSet_[index] = true;
  return this->nodeIndexVec[index / CELL_SIZE][index % CELL_SIZE].reset(NodeID(index + 1));
}

TraceNode& PoolManager::_take(NodeID id) {
  if (id != E_ROOT_NODE) {
    return this->getUsedNode(id);
  } else {
    return this->getReadyNode();
  }
}

void PoolManager::expandOnce() {
  // pp_trace("Node pool expanding self! Old size:%ld", this->nodeIndexVec.size() * CELL_SIZE);
  // append new nodes into nodeIndexVec
  this->nodeIndexVec.push_back(std::unique_ptr<TraceNode[]>(new TraceNode[CELL_SIZE]));
  // this->nodeIndexVec.push_back(std::make_unique<TraceNode[]>(CELL_SIZE));
  // append new bitflag into aliveNodeSet
  this->usedNodeSet_.insert(this->usedNodeSet_.end(), this->readyNodeSet_.begin(),
                            this->readyNodeSet_.end());
  for (int32_t id = this->maxId; id < (this->maxId + CELL_SIZE); id++) {
    this->_freeNodeList.push(id);
  }
  this->maxId += CELL_SIZE;
  // pp_trace("Node pool expanding is done! news size:%ld", this->nodeIndexVec.size() * CELL_SIZE);
  assert(this->nodeIndexVec.size() * CELL_SIZE == this->usedNodeSet_.size());
}
Json::Value empty(Json::nullValue);
Json::Value& PoolManager::getRootNodeValue(WrapperTraceNodePtr& node) {

  if (node->sibling_id_ != E_INVALID_NODE) {
    WrapperTraceNodePtr sibling = ReferNode(node->sibling_id_);
    getRootNodeValue(sibling);
  }

  if (node->last_child_id_ != E_INVALID_NODE) {
    WrapperTraceNodePtr child = ReferNode(node->last_child_id_);
    getRootNodeValue(child);
  }

  if (node->checkOpt() == false) {
    return empty;
  }

  if (node->parent_id_ > E_ROOT_NODE) {
    WrapperTraceNodePtr parent = ReferNode(node->parent_id_);
    parent->appendNodeValue("calls", node->EncodeProtocol());
  }

  return node->EncodeProtocol();
}

} // namespace NodePool
} // namespace PP