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

#define UINT32_MAX (0xfffffff)
std::atomic<uint32_t> TraceNode::_Global_ID = 0;
static std::atomic<uint32_t> _Start_ID=0;
static std::atomic<uint32_t> _MAX_ID=UINT32_MAX;

uint32_t TraceNode::getID()
{
    TraceNode::_Global_ID.compare_exchange_week(_MAX_ID,_Start_ID);
    TraceNode::_Global_ID++;
    return  TraceNode::_Global_ID;
}

TraceNode::TraceNode()
{
    this->id = this->getID();
}

TraceNode::~TraceNode()
{

}

}
