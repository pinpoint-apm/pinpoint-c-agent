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
#include <utility>

#ifndef UINT32_MAX
#define UINT32_MAX (0xfffffff)
#endif

namespace NodePool{

static std::atomic<NodeID> _Global_ID(0);

uint32_t generateNid()
{
    NodeID _int_max = UINT32_MAX;
    _Global_ID.compare_exchange_weak(_int_max,0);
#ifdef COMMON_DEBUG
    {
        NodeID _id = _Global_ID;
        pp_trace("last global id is %u",_id);
    }
#endif
    return ++_Global_ID;
}

void PoolManager::giveBack(TraceNode& node)
{
    this->giveBack(node.getId());
}

void PoolManager::giveBack(NodeID id)
{
    std::lock_guard<std::mutex> _safe(this->_lock);

    if(!this->nodeIsAlive(id))
    {
        pp_trace("node id:[%d] is not alive,please check the id life-cycle ",id);
        return ;
    }

#ifdef COMMON_DEBUG
    {
        std::list<NodeID>::iterator it = this->_freeNode.begin();

        for(;it!= this->_freeNode.end();it++)
        {
            assert(*it != id );
        }
    }
#endif
    NodeIter it = this->nodePool.find(id);
    if(it!=this->nodePool.end() ){
        NodeID nid = generateNid();
        // update Node with nid
        it->second.reset(nid);
        // move to _node
        const TraceNode& _node = std::move(it->second);
        // erase key -> value
        this->nodePool.erase(it);
        // insert a nid with new _node
        this->nodePool.insert({nid,std::move(_node)});
        this->_freeNode.push_front(nid);
    }
    else{
        pp_trace("[error] %u not find in pool",id);
    }
}

TraceNode& PoolManager::refNodeById(NodeID id)
{
      std::lock_guard<std::mutex> _safe(this->_lock);

      if(!this->nodeIsAlive(id))
          throw std::out_of_range("id is not alive");

      NodeIter iter =   this->nodePool.find(id);

      if( iter ==  this->nodePool.end())   throw std::out_of_range("node id not found in pool");

      return iter->second;
}

 TraceNode& PoolManager::getFreeNode()
 {
     std::lock_guard<std::mutex> _safe(this->_lock);

     if(!this->_freeNode.empty()){ //use free node
        NodeID id = this->_freeNode.front();
        this->_freeNode.pop_front();

        NodeIter it = this->nodePool.find(id);
        if(it!=this->nodePool.end()){
            this->_aliveNode.insert(id);
            return it->second;
        }
        // throw std::invalid_argument(id);
     } else { // create a new node 
        NodeID id = generateNid();
        TraceNode node(id);
        this->nodePool.insert({id,node});
        this->_aliveNode.insert(id);
        NodeIter it =  this->nodePool.find(id);
        if(it!= this->nodePool.end()){
            return it->second;
        }
     }

     throw std::runtime_error("node pool locates node failed");
 }


}
