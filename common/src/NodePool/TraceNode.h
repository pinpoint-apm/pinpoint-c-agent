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

namespace NodePool{
/*
 * @author eeliu
 * Aug 19, 2020 4:33:36 PM
 */
class TraceNode
{
public:
    int id;

public:
    TraceNode();

    virtual ~TraceNode();

    const Json::Value& getSelfNode() const {
        return &this->_node;
    }

    void reset()
    {
        _node.clear();
        id = -1;
    }

private:

    uint32_t getID();

private:
    static std::atomic<uint32_t> _Global_ID;
    Json::Value _node;
};
}
#endif /* COMMON_SRC_TRACENODE_H_ */
