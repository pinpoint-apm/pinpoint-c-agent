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
 *      Author: test
 */

#ifndef COMMON_SRC_TRACENODE_H_
#define COMMON_SRC_TRACENODE_H_
#include "common.h"
#include "json/json.h"
#include <atomic>
#include <iostream>

namespace NodePool{

 typedef uint32_t NodeID;

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


/*
 * @author eeliu
 * Aug 19, 2020 4:33:36 PM
 */
class TraceNode
{

/**
+---------------------------------+
|               +------+          |
|            +  | Node | <---+    |
|      sub   |  +------+     |    |
|            | ^             |    |
|            v |             |    |
|        +---+-++        +---+--+ |
|        | Node | +--->  | Node | |
|        +--^--^+        +------+ |
|           |  |   next           |
|         + |  |                  |
|   sub   | |  |                  |
|         v |  |                  |
|           |  |                  |
|    +------+  +-----+------+     |
|    | Node | +-->   | Node |     |
|    +------+        +------+     |
|             next                |
|                                 |
+---------------------------------+
**/

// c-style tree node

public: 
    TraceNode* next;
    TraceNode* children;
    TraceNode* parent;
    
public:
    inline void addChild(TraceNode& node)
    {
        if(this->children)  node.next = this->children;

        this->children = &node;
        node.parent = this;
    }

    // inline bool isRoot() const
    // {
    //     return !(this->parent || this->next);
    // }

    inline bool isLeaf() const
    {
        return !(this->children);
    }

public:
    TraceNode(NodeID id);

    virtual ~TraceNode();

    Json::Value& getValue()
    {
        return this->_value;
    }

    void reset(NodeID id)
    {
        this->clear();
        this->init(id);
    }

    NodeID getId() const
    {
        return this->id;
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

    void clear();

    void init(NodeID& id);

    inline void resetRelative(){
        this->next = nullptr;
        this->children = nullptr;
        this->parent = nullptr;
    }

private:
    NodeID id;
    Json::Value _value;
};
}
#endif /* COMMON_SRC_TRACENODE_H_ */
