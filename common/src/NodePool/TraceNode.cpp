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
 * TraceNode.cpp
 *
 *  Created on: Aug 19, 2020
 *      Author: eeliu
 */

#include "TraceNode.h"
#include "Util/Helper.h"
#include "PoolManager.h"

namespace NodePool
{
    TraceNode::~TraceNode()
    {
    }

    void TraceNode::clearAttach()
    {
        // empty the json value
        if (!this->_value.empty())
            this->_value.clear(); // Json::Value();

        if (!this->_context.empty())
            this->_context.clear();
    }

    void TraceNode::initId(NodeID &id)
    {
        this->ID = id;
    }

    void TraceNode::addChild(TraceNode &child)
    {
        std::lock_guard<std::mutex> _safe(this->_lock);
        if (child.mParentId != ID)
        {
            if (this->mChildId != E_INVALID_NODE)
                child.mNextId = this->mChildId;

            this->mChildId = child.ID;
            child.mParentId = ID;
        }
    }

    void TraceNode::endTimer()
    {

        uint64_t end_time = Helper::get_current_msec_stamp();

        this->cumulative_time += (end_time - this->start_time);
    }

    void TraceNode::wake()
    {
        this->start_time = Helper::get_current_msec_stamp();
    }

    void TraceNode::convertToSpan()
    {
        this->setNodeValue("E", this->cumulative_time);
        this->setNodeValue("S", this->start_time);

        this->setNodeValue("FT", global_agent_info.agent_type);
    }

    void TraceNode::convertToSpanEvent()
    {
        this->setNodeValue("E", this->cumulative_time);
        TraceNode &root = PoolManager::getInstance().GetNode(this->mRootId);
        this->setNodeValue("S", this->start_time - root.start_time);
    }

    void TraceNode::setTraceParent(TraceNode &parent)
    {
        this->startTraceParentId = parent.ID;
        this->mRootId = parent.mRootId;
    }

    void TraceNode::startTimer()
    {
        uint64_t time_in_ms = Helper::get_current_msec_stamp();
        this->start_time = time_in_ms;
    }

    void TraceNode::parseOpt(std::string key, std::string value)
    {
    }

    void TraceNode::setOpt(const char *opt, va_list *args)
    {
        const char *var = opt;

        while (var != nullptr)
        {

            const char *delimit = strchr(var, ':');
            if (delimit == nullptr)
            {
                this->parseOpt(std::string(var), "");
            }
            else
            {
                std::string key(var, delimit - var - 1);
                std::string value(delimit + 1);
                this->parseOpt(key, value);
            }
            var = va_arg(*args, const char *);
        }
    }

}
