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
#include <utility>

#include "PoolManager.h"

#include "../Util/Helper.h"
#include "common.h"
#include <thread>
#ifndef UINT32_MAX
#define UINT32_MAX (0xfffffff)
#endif

namespace NodePool
{
    void freeNodeTree(NodeID rootId)
    {
        if (rootId == E_INVALID_NODE || rootId == E_ROOT_NODE)
        {
            return;
        }

        NodeID child_id = PoolManager::getInstance().Restore(rootId);
        while (child_id != E_INVALID_NODE)
        {
            TraceNode &child = PoolManager::getInstance().Take(child_id);
            child_id = child.mNextId;
            freeNodeTree(child.mPoolIndex);
        }
    }

    NodeID PoolManager::Restore(NodeID id)
    {
        NodeID child = E_INVALID_NODE;
        for (int i = 0; i < 1000; i++)
        {
            if (this->_restore(id, child, false))
            {
                return child;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        pp_trace("[🐛]Restore node failed: #%d; node restore forcefully", id);
        this->_restore(id, child, true);
        return child;
    }

    // avoiding `locking and waitting`
    bool PoolManager::_restore(NodeID id, NodeID &child, bool force)
    {
        std::lock_guard<std::mutex> _safe(this->_lock);

        int32_t index = (int32_t)id - 1;

        if (this->indexInAliveVec(index) == false)
        {
            pp_trace("%d not alive !!!", id);
            child = E_INVALID_NODE;
            return true;
        }

        // check refcount
        TraceNode &node = this->_fetchNodeBy(id);

        if (node.checkZoreRef() == false && force == false)
        {
            return false;
        }
        else
        {
            this->_aliveNodeSet[index] = false;
            child = node.mChildHeadIndex;
            this->_freeNodeList.push(index);
            return true;
        }
    }

    TraceNode &PoolManager::_fetchNodeBy(NodeID id)
    {
        // assert(id != E_INVALID_NODE);
        if (id == E_ROOT_NODE)
        {
            throw std::out_of_range("id should not be 0");
        }

        int32_t index = int32_t(id) - 1;

        if (this->indexInAliveVec(index) == false)
        {
            std::string msg = "#";
            msg += std::to_string(id) + " is not alive";
            throw std::out_of_range(msg);
        }

        return this->nodeIndexVec[index / CELL_SIZE][index % CELL_SIZE];
    }

    TraceNode &PoolManager::_getInitNode() noexcept
    { // create a new node
        if (this->_freeNodeList.empty())
        {
            this->expandOnce();
        }
        // as it holds a _lock, so no more _freeNodeList is empty
        int32_t index = this->_freeNodeList.top();
        this->_freeNodeList.pop();
        this->_aliveNodeSet[index] = true;
        return this->nodeIndexVec[index / CELL_SIZE][index % CELL_SIZE].reset(NodeID(index + 1));
    }

    TraceNode &PoolManager::_take(NodeID id)
    {
        if (id != E_ROOT_NODE)
        {
            return this->_fetchNodeBy(id);
        }
        else
        {
            return this->_getInitNode();
        }
    }

    void PoolManager::expandOnce()
    {
        ADDTRACE();
        // pp_trace("Node pool expanding self! Old size:%ld", this->nodeIndexVec.size() * CELL_SIZE);
        // append new nodes into nodeIndexVec
        this->nodeIndexVec.push_back(std::unique_ptr<TraceNode[]>(new TraceNode[CELL_SIZE]));
        // this->nodeIndexVec.push_back(std::make_unique<TraceNode[]>(CELL_SIZE));
        // append new bitflag into aliveNodeSet
        this->_aliveNodeSet.insert(this->_aliveNodeSet.end(), this->_emptyAliveSet.begin(), this->_emptyAliveSet.end());
        for (int32_t id = this->maxId; id < (this->maxId + CELL_SIZE); id++)
        {
            this->_freeNodeList.push(id);
        }
        this->maxId += CELL_SIZE;
        // pp_trace("Node pool expanding is done! news size:%ld", this->nodeIndexVec.size() * CELL_SIZE);
        assert(this->nodeIndexVec.size() * CELL_SIZE == this->_aliveNodeSet.size());
    }
}
