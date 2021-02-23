/*******************************************************************************
 * Copyright 2019 NAVER Corp
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations under
 * the License.
 ******************************************************************************/
//
// Created by eeliu on 1/3/2020.
//

#include <cstdio>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <thread>

#include "common.h"

#include "Cache/SafeSharedState.h"
#include "NodePool/PoolManager.h"
#include "Util/Helper.h"
#include "ConnectionPool/SpanConnectionPool.h"

using NodePool::TraceNode;
using NodePool::PoolManager;
using NodePool::PContextType;
using ConnectionPool::TransConnection;
using Cache::SafeSharedState;
using Helper::get_current_msec_stamp;
static NodePool::PoolManager g_node_pool;
const static char* CLUSE="clues";
static NodeID do_end_trace(NodeID&);
static NodeID do_end_trace(TraceNode&);

static thread_local NodeID __tls_id = 0;
// send current span with timeout
static const int SPAN_TIMEOUT_MS=0;
static thread_local int _span_timeout = SPAN_TIMEOUT_MS;

NodeID pinpoint_get_per_thread_id()
{
    return __tls_id;
}

void pinpoint_update_per_thread_id(NodeID id)
{
    __tls_id = id;
}


static NodeID do_start_trace(NodeID& _id)
{
    uint64_t time_in_ms = Helper::get_current_msec_stamp();
    if(_id == 0)  // it's a root node
    {
        TraceNode&  node =  g_node_pool.getNode();
        node["S"] = time_in_ms;
        node["FT"] = global_agent_info.agent_type;
        node.start_time = time_in_ms;
        return node.getId();
    }else 
    {
        TraceNode&  parent = g_node_pool.getNodeById(_id);
        TraceNode&  child  = g_node_pool.getNode();
        parent.addChild(child);
        assert(child.p_root_node);
        child["S"] = time_in_ms - child.p_root_node->start_time;
        child.start_time = time_in_ms;
        return child.getId();
    }
}

// root must be the trace root
// if not, your nodes must be leaked !!!
static void free_nodes_tree(TraceNode* root)
{
    if(root == nullptr) return ;

    TraceNode * p_child = root->p_child_head;
    while (p_child) // free all children
    {
        // keep the next child
        TraceNode * p_bro = p_child->p_brother_node;
        // free current child tree
        free_nodes_tree(p_child);
        // go on
        p_child = p_bro;
    }
    // free self
    g_node_pool.freeNode(*root);
}

static void flush_to_agent(std::string& span)
{
    TransConnection trans = Helper::getConnection();

    if(span.length() < MAX_SPAN_SIZE){
        trans->sendMsgToAgent(span);
    }
    else{
        pp_trace("drop current span as it's too heavy! size:%d",span.length());
    }
    trans->trans_layer_pool(_span_timeout);
    Helper::freeConnection(trans);
}

NodeID do_end_trace(TraceNode& node)
{
    if(node.isRoot())
    {
        assert(node.p_root_node);
        /// this trace is end. Try to send current trace
        if( node.limit == E_TRACE_PASS ){
            uint64_t end_time = (node.p_root_node->fetal_error_time == 0) ? 
                                (Helper::get_current_msec_stamp()):
                                (node.p_root_node->fetal_error_time);
            node["E"] = end_time - node.start_time;
            const Json::Value& trace = Helper::merge_node_tree(node);
            std::string spanStr = Helper::node_tree_to_string(trace);
            pp_trace("this span:(%s)",spanStr.c_str());
            flush_to_agent(spanStr);

        }else if( node.limit == E_TRACE_BLOCK ){
            pp_trace("current#%ld span dropped,due to TRACE_BLOCK",node.getId());
        }else{
            pp_trace("current#%ld span dropped,due to limit=%d",node.getId(),node.limit);
        }
        /// this span is done, reset the trace node tree
        free_nodes_tree(&node);
        return 0;
    }else
    {
        assert(node.p_root_node);
        uint64_t end_time = (node.p_root_node->fetal_error_time == 0) ? 
                                (Helper::get_current_msec_stamp()):
                                (node.p_root_node->fetal_error_time);
        node["E"] = end_time - node.start_time;
        return node.p_parent_node->getId();
    }
}



NodeID do_end_trace(NodeID& _id)
{
    TraceNode&  node = g_node_pool.getNodeById(_id);
    return do_end_trace(node);
}

NodeID pinpoint_start_trace(NodeID _id)
{
    try
    {
        pp_trace("#%ld pinpoint_start start",_id);
        return do_start_trace(_id);
    }
    catch(const std::out_of_range& ex)
    {
        pp_trace(" start_trace#%ld failed with %s",_id,ex.what());
    }
    catch(const std::runtime_error& ex)
    {
        pp_trace(" start_trace#%ld failed with %s",_id,ex.what());
    }catch(...)
    {
        pp_trace(" start_trace#%ld failed with unkonw reason",_id);
    }
    return 0;
}

int pinpoint_force_end_trace(NodeID id,int32_t timeout)
{
    try
    {   
        _span_timeout = timeout;
        while(id != 0){
            id = pinpoint_end_trace(id);
        }
        _span_timeout = SPAN_TIMEOUT_MS;
        return 0;
    }catch(const std::out_of_range&){
        pp_trace("#%ld not found",id);
    }catch(const std::exception &ex){
        pp_trace("#%ld end trace failed. %s",id,ex.what());
    }
    return -1;
}

int pinpoint_trace_is_root(NodeID _id)
{
    try
    {
        TraceNode&  node = g_node_pool.getNodeById(_id);
        return node.isRoot()?(1):(0);
    }catch(const std::out_of_range&){
        pp_trace("#%ld not found",_id);
        return -1;
    }catch(const std::exception &ex){
        pp_trace("#%ld end trace failed. %s",_id,ex.what());
        return -1;
    }
}

NodeID pinpoint_end_trace(NodeID _id)
{
    try
    {   
        NodeID ret = do_end_trace(_id);
        pp_trace("#%ld pinpoint_end_trace Done!",_id);
        return ret;
    }catch(const std::out_of_range&){
        pp_trace("#%ld not found",_id);
        return 0;
    }catch(const std::exception &ex){
        pp_trace("#%ld end trace failed. %s",_id,ex.what());
        return 0;
    }
}

uint64_t pinpoint_start_time(void)
{
    return (uint64_t)SafeSharedState::instance().getStartTime();
}


int64_t generate_unique_id()
{
    return SafeSharedState::instance().generateUniqueId();
}

void reset_unique_id(void)
{

    return SafeSharedState::instance().resetUniqueId();
}

bool do_mark_current_trace_status(NodeID& _id,E_AGENT_STATUS status) 
{
    TraceNode& node = g_node_pool.getNodeById(_id);
    assert(node.p_root_node);
    TraceNode* root = node.p_root_node;
    pp_trace("change current#%ld status, before:%d,now:%d",root->getId(),root->limit,status);
    root->limit = status;
    return true;
}

int mark_current_trace_status(NodeID _id,int status)
{
    try
    {
        return do_mark_current_trace_status(_id,(E_AGENT_STATUS)status)?(0):(1);
    }
    catch(const std::out_of_range& ex)
    {
        pp_trace(" %s#%ld failed with %s",__func__,_id,ex.what());
    }
    catch(const std::runtime_error& ex)
    {
        pp_trace(" %s#%ld failed with %s",__func__,_id,ex.what());
    }catch(...)
    {
        pp_trace(" %s#%ld failed with unkonw reason",__func__,_id);
    }
    return 1;
}


int check_tracelimit(int64_t timestamp)
{
    return SafeSharedState::instance().checkTraceLimit(timestamp)?(1):(0);
}

static inline TraceNode& parse_flag(NodeID _id,E_NODE_LOC flag)
{
    TraceNode& node = g_node_pool.getNodeById(_id);
    if(flag == E_ROOT_LOC){
        assert(node.p_root_node);
        return *node.p_root_node;
    }else{
        return node;
    }
}


static void do_add_clue(NodeID _id,const  char* key,const  char* value,E_NODE_LOC flag)
{
    TraceNode& node = parse_flag(_id,flag);
    node[key]=value;
    pp_trace("#%ld add clue key:%s value:%s",_id,key,value);
}

static void do_add_clues(NodeID _id,const  char* key,const  char* value,E_NODE_LOC flag)
{
    TraceNode& node = parse_flag(_id,flag);
    std::string cvalue ="";
    cvalue+=key;
    cvalue+=':';
    cvalue+=value;
    node[CLUSE].append(cvalue);
    pp_trace("#%ld add clues:%s:%s",_id,key,value);
}

void pinpoint_add_clues(NodeID _id,const  char* key,const  char* value,E_NODE_LOC flag)
{
    try
    {
        do_add_clues(_id,key,value,flag);
    }
    catch(const std::out_of_range& ex)
    {
        pp_trace(" %s#%ld failed.Reason %s,parameters:%s:%s",__func__,_id,ex.what(),key,value);
    }
    catch(const std::runtime_error& ex)
    {
        pp_trace(" %s#%ld failed.Reason %s,parameters:%s:%s",__func__,_id,ex.what(),key,value);
    }catch(...)
    {
        pp_trace(" %s#%ld failed.Reason: unknown reason,parameters:%s:%s",__func__,_id,key,value);
    }
}

void pinpoint_add_clue(NodeID _id,const  char* key,const  char* value,E_NODE_LOC flag)
{
    try
    {
        do_add_clue(_id,key,value,flag);
    }
    catch(const std::out_of_range& ex)
    {
        pp_trace(" %s#%ld failed. Reason: %s,parameters:%s:%s",__func__,_id,ex.what(),key,value);
    }
    catch(const std::runtime_error& ex)
    {
        pp_trace(" %s#%ld failed. Reason: %s,parameters:%s:%s",__func__,_id,ex.what(),key,value);
    }catch(...)
    {
        pp_trace(" %s#%ld failed. Reason: unknow reason,parameters:%s:%s",__func__,_id,key,value);
    }
}

static inline void do_set_context_key(NodeID _id,const char* key,const char* value)
{
    TraceNode& node = g_node_pool.getNodeById(_id);
    assert(node.p_root_node);
    node.p_root_node->setStrContext(key,value);
}

static const char* do_get_context_key(NodeID _id,const char* key)
{
    TraceNode& node = g_node_pool.getNodeById(_id);
    assert(node.p_root_node);
    PContextType& pValue = node.p_root_node->getContextByKey(key);
    return pValue->asStringValue().c_str();
}

void pinpoint_set_context_key(NodeID _id,const char* key,const char* value)
{
    try
    {
        do_set_context_key(_id,key,value);
    }
    catch(const std::out_of_range& ex)
    {
        pp_trace(" %s#%ld failed with out_of_range. %s,parameters:%s:%s",__func__,_id,ex.what(),key,value);
    }
    catch(const std::runtime_error& ex)
    {
        pp_trace(" %s#%ld failed with runtime_error. %s,parameters:%s:%s",__func__,_id,ex.what(),key,value);
    }catch(const std::exception&ex)
    {
        pp_trace(" %s#%ld failed with %s,parameters:%s:%s",__func__,_id,ex.what(),key,value);
    }
}

static void do_set_long_key(NodeID _id,const char* key,long l) noexcept
{
    TraceNode& node = g_node_pool.getNodeById(_id);
    assert(node.p_root_node);
    node.p_root_node->setLongContext(key,l);
}

void pinpoint_set_context_long(NodeID _id,const char* key,long l)
{
    try
    {
        do_set_long_key(_id,key,l);
    }
    catch(const std::out_of_range& ex)
    {
        pp_trace(" %s#%ld failed with %s",__func__,_id,ex.what());
    }
    catch(const std::runtime_error& ex)
    {
        pp_trace(" %s#%ld failed with %s",__func__,_id,ex.what());
    }catch(const std::exception&ex)
    {
        pp_trace(" %s#%ld failed with %s",__func__,_id,ex.what());
    }
}

static int do_get_long_key(NodeID _id,const char* key,long* l)
{
    TraceNode& node = g_node_pool.getNodeById(_id);
    assert(node.p_root_node);
    PContextType& pValue = node.p_root_node->getContextByKey(key);
    *l = pValue->asLongValue();
    return 0;
}

int pinpoint_get_context_long(NodeID _id,const char* key,long* l)
{
    try
    {
        do_get_long_key(_id,key,l);
        return 0;
    }
    catch(const std::out_of_range& ex)
    {
        pp_trace(" %s#%ld failed with %s",__func__,_id,ex.what());
    }
    catch(const std::runtime_error& ex)
    {
        pp_trace(" %s#%ld failed with %s",__func__,_id,ex.what());
    }catch(const std::exception&ex)
    {
        pp_trace(" %s#%ld failed with %s",__func__,_id,ex.what());
    }
    return 1;
}

const char* pinpoint_get_context_key(NodeID _id,const char* key)
{
    try
    {
        return do_get_context_key(_id,key);
    }
    catch(const std::out_of_range& ex)
    {
        pp_trace(" %s#%ld failed with %s, parameters:%s",__func__,_id,ex.what(),key);
    }
    catch(const std::runtime_error& ex)
    {
        pp_trace(" %s#%ld failed with %s, parameters:%s",__func__,_id,ex.what(),key);
    }catch(const std::exception&ex)
    {
        pp_trace(" %s#%ld failed with %s, parameters:%s",__func__,_id,ex.what(),key);
    }
    return nullptr;
}

void do_catch_error(NodeID _id,const char* msg,const char* error_filename,uint32_t error_lineno)
{
    TraceNode& node = g_node_pool.getNodeById(_id);
    TraceNode* root = node.p_root_node;
    if(root == nullptr) throw std::runtime_error("traceNode tree is crash");
    Json::Value eMsg;
    eMsg["msg"] = msg;
    eMsg["file"] = error_filename;
    eMsg["line"] = error_lineno;
    (*root)["ERR"] = eMsg;
}


void catch_error(NodeID _id,const char* msg,const char* error_filename,uint32_t error_lineno)
{
    try
    {
        do_catch_error(_id,msg,error_filename,error_lineno);
    }
    catch(const std::out_of_range& ex)
    {
        pp_trace(" %s#%ld failed with %s",__func__,_id,ex.what());
    }
    catch(const std::runtime_error& ex)
    {
        pp_trace(" %s#%ld failed with %s",__func__,_id,ex.what());
    }catch(const std::exception& ex)
    {
        pp_trace(" %s#%ld failed with %s",__func__,_id,ex.what());
    }
}
