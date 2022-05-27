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
        std::lock_guard<std::mutex> _safe(this->mlock);
        // empty the json value
        if (!this->_value.empty())
            this->_value.clear(); // Json::Value();

        if (!this->_context.empty())
            this->_context.clear();

        if (!this->_callback.empty())
            this->_callback.clear();
    }

    void TraceNode::initId(NodeID &id)
    {
        this->ID = id;
    }

    void TraceNode::addChild(TraceNode &child)
    {
        std::lock_guard<std::mutex> _safe(this->mlock);
        if (child.hasParent() == false)
        {
            if (this->mChildListHeaderId != E_INVALID_NODE)
                child.mNextId = this->mChildListHeaderId;
            assert(child.mNextId != child.ID);
            this->mChildListHeaderId = child.ID;
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
        this->setNodeValue("S", this->start_time - this->root_start_time);
    }

    void TraceNode::setTraceParent(TraceNode &parent)
    {
        this->startParentId = parent.ID;
        this->mRootId = parent.mRootId;
        TraceNode &root = PoolManager::getInstance().GetNode(this->mRootId);
        this->root_start_time = root.start_time;
    }

    void TraceNode::startTimer()
    {
        uint64_t time_in_ms = Helper::get_current_msec_stamp();
        this->start_time = time_in_ms;
    }

    void TraceNode::parseOpt(std::string key, std::string value)
    {
        pp_trace("#%d add opt: key:%s value:%s", ID, key.c_str(), value.c_str());
        if (key == "TraceMinTimeMs")
        {
            int64_t min = std::stoll(value);
            auto cb = [=]() -> bool
            {
                if ((int64_t)this->cumulative_time >= min)
                    return true;
                return false;
            };
            this->_callback.push_back(cb);
        }
        else if (key == "TraceOnlyException")
        {
            auto cb = [=]() -> bool
            {
                return this->mHasExp;
            };

            this->_callback.push_back(cb);
        }
    }

    bool TraceNode::checkOpt()
    {
        bool ret = true;
        for (auto &cb : this->_callback)
        {
            if ((ret = cb()) == true)
                return ret;
        }
        return ret;
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
                std::string key(var, delimit - var);
                std::string value(delimit + 1);
                this->parseOpt(key, value);
            }
            var = va_arg(*args, const char *);
        }
    }
}
