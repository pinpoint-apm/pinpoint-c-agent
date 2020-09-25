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

namespace NodePool{

//class NodeID{
//public:
//    NodeID() = default;
//
//    NodeID(uint32_t _id)
//    {
//        this->_id = _id;
//    }
//
//    NodeID(const NodeID &_ID)
//    {
//        this->_id = _ID._id;
//    }
//
//
//    NodeID& operator=(const NodeID& _ID)
//    {
//        this->_id = _ID._id;
//        return *this;
//    }
//
//    NodeID& operator=(uint32_t _id)
//    {
//        this->_id = _id;
//        return *this;
//    }
//
//    std::string toString() const
//    {
//        return std::to_string(this->_id);
//    }
//
//    bool operator<(NodeID const& _ID) const
//    {
//        return this->_id < _ID._id;
//    }
//
//    bool operator==(NodeID const& _ID) const
//    {
//        return this->_id == _ID._id;
//    }
//
//      bool operator!=(NodeID const& _ID) const
//    {
//        return this->_id != _ID._id;
//    }
//
//private:
//
//    friend std::ostream& operator<<(std::ostream& iostream, const NodeID& nodeId){
//        return iostream<<"NodeID:"<<nodeId._id;
//    }
//
//
//
//private:
//    uint32_t _id;
//};
//

using Context::ContextType;
using Context::StringContextType;

typedef std::shared_ptr<ContextType> PContextType;
class TraceNode
{

public: 
    // c-style tree node
    TraceNode* p_brother_node; // equal next node
    TraceNode* p_child_head;   // subtree
    TraceNode* p_parent_node;  // parent
    TraceNode* p_root_node;    // highway to root node

    uint64_t start_time;
    uint64_t fetal_error_time;
    uint64_t limit;

public:
    void addChild(TraceNode& child);

    inline bool isRoot() const
    {
        return (this->p_root_node  == this);
    }

    inline bool isLeaf() const
    {
        return !(this->p_child_head);
    }

public:
    TraceNode(NodeID id = 0);

    virtual ~TraceNode();

    Json::Value& getValue()
    {
        return this->_value;
    }

    TraceNode& reset(NodeID id)
    {
        this->clearAttach();
        this->initId(id);
        this->resetStatus();
        this->resetRelative();
        return *this;
    }

    NodeID getId() const
    {
        return this->id;
    }

    PContextType& getContextByKey(const char* key)
    {
        return this->_context.at(key);
    }

    void setStrContext(const char* key,const char* buf)
    {
        PContextType context(new StringContextType(buf));
        this->_context[key] = context;
        // std::string& value =  this->_context[key]->asStringValue();
        // pp_trace("value %s",value.c_str());
    }

public:

    bool operator==(TraceNode const& _node) const
    {
        return this->id == _node.id;
    }

    bool operator!=(TraceNode const& _node) const
    {
        return this->id != _node.id;
    }

    Json::Value& operator[](const std::string key)
    {
        return this->_value[key];
    }

private:

    void clearAttach();

    void initId(NodeID& id);

    inline void resetRelative(){
        this->p_brother_node = nullptr;
        this->p_child_head = nullptr;
        this->p_parent_node = this;
        this->p_root_node = this;
    }

    inline void resetStatus()
    {
        this->fetal_error_time = 0;
        this->start_time = 0;
        this->limit  = E_TRACE_PASS;
    }

private:
    //note: logic_id  = physical_id + 1
    NodeID id;
    Json::Value _value;

    std::map<std::string,PContextType>  _context;
};
}
#endif /* COMMON_SRC_TRACENODE_H_ */
