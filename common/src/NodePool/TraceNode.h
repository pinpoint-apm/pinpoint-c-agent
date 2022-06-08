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
#include <functional>
#include <mutex>
#include <stdarg.h>

namespace NodePool
{

    namespace Json = AliasJson;
    using Context::ContextType;
    using Context::LongContextType;
    using Context::StringContextType;
    class TraceNode;
    class WrapperTraceNode
    {
    public:
        WrapperTraceNode() = delete;
        WrapperTraceNode(const WrapperTraceNode &other) = delete;
        WrapperTraceNode(WrapperTraceNode &&other) : _traceNode(std::move(other._traceNode))
        {
            other._traceNode = nullptr;
        }

        WrapperTraceNode &operator=(const WrapperTraceNode &other) = delete;

        WrapperTraceNode(TraceNode *node);
        TraceNode *operator->()
        {
            return _traceNode;
        }
        ~WrapperTraceNode();

    private:
        TraceNode *_traceNode;
    };

    typedef std::shared_ptr<ContextType> _ContextType_;
    class TraceNode
    {
    public:
        NodeID mNextId;         // equal brother node
        NodeID mChildHeadIndex; // subtree/child tree

        NodeID mParentIndex; // parent Id [end_trace] avoiding re-add
        NodeID mRootIndex;   // highway to root node
        NodeID mPoolIndex;
        // int64_t mUID;
        // int64_t mParentUID;
        // int64_t mRootUID;

        uint64_t start_time;
        uint64_t fetal_error_time;
        uint64_t limit;
        uint64_t cumulative_time;
        uint64_t root_start_time;
        bool mHasExp;

    public:
        void startTimer();
        void endTimer();
        void wakeUp();

        void convertToSpanEvent();
        void convertToSpan();

    public:
        void addChild(WrapperTraceNode &child);
        void remove();

        inline bool isRoot() const
        {
            return this->mRootIndex == mPoolIndex;
        }

        inline bool isLeaf() const
        {
            return this->mChildHeadIndex == E_INVALID_NODE;
        }

        // private:
        //     inline bool isRelated(NodeID mParentIndex)
        //     {
        //         assert(mParentIndex != mPoolIndex);
        //         assert(this->mStartParentId != E_INVALID_NODE);
        //         assert(mParentIndex != E_INVALID_NODE);
        //         if (this->mParentIndex == this->mStartParentId)
        //         {
        //             assert(this->mParentIndex == mParentIndex);
        //             return true;
        //         }
        //         else
        //         {
        //             assert(this->mParentIndex == mPoolIndex);
        //             return false;
        //         }
        //     }

    public:
        TraceNode()
        {
            this->mPoolIndex = E_INVALID_NODE;
            this->mRootIndex = E_INVALID_NODE;
            this->resetRelative();
            this->resetStatus();
            this->_mRef = 0;
        }

        virtual ~TraceNode();

        TraceNode &reset(NodeID id)
        {
            std::lock_guard<std::mutex> _safe(this->mlock);
            this->clearAttach();
            this->initId(id);
            this->resetStatus();
            this->resetRelative();
            this->_mRef = 0;
            return *this;
        }

        NodeID getId() const
        {
            return this->mPoolIndex;
        }

        Json::Value getJsValue()
        {
            std::lock_guard<std::mutex> _safe(this->mlock);
            return this->_value;
        }

        void getContext(const char *key, std::string &value)
        {
            std::lock_guard<std::mutex> _safe(this->mlock);
            _ContextType_ &ctx = this->_context.at(key);
            value = ctx->asStringValue();
        }

        void getContext(const char *key, long &value)
        {
            std::lock_guard<std::mutex> _safe(this->mlock);
            _ContextType_ &ctx = this->_context.at(key);
            value = ctx->asLongValue();
        }

        void setContext(const char *key, const char *buf)
        {
            std::lock_guard<std::mutex> _safe(this->mlock);
            _ContextType_ context(std::make_shared<StringContextType>(buf));
            this->_context[key] = context;
        }

        void setContext(const char *key, long l)
        {
            std::lock_guard<std::mutex> _safe(this->mlock);
            _ContextType_ context(std::make_shared<LongContextType>(l));
            this->_context[key] = context;
        }

    public:
        TraceNode &operator=(const TraceNode &) = delete;
        TraceNode(const TraceNode &) = delete;

        bool operator==(TraceNode const &_node) const
        {
            return this->mPoolIndex == _node.mPoolIndex;
        }

        bool operator!=(TraceNode const &_node) const
        {
            return this->mPoolIndex != _node.mPoolIndex;
        }

    public:
        void AddTraceDetail(const char *key, const char *v)
        {
            std::lock_guard<std::mutex> _safe(this->mlock);
            this->_value[key] = v;
        }

        void AddTraceDetail(const char *key, int v)
        {
            std::lock_guard<std::mutex> _safe(this->mlock);
            this->_value[key] = v;
        }

        void AddTraceDetail(const char *key, uint64_t v)
        {
            std::lock_guard<std::mutex> _safe(this->mlock);
            this->_value[key] = v;
        }

        void AddTraceDetail(const char *key, Json::Value &v)
        {
            std::lock_guard<std::mutex> _safe(this->mlock);
            this->_value[key] = v;
        }

        void appendNodeValue(const char *key, const char *v)
        {
            std::lock_guard<std::mutex> _safe(this->mlock);
            this->_value[key].append(v);
        }

    public:
        void setOpt(const char *opt, va_list *args);
        bool checkOpt();

    private:
        void parseOpt(std::string key, std::string value);

        void clearAttach();

        void initId(NodeID &id);

        inline void resetRelative()
        {
            this->mNextId = E_INVALID_NODE;
            this->mChildHeadIndex = E_INVALID_NODE;
            this->mParentIndex = mPoolIndex;
            this->mRootIndex = mPoolIndex;
        }

        inline void resetStatus()
        {
            this->fetal_error_time = 0;
            this->root_start_time = 0;
            this->start_time = 0;
            this->limit = E_TRACE_PASS;
            this->cumulative_time = 0;
            this->mHasExp = false;
        }

    public:
        // changes: expose _lock
        std::mutex mlock;

    public:
        int addRef()
        {
            _mRef++;
            return _mRef.load();
        }

        int rmRef()
        {
            _mRef--;
            return _mRef.load();
        }

        bool checkZoreRef()
        {
            return _mRef.load() == 0;
        }

    public:
        std::string ToString()
        {
            std::lock_guard<std::mutex> _safe(this->mlock);
            char pbuf[1024] = {0};
            int len = snprintf(pbuf, 1024, "mNextId:%d mChildListHeaderId:%d mParentIndex:%d mRootIndex:%d mPoolIndex:%d \n"
                                           "start_time:%lu,fetal_error_time:%lu,limit:%lu,cumulative_time:%lu,root_start_time:%lu,mHasExp:%d \n"
                                           "_mRef:%d\n"
                                           "_value:%s \n"
                                           "_context size:%lu,_callback:%lu \n",
                               (int)this->mNextId, (int)this->mChildHeadIndex, (int)this->mParentIndex, (int)this->mRootIndex, (int)this->mPoolIndex,
                               this->start_time, this->fetal_error_time, this->limit, this->cumulative_time, this->root_start_time, this->mHasExp,
                               this->_mRef.load(),
                               this->_value.toStyledString().c_str(),
                               this->_context.size(), this->_callback.size());
            return std::string(pbuf, len);
        }

    private:
        std::atomic<int> _mRef;

    private:
        Json::Value _value;
        std::map<std::string, _ContextType_> _context;
        std::vector<std::function<bool()>> _callback;
    };
}
#endif /* COMMON_SRC_TRACENODE_H_ */
