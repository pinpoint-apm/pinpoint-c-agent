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
 *      Author: test
 */

#include "TraceNode.h"

namespace NodePool{


TraceNode::TraceNode(NodeID id)
{
    this->id = id;
    this->resetRelative();
}

TraceNode::~TraceNode()
{

}

void TraceNode::clear()
{
    // empty the json value 
    if(!this->_value.empty())  this->_value.clear();

    this->resetRelative();
}

void TraceNode::init(NodeID& id)
{
    this->id = id;
}

}
