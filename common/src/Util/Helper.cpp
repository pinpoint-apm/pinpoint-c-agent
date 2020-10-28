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
 * Trace.cpp
 *
 *  Created on: Sep 18, 2020
 *      Author: eeliu
 */
#include <mutex> 

#include "Cache/NodeTreeWriter.h"
#include "Helper.h"

namespace Helper{
using Cache::NodeTreeWriter;
static NodeTreeWriter _writer;
static ConnectionPool::SpanConnectionPool _con_pool;
static std::once_flag _pool_init_flag;

uint64_t get_current_msec_stamp()
{
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    time_point<system_clock,milliseconds> current= time_point_cast<std::chrono::milliseconds>(now);
    return current.time_since_epoch().count();
}


std::string node_tree_to_string(Json::Value &value)
{
    return _writer.write(value);
}

/**
 * @brief  as p_brother_node is a reverser list
 * 
 * @param parents 
 * @param head 
 */
static void reverse_node_list(Json::Value& parents,TraceNode* head)
{
    if(head == nullptr) return ;
        
    if(head->p_brother_node){
        reverse_node_list(parents,head->p_brother_node);
    }
    parents.append(merge_children(*head));
}

Json::Value merge_children(TraceNode& node)
{
    Json::Value& value = node.getValue();

    if(! node.isLeaf())
    {
        TraceNode * pstart = node.p_child_head;
        reverse_node_list(value["calls"],pstart);
    }

    return value;
}

Json::Value merge_node_tree(TraceNode& root)
{
    return merge_children(root);
}



TransConnection getConnection()
{
    
    std::call_once (_pool_init_flag,
        [](){_con_pool.initPool(global_agent_info.co_host,global_agent_info.timeout_ms);}
    );

    return _con_pool.get();
}

void freeConnection(TransConnection& trans)
{
  
    std::call_once (_pool_init_flag,
        [](){_con_pool.initPool(global_agent_info.co_host,global_agent_info.timeout_ms);}
    );
    
    return _con_pool.free(trans);
}


}


