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
 * TraceNode.h
 *
 *  Created on: Aug 19, 2020
 *      Author: eeliu
 */

#ifndef COMMON_SRC_TRACENODE_H_
#define COMMON_SRC_TRACENODE_H_
#include "common.h"
#include "json/json.h"
#include "Context/ContextType.h"
#include <atomic>
#include <map>
#include <iostream>
#include <mutex>
#include <stdarg.h>

namespace NodePool
{

    namespace Json = AliasJson;
    using Context::ContextType;
    using Context::LongContextType;
    using Context::StringContextType;

    typedef std::shared_ptr<ContextType> PContextType;
    class TraceNode
    {

    public:
        // c-style tree node
        NodeID mNextId;            // equal brother node
        NodeID mChildId;           // subtree/child tree
        NodeID mParentId;          // parent Id [end_trace] avoiding re-add
        NodeID startTraceParentId; // parent Id [start_trace]
        NodeID mRootId;            // highway to root node
        NodeID ID;

        uint64_t start_time;
        uint64_t fetal_error_time;
        uint64_t limit;
        uint64_t cumulative_time;
        bool mHasExp;

    public:
        void startTimer();
        void setTraceParent(TraceNode &parent);
        void endTimer();
        void wake();

        void convertToSpanEvent();
        void convertToSpan();

    public:
        void addChild(TraceNode &child);

        void remove();

        inline bool isRoot() const
        {
            return this->mRootId == ID;
        }

        inline bool isLeaf() const
        {
            return this->mChildId == E_INVALID_NODE;
        }

    public:
        TraceNode()
        {
            this->ID = E_INVALID_NODE;
            this->mRootId = E_INVALID_NODE;
            this->resetRelative();
            this->resetStatus();
        }

        virtual ~TraceNode();

        Json::Value &getValue()
        {
            return this->_value;
        }

        TraceNode &reset(NodeID id)
        {
            this->clearAttach();
            this->initId(id);
            this->resetStatus();
            this->resetRelative();
            return *this;
        }

        NodeID getId() const
        {
            return this->ID;
        }

        PContextType &getContextByKey(const char *key)
        {
            std::lock_guard<std::mutex> _safe(this->_lock);
            return this->_context.at(key);
        }

        void setStrContext(const char *key, const char *buf)
        {
            std::lock_guard<std::mutex> _safe(this->_lock);
            PContextType context(std::make_shared<StringContextType>(buf));
            this->_context[key] = context;
        }

        void setLongContext(const char *key, long l)
        {
            PContextType context(std::make_shared<LongContextType>(l));
            this->_context[key] = context;
        }

    public:
        bool operator==(TraceNode const &_node) const
        {
            return this->ID == _node.ID;
        }

        bool operator!=(TraceNode const &_node) const
        {
            return this->ID != _node.ID;
        }

    public:
        void setNodeValue(const char *key, const char *v)
        {
            std::lock_guard<std::mutex> _safe(this->_lock);
            this->_value[key] = v;
        }

        void setNodeValue(const char *key, int v)
        {
            std::lock_guard<std::mutex> _safe(this->_lock);
            this->_value[key] = v;
        }

        void setNodeValue(const char *key, uint64_t v)
        {
            std::lock_guard<std::mutex> _safe(this->_lock);
            this->_value[key] = v;
        }

        void setNodeValue(const char *key, Json::Value &v)
        {
            std::lock_guard<std::mutex> _safe(this->_lock);
            this->_value[key] = v;
        }

        void appendNodeValue(const char *key, const char *v)
        {
            std::lock_guard<std::mutex> _safe(this->_lock);
            this->_value[key].append(v);
        }

    public:
        void setOpt(const char *opt, va_list *args);

    private:
        void parseOpt(std::string key, std::string value);

        void clearAttach();

        void initId(NodeID &id);

        inline void resetRelative()
        {
            this->mNextId = E_INVALID_NODE;
            this->mChildId = E_INVALID_NODE;
            this->mParentId = ID;
            this->mRootId = ID;
        }

        inline void resetStatus()
        {
            this->fetal_error_time = 0;
            this->start_time = 0;
            this->limit = E_TRACE_PASS;
            this->cumulative_time = 0;
            this->mHasExp = false;
        }

    public:
        // changes: expose _lock
        std::mutex _lock;

    private:
        Json::Value _value;
        std::map<std::string, PContextType> _context;
    };
}
#endif /* COMMON_SRC_TRACENODE_H_ */
