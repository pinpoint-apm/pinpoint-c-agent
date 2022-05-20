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
#ifndef UINT32_MAX
#define UINT32_MAX (0xfffffff)
#endif

namespace NodePool
{

    static std::atomic<int32_t> _Global_ID(0);

    //     uint32_t generateNid()
    //     {
    //         int32_t _int_max = INT32_MAX;
    //         _Global_ID.compare_exchange_weak(_int_max, 0);
    // #ifdef COMMON_DEBUG
    //         {
    //             NodeID _id = _Global_ID;
    //             pp_trace("last global id is %u", _id);
    //         }
    // #endif
    //         return ++_Global_ID;
    //     }

    void PoolManager::freeNode(TraceNode &node)
    {
        this->freeNode(node.getId());
    }

    void PoolManager::freeNode(NodeID id)
    {
        int32_t index = (int32_t)id - 1;

        std::lock_guard<std::mutex> _safe(this->_lock);
        if (this->_aliveNodeSet[index] == false)
        {
            pp_trace("%d not alive !!!", id);
#ifndef NDEBUG
            throw std::runtime_error("input is invalid");
#endif
            return;
        }
        this->_aliveNodeSet[index] = false;

        this->_freeNodeList.push(index);
    }

    TraceNode &PoolManager::getNodeById(NodeID id)
    {
        // assert(id != E_INVALID_NODE);
        if (id == E_ROOT_NODE)
        {
            throw std::out_of_range("id should not be 0");
        }

        std::lock_guard<std::mutex> _safe(this->_lock);
        int32_t index = int32_t(id) - 1;

        if (this->nodeIsAlive(index) == false)
        {
            std::string msg = "#";
            msg += std::to_string(id) + " is not alive";
            throw std::out_of_range(msg);
        }

        return this->nodeIndexVec[index / CELL_SIZE][index % CELL_SIZE];
    }

    TraceNode &PoolManager::getNode()
    {
        std::lock_guard<std::mutex> _safe(this->_lock);

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

    void PoolManager::expandOnce()
    {
        ADDTRACE();
        pp_trace("Node pool expanding self! Old size:%ld", this->nodeIndexVec.size() * CELL_SIZE);
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
        pp_trace("Node pool expanding is done! news size:%ld", this->nodeIndexVec.size() * CELL_SIZE);
        assert(this->nodeIndexVec.size() * CELL_SIZE == this->_aliveNodeSet.size());
    }

}
