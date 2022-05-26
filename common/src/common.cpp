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
#include <stdexcept>
#include <stdarg.h>
#include "common.h"

#include "Cache/SafeSharedState.h"
#include "NodePool/PoolManager.h"
#include "Util/Helper.h"
#include "ConnectionPool/SpanConnectionPool.h"
#include "define.h"
namespace Json = AliasJson;

using Cache::SafeSharedState;
using ConnectionPool::TransConnection;
using Helper::get_current_msec_stamp;
using NodePool::PContextType;
using NodePool::PoolManager;
using NodePool::TraceNode;

const static char *CLUSE = "clues";
static thread_local NodeID __tls_id = E_ROOT_NODE;
// send current span with timeout
static thread_local int _span_timeout = global_agent_info.timeout_ms;
static std::function<void(const char *)> _SpanHandler_;

NodeID pinpoint_get_per_thread_id()
{
    return __tls_id;
}

void pinpoint_update_per_thread_id(NodeID id)
{
    __tls_id = id;
}

static NodeID do_start_trace(NodeID id, const char *opt = nullptr, va_list *args = nullptr)
{
    if (id <= E_INVALID_NODE)
    {
        throw std::runtime_error("invalid node id");
    }
    else if (id == E_ROOT_NODE)
    {
        TraceNode &node = PoolManager::getInstance().GetNode();
        node.startTimer();
        return node.ID;
    }
    else
    {
        TraceNode &parent = PoolManager::getInstance().GetNode(id);
        TraceNode &node = PoolManager::getInstance().GetNode();
        node.startTimer();
        node.setTraceParent(parent);

        // pass opt
        if (opt != nullptr)
        {
            node.setOpt(opt, args);
        }

        return node.ID;
    }
}

static void free_nodes_tree(NodeID id)
{
    if (id == E_INVALID_NODE || id == E_ROOT_NODE)
    {
        return;
    }

    TraceNode &node = PoolManager::getInstance().GetNode(id);

    NodeID child_id = node.mChildListHeaderId;
    while (child_id != E_INVALID_NODE)
    {
        TraceNode &child = PoolManager::getInstance().GetNode(child_id);
        child_id = child.mNextId;
        free_nodes_tree(child.ID);
    }

    PoolManager::getInstance().freeNode(id);
}

static void flush_to_agent(std::string &span)
{
    TransConnection trans = Helper::getConnection();

    if (span.length() < MAX_SPAN_SIZE)
    {
        trans->copy_into_send_buffer(span);
    }
    else
    {
        pp_trace("drop current span as it's too heavy! size:%lu", span.length());
    }
    trans->trans_layer_pool(_span_timeout);
    // if network not ready, span will send in next time.
    Helper::freeConnection(trans);
}

NodeID do_end_trace(NodeID Id)
{
    TraceNode &node = PoolManager::getInstance().GetNode(Id);
    if (node.isRoot())
    {
        // assert(node.mRootId);
        /// this trace is end. Try to send current trace
        if (node.limit == E_TRACE_PASS)
        {
            node.endTimer();
            node.convertToSpan();
            const Json::Value trace = Helper::mergeTraceNodeTree(node);
            std::string spanStr = Helper::node_tree_to_string(trace);
            if (unlikely(_SpanHandler_ != nullptr))
            {
                _SpanHandler_(spanStr.c_str());
            }
            else
            {
                pp_trace("this span:(%s)", spanStr.c_str());
                flush_to_agent(spanStr);
            }
        }
        else if (node.limit == E_TRACE_BLOCK)
        {
            pp_trace("current#%d span dropped,due to TRACE_BLOCK", node.getId());
        }
        else
        {
            pp_trace("current#%d span dropped,due to limit=%ld", node.getId(), node.limit);
        }
        /// this span is done, reset the trace node tree
        free_nodes_tree(node.ID);
        return E_ROOT_NODE;
    }
    else
    {

        node.endTimer();
        node.convertToSpanEvent();

        NodeID parentId = node.startParentId;

        // check opt
        if (node.checkOpt() == true)
        {
            try
            {
                TraceNode &parent = PoolManager::getInstance().GetNode(parentId);
                parent.addChild(node);
            }
            catch (const std::out_of_range &ex)
            {
                pp_trace("current#%d dropped,due to parent is end", node.ID);
                PoolManager::getInstance().freeNode(node);
            }
        }
        else
        {
            pp_trace("current#%d dropped,due to checkOpt false", node.ID);
            PoolManager::getInstance().freeNode(node);
        }

        return parentId;
    }
}

// NodeID do_end_trace(NodeID &_id)
// {
//     TraceNode &node = PoolManager::getInstance().getNodeById(_id);
//     return do_end_trace(node);
// }

NodeID pinpoint_start_trace(NodeID parentId)
{
    try
    {
        NodeID childId = do_start_trace(parentId);
        pp_trace("#%d pinpoint_start child #%d", parentId, childId);
        return childId;
    }
    catch (const std::out_of_range &ex)
    {
        pp_trace(" start_trace#%d failed with %s", parentId, ex.what());
    }
    catch (const std::runtime_error &ex)
    {
        pp_trace(" start_trace#%d failed with %s", parentId, ex.what());
    }
    catch (const std::exception &ex)
    {
        pp_trace(" start_trace#%d failed with %s", parentId, ex.what());
    }
    return E_INVALID_NODE;
}

static NodeID do_wake_trace(NodeID &id)
{
    // routine
    // 1. check id alive
    // 2. check if it's a root node
    // 3. update start time

    TraceNode &trace_node = PoolManager::getInstance().GetNode(id);

    if (trace_node.isRoot())
    {
        pp_trace("#%d wake_trace failed, it's a root node", id);
        return E_INVALID_NODE;
    }

    trace_node.wake();

    return id;
}

int pinpoint_wake_trace(NodeID traceId)
{
    try
    {
        pp_trace("wake_trace #%d ", traceId);
        return do_wake_trace(traceId);
    }
    catch (const std::out_of_range &ex)
    {
        pp_trace(" wake_trace#%d failed with %s", traceId, ex.what());
    }
    catch (const std::runtime_error &ex)
    {
        pp_trace(" wake_trace#%d failed with %s", traceId, ex.what());
    }
    catch (...)
    {
        pp_trace(" wake_trace#%d failed with unkonw reason", traceId);
    }
    return 0;
}

int pinpoint_force_end_trace(NodeID id, int32_t timeout)
{
    try
    {
        _span_timeout = timeout;
        while (id != E_ROOT_NODE)
        {
            id = pinpoint_end_trace(id);
        }
        // back to normal
        _span_timeout = global_agent_info.timeout_ms;
        return 0;
    }
    catch (const std::out_of_range &)
    {
        pp_trace("#%d not found", id);
    }
    catch (const std::exception &ex)
    {
        pp_trace("#%d end trace failed. %s", id, ex.what());
    }
    return -1;
}

int pinpoint_trace_is_root(NodeID _id)
{
    try
    {
        TraceNode &node = PoolManager::getInstance().GetNode(_id);
        return node.isRoot() ? (1) : (0);
    }
    catch (const std::out_of_range &)
    {
        pp_trace("#%d not found", _id);
    }
    catch (const std::exception &ex)
    {
        pp_trace("#%d end trace failed. %s", _id, ex.what());
    }
    return -1;
}

NodeID pinpoint_end_trace(NodeID traceId)
{
    try
    {
        NodeID ret = do_end_trace(traceId);
        pp_trace("#%d pinpoint_end_trace Done!", traceId);
        return ret;
    }
    catch (const std::out_of_range &ex)
    {
        pp_trace("end_trace %d out_of_range exception: %s", traceId, ex.what());
    }
    catch (const std::exception &ex)
    {
        pp_trace("end_trace #%d end trace failed. %s", traceId, ex.what());
    }

    return E_INVALID_NODE;
}

// static int do_remove_trace(NodeID traceId)
// {
//     TraceNode&  node = PoolManager::getInstance().getNodeById(traceId);
//     node.remove();
//     return 0;
// }

// int pinpoint_remove_trace(NodeID traceId)
// {
//     try
//     {
//         NodeID ret = do_remove_trace(traceId);
//         pp_trace("#%d pinpoint_remove_trace Done!",traceId);
//         return ret;
//     }catch(const std::out_of_range&){
//         pp_trace("#%d not found",traceId);
//         return -1;
//     }catch(const std::exception &ex){
//         pp_trace("#%d end trace failed. %s",traceId,ex.what());
//         return -1;
//     }
// }

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

uint64_t inline do_mark_current_trace_status(NodeID &_id, E_AGENT_STATUS status)
{
    TraceNode &node = PoolManager::getInstance().GetNode(_id);
    assert(node.mRootId);
    TraceNode &root = PoolManager::getInstance().GetNode(node.mRootId);
    pp_trace("change current#%d status, before:%lld,now:%d", root.getId(), root.limit, status);
    return __sync_lock_test_and_set(&root.limit, status);
}

uint64_t mark_current_trace_status(NodeID _id, int status)
{
    try
    {
        return do_mark_current_trace_status(_id, (E_AGENT_STATUS)status);
    }
    catch (const std::out_of_range &ex)
    {
        pp_trace(" %s#%d failed with %s", __func__, _id, ex.what());
    }
    catch (const std::runtime_error &ex)
    {
        pp_trace(" %s#%d failed with %s", __func__, _id, ex.what());
    }
    catch (...)
    {
        pp_trace(" %s#%d failed with unkonw reason", __func__, _id);
    }
    return 0;
}

int check_tracelimit(int64_t timestamp)
{
    return SafeSharedState::instance().checkTraceLimit(timestamp) ? (1) : (0);
}

static inline TraceNode &parse_flag(NodeID _id, E_NODE_LOC flag)
{
    TraceNode &node = PoolManager::getInstance().GetNode(_id);
    if (flag == E_LOC_ROOT)
    {
        return PoolManager::getInstance().GetNode(node.mRootId);
    }
    else
    {
        return node;
    }
}

static void do_add_clue(NodeID _id, const char *key, const char *value, E_NODE_LOC flag)
{
    TraceNode &node = parse_flag(_id, flag);
    node.setNodeValue(key, value);
    pp_trace("#%d add clue key:%s value:%s", _id, key, value);
}

static void do_add_clues(NodeID _id, const char *key, const char *value, E_NODE_LOC flag)
{
    TraceNode &node = parse_flag(_id, flag);

    std::string cvalue = "";
    cvalue += key;
    cvalue += ':';
    cvalue += value;
    node.appendNodeValue(CLUSE, cvalue.c_str());
    pp_trace("#%d add clues:%s:%s", _id, key, value);
}

void pinpoint_add_clues(NodeID _id, const char *key, const char *value, E_NODE_LOC flag)
{
    try
    {
        do_add_clues(_id, key, value, flag);
    }
    catch (const std::out_of_range &ex)
    {
        pp_trace(" %s#%d failed.Reason %s,parameters:%s:%s", __func__, _id, ex.what(), key, value);
    }
    catch (const std::runtime_error &ex)
    {
        pp_trace(" %s#%d failed.Reason %s,parameters:%s:%s", __func__, _id, ex.what(), key, value);
    }
    catch (...)
    {
        pp_trace(" %s#%d failed.Reason: unknown reason,parameters:%s:%s", __func__, _id, key, value);
    }
}

void pinpoint_add_clue(NodeID _id, const char *key, const char *value, E_NODE_LOC flag)
{
    try
    {
        do_add_clue(_id, key, value, flag);
    }
    catch (const std::out_of_range &ex)
    {
        pp_trace(" %s#%d failed. Reason: %s,parameters:%s:%s", __func__, _id, ex.what(), key, value);
    }
    catch (const std::runtime_error &ex)
    {
        pp_trace(" %s#%d failed. Reason: %s,parameters:%s:%s", __func__, _id, ex.what(), key, value);
    }
    catch (...)
    {
        pp_trace(" %s#%d failed. Reason: unknow reason,parameters:%s:%s", __func__, _id, key, value);
    }
}

static inline void do_set_context_key(NodeID _id, const char *key, const char *value)
{
    TraceNode &node = PoolManager::getInstance().GetNode(_id);
    TraceNode &root = PoolManager::getInstance().GetNode(node.mRootId);

    root.setStrContext(key, value);
}

static const char *do_get_context_key(NodeID _id, const char *key)
{
    TraceNode &node = PoolManager::getInstance().GetNode(_id);
    TraceNode &root = PoolManager::getInstance().GetNode(node.mRootId);
    PContextType &pValue = root.getContextByKey(key);
    return pValue->asStringValue().c_str();
}

void pinpoint_set_context_key(NodeID _id, const char *key, const char *value)
{
    try
    {
        do_set_context_key(_id, key, value);
    }
    catch (const std::out_of_range &ex)
    {
        pp_trace(" %s#%d failed with out_of_range. %s,parameters:%s:%s", __func__, _id, ex.what(), key, value);
    }
    catch (const std::runtime_error &ex)
    {
        pp_trace(" %s#%d failed with runtime_error. %s,parameters:%s:%s", __func__, _id, ex.what(), key, value);
    }
    catch (const std::exception &ex)
    {
        pp_trace(" %s#%d failed with %s,parameters:%s:%s", __func__, _id, ex.what(), key, value);
    }
}

static void do_set_long_key(NodeID id, const char *key, long l)
{
    TraceNode &node = PoolManager::getInstance().GetNode(id);
    TraceNode &root = PoolManager::getInstance().GetNode(node.mRootId);
    root.setLongContext(key, l);
}

void pinpoint_set_context_long(NodeID _id, const char *key, long l)
{
    try
    {
        do_set_long_key(_id, key, l);
    }
    catch (const std::out_of_range &ex)
    {
        pp_trace(" %s#%d failed with %s", __func__, _id, ex.what());
    }
    catch (const std::runtime_error &ex)
    {
        pp_trace(" %s#%d failed with %s", __func__, _id, ex.what());
    }
    catch (const std::exception &ex)
    {
        pp_trace(" %s#%d failed with %s", __func__, _id, ex.what());
    }
}

static int do_get_long_key(NodeID _id, const char *key, long *l)
{
    TraceNode &node = PoolManager::getInstance().GetNode(_id);
    TraceNode &root = PoolManager::getInstance().GetNode(node.mRootId);
    PContextType &pValue = root.getContextByKey(key);
    *l = pValue->asLongValue();
    return 0;
}

int pinpoint_get_context_long(NodeID _id, const char *key, long *l)
{
    try
    {
        do_get_long_key(_id, key, l);
        return 0;
    }
    catch (const std::out_of_range &ex)
    {
        pp_trace(" %s#%d failed with %s", __func__, _id, ex.what());
    }
    catch (const std::runtime_error &ex)
    {
        pp_trace(" %s#%d failed with %s", __func__, _id, ex.what());
    }
    catch (const std::exception &ex)
    {
        pp_trace(" %s#%d failed with %s", __func__, _id, ex.what());
    }
    return 1;
}

const char *pinpoint_get_context_key(NodeID _id, const char *key)
{
    try
    {
        return do_get_context_key(_id, key);
    }
    catch (const std::out_of_range &ex)
    {
        pp_trace(" %s#%d failed with %s, parameters:%s", __func__, _id, ex.what(), key);
    }
    catch (const std::runtime_error &ex)
    {
        pp_trace(" %s#%d failed with %s, parameters:%s", __func__, _id, ex.what(), key);
    }
    catch (const std::exception &ex)
    {
        pp_trace(" %s#%d failed with %s, parameters:%s", __func__, _id, ex.what(), key);
    }
    return nullptr;
}

void do_catch_error(NodeID _id, const char *msg, const char *error_filename, uint32_t error_lineno)
{
    TraceNode &node = PoolManager::getInstance().GetNode(_id);
    TraceNode &root = PoolManager::getInstance().GetNode(node.mRootId);

    Json::Value eMsg;
    eMsg["msg"] = msg;
    eMsg["file"] = error_filename;
    eMsg["line"] = error_lineno;
    root.setNodeValue("ERR", eMsg);
}

void catch_error(NodeID _id, const char *msg, const char *error_filename, uint32_t error_lineno)
{
    try
    {
        do_catch_error(_id, msg, error_filename, error_lineno);
    }
    catch (const std::out_of_range &ex)
    {
        pp_trace(" %s#%d failed with %s", __func__, _id, ex.what());
    }
    catch (const std::runtime_error &ex)
    {
        pp_trace(" %s#%d failed with %s", __func__, _id, ex.what());
    }
    catch (const std::exception &ex)
    {
        pp_trace(" %s#%d failed with %s", __func__, _id, ex.what());
    }
}

const char *pinpoint_agent_version()
{
#ifdef AGENT_VERSION
    static const char *_version_ = AGENT_VERSION;
#else
    static const char *_version_ = "x.x.x";
#endif
    return _version_;
}

NodeID pinpoint_start_traceV1(NodeID parentId, const char *opt, ...)
{
    try
    {
        va_list args;
        va_start(args, opt);
        NodeID childId = do_start_trace(parentId, opt, &args);
        va_end(args);
        pp_trace("#%d pinpoint_start_traceV1 child #%d", parentId, childId);
        return childId;
    }
    catch (const std::out_of_range &ex)
    {
        pp_trace(" start_trace#%d failed with %s", parentId, ex.what());
    }
    catch (const std::runtime_error &ex)
    {
        pp_trace(" start_trace#%d failed with %s", parentId, ex.what());
    }
    catch (...)
    {
        pp_trace(" start_trace#%d failed with unkonw reason", parentId);
    }
    return E_INVALID_NODE;
}

static void do_add_exp(NodeID _id, const char *value)
{
    TraceNode &node = parse_flag(_id, E_LOC_CURRENT);
    node.setNodeValue("EXP", value);
    node.mHasExp = true;
    pp_trace("#%d add exp value:%s", _id, value);
}

void pinpoint_add_exception(NodeID traceId, const char *exp)
{
    try
    {
        do_add_exp(traceId, exp);
    }
    catch (const std::out_of_range &ex)
    {
        pp_trace(" %s#%d failed. Reason: %s,parameters:%s", __func__, traceId, ex.what(), exp);
    }
    catch (const std::runtime_error &ex)
    {
        pp_trace(" %s#%d failed. Reason: %s,parameters:%s", __func__, traceId, ex.what(), exp);
    }
    catch (const std::exception &ex)
    {
        pp_trace(" %s#%d failed. Reason: %s,parameters:%s", __func__, traceId, ex.what(), exp);
    }
}

void register_span_handler(void (*handler)(const char *))
{
    if (likely(handler != nullptr))
    {
        _SpanHandler_ = std::bind(handler, std::placeholders::_1);
    }
}

void show_status(void)
{
    Json::Value status;
    status["pool_total_node"] = PoolManager::getInstance().totoalNodesCount();
    status["pool_free_node"] = PoolManager::getInstance().freeNodesCount();

    auto add_alive_node_fun = [&status](int _id)
    {
        status["pool_alive_nodes"].append(_id);
    };
    PoolManager::getInstance().foreachAliveNode(std::bind(add_alive_node_fun, std::placeholders::_1));

    fprintf(stderr, "%s\n", status.toStyledString().c_str());
}