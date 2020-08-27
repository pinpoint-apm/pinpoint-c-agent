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
#include <map>
#include <set>
#include <list>
#include <mutex>

namespace NodePool{

typedef  std::map<NodeID,TraceNode>::iterator NodeIter ;

class PoolManager
{

public:
    TraceNode& refNodeById(NodeID id);
    void giveBack(NodeID id);
    void giveBack(TraceNode&);
    TraceNode& getFreeNode();

    uint32_t poolNodeCount() 
    {
        std::lock_guard<std::mutex> _safe(this->_lock);
        return this->nodePool.size();
    }

    PoolManager(){}
    virtual ~PoolManager(){}
    
private:
    inline bool nodeIsAlive(NodeID id)
    {
        return this->_aliveNode.find(id) !=  this->_aliveNode.end();
    }


private:
    std::map<NodeID,TraceNode> nodePool;
    std::set<uint32_t> _aliveNode;
    std::list<uint32_t> _freeNode;
    std::mutex _lock;
};


uint32_t generateNid();

}


#endif /* COMMON_SRC_NODEPOOL_POOLMANAGER_H_ */
