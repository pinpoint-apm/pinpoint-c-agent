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
#include "NodePool/PoolManager.h"
#include "Helper.h"

namespace Helper
{
    namespace Json = AliasJson;
    using Cache::NodeTreeWriter;
    using NodePool::PoolManager;
    static NodeTreeWriter _writer;
    static ConnectionPool::SpanConnectionPool _con_pool;
    static std::once_flag _pool_init_flag;
    static Json::Value mergeChildren(TraceNode &node);
    uint64_t get_current_msec_stamp() noexcept
    {
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        time_point<system_clock, milliseconds> current = time_point_cast<std::chrono::milliseconds>(now);
        return current.time_since_epoch().count();
    }

    std::string node_tree_to_string(const Json::Value &value)
    {
        return _writer.write(value);
    }

    static void gatcherChildDetailByReverse(Json::Value &detail, TraceNode &head)
    {
        if (head.mNextId != E_INVALID_NODE)
        {
            TraceNode &next = PoolManager::getInstance().Take(head.mNextId);
            gatcherChildDetailByReverse(detail, next);
        }
        Json::Value childrenDetail = mergeChildren(head);
        if (!childrenDetail.empty())
        {
            detail.append(childrenDetail);
        }
    }

    Json::Value mergeChildren(TraceNode &node)
    {
        if (node.checkOpt() == false)
        {
            return Json::Value();
        }
        else if (!node.isLeaf())
        {
            TraceNode &child = PoolManager::getInstance().Take(node.mChildHeadIndex);
            Json::Value childTraceDetail;
            gatcherChildDetailByReverse(childTraceDetail, child);
            node.AddTraceDetail("calls", childTraceDetail);
        }
        return node.getJsValue();
    }

    Json::Value mergeTraceNodeTree(NodeID &Id) noexcept
    {
        return mergeTraceNodeTree(PoolManager::getInstance().Take(Id));
    }

    Json::Value mergeTraceNodeTree(TraceNode &root)
    {
        return mergeChildren(root);
    }

    TransConnection getConnection()
    {

        std::call_once(_pool_init_flag,
                       []()
                       { _con_pool.initPool(global_agent_info.co_host); });

        return _con_pool.get();
    }

    void freeConnection(TransConnection &trans)
    {

        std::call_once(_pool_init_flag,
                       []()
                       { _con_pool.initPool(global_agent_info.co_host); });

        return _con_pool.free(trans);
    }

}
