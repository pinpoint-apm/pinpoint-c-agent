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

namespace NodePool{

class PoolManager
{

public:

    TraceNode& getNodeById(NodeID id);

    void freeNode(NodeID id);

    void freeNode(TraceNode&);

    TraceNode& getNode();

    uint32_t totoalNodesCount() 
    {
        std::lock_guard<std::mutex> _safe(this->_lock);
        return nodeIndexVec.size()*CELL_SIZE;
    }

    uint32_t freeNodesCount() 
    {
        std::lock_guard<std::mutex> _safe(this->_lock);
        return this->_freeNodeList.size();
    }

    PoolManager():maxId(0){
        this->_emptyAliveSet.reserve(CELL_SIZE);
        for(int i =0;i<CELL_SIZE;i++)
        {
            this->_emptyAliveSet.push_back(false);
        }

        this->expandOnce();
    }
    virtual ~PoolManager(){}
    
private:
    inline bool nodeIsAlive(NodeID id)
    {

        return this->_aliveNodeSet[id];
    }

    void expandOnce();

private:
    std::mutex _lock;
    // std::set<NodeID> _aliveNodeSet;
    std::vector<bool> _aliveNodeSet;
    std::vector<bool> _emptyAliveSet;
    NodeID maxId;
    std::stack<NodeID> _freeNodeList;
    static const int CELL_SIZE = 128;
    std::vector<std::unique_ptr<TraceNode[]>> nodeIndexVec;
};


uint32_t generateNid();

}


#endif /* COMMON_SRC_NODEPOOL_POOLMANAGER_H_ */
