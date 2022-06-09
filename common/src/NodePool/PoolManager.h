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
#include <iostream>
#include <vector>
#include <set>
#include <list>
#include <mutex>

namespace NodePool
{
    class PoolManager
    {
    private:
        TraceNode &_fetchNodeBy(NodeID id);

        TraceNode &_getInitNode(void) noexcept;

        TraceNode &_take(NodeID id);

        bool _restore(NodeID id, NodeID &child, bool force);

    public:
        inline TraceNode &Take(NodeID id = E_ROOT_NODE)
        {
            std::lock_guard<std::mutex> _safe(this->_lock);
            return this->_take(id);
        }

        inline WrapperTraceNode GetWrapperNode(NodeID id = E_ROOT_NODE)
        {
            std::lock_guard<std::mutex> _safe(this->_lock);
            TraceNode &e = this->_take(id);
            return WrapperTraceNode(&e);
        }

        NodeID Restore(NodeID id);

        inline void Restore(TraceNode &node)
        {
            this->Restore(node.getId());
        }

        uint32_t totoalNodesCount()
        {
            std::lock_guard<std::mutex> _safe(this->_lock);
            return nodeIndexVec.size() * CELL_SIZE;
        }

        uint32_t freeNodesCount()
        {
            std::lock_guard<std::mutex> _safe(this->_lock);
            return this->_freeNodeList.size();
        }

        void foreachAliveNode(std::function<void(TraceNode &node)> func)
        {
            std::lock_guard<std::mutex> _safe(this->_lock);
            for (int32_t index = 0; index < this->maxId; index++)
            {
                if (this->indexInAliveVec(index))
                {
                    func(this->_fetchNodeBy((NodeID)(index + 1)));
                }
            }
        }

#ifdef COMMON_DEBUG
        inline bool NoNodeLeak()
        {
            std::lock_guard<std::mutex> _safe(this->_lock);
            return this->_freeNodeList.size() == nodeIndexVec.size() * CELL_SIZE;
        }
#endif
        virtual ~PoolManager()
        {
        }

    public:
        static PoolManager &getInstance()
        {
            static PoolManager _instance;
            return _instance;
        }
#ifndef COMMON_DEBUG
    private:
#endif
        PoolManager() : maxId(E_ROOT_NODE)
        {
            this->_emptyAliveSet.reserve(CELL_SIZE);
            for (int i = 0; i < CELL_SIZE; i++)
            {
                this->_emptyAliveSet.push_back(false);
            }
            this->expandOnce();
        }

    private:
        inline bool indexInAliveVec(int32_t index)
        {
            if (index >= 0 && index < this->maxId)
            {
                return this->_aliveNodeSet.at(index);
            }
            return false;
        }

        void expandOnce();

    private:
        std::mutex _lock;
        // std::set<NodeID> _aliveNodeSet;
        std::vector<bool> _aliveNodeSet;
        std::vector<bool> _emptyAliveSet;
        int32_t maxId;
        std::stack<int32_t> _freeNodeList;
        static const int CELL_SIZE = 128;
        std::vector<std::unique_ptr<TraceNode[]>> nodeIndexVec;
    };
    void freeNodeTree(NodeID root);
}

#endif /* COMMON_SRC_NODEPOOL_POOLMANAGER_H_ */
