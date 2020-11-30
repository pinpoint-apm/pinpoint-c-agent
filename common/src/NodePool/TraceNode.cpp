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

namespace NodePool{


TraceNode::TraceNode(NodeID id)
{
    this->id = id;
    this->p_root_node = this;
    this->resetRelative();
    this->resetStatus();
}

TraceNode::~TraceNode()
{

}

void TraceNode::clearAttach()
{
    // empty the json value 
    if(!this->_value.empty())  this->_value.clear();

    if(!this->_context.empty()) this->_context.clear();
    
    this->resetRelative();
}

void TraceNode::initId(NodeID& id)
{
    this->id = id;
}

void TraceNode::addChild(TraceNode& child)
{
    if(this->p_child_head)  child.p_brother_node = this->p_child_head;

    this->p_child_head = &child;
    child.p_parent_node = this;
    // pass the root to every child
    child.p_root_node = this->p_root_node;
}

}
