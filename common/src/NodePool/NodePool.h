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
 * NodePool.h
 *
 *  Created on: Aug 19, 2020
 *      Author: test
 */

#ifndef COMMON_SRC_NODEPOOL_NODEPOOL_H_
#define COMMON_SRC_NODEPOOL_NODEPOOL_H_

#include "common.h"
#include "TraceNode.h"
#include <iostream>
#include <map>
namespace NodePool{

typedef  std::map<uint32_t,TraceNode>::iterator NodeIter ;
/*
 * @author eeliu
 * Aug 19, 2020 5:59:52 PM
 */
class NodePool
{

public:
    TraceNode* fetchNodeById(uint32_t id)
    {
        NodeIter iter =    this->_aliveNode.find(id);

        if(iter ==  this->_aliveNode.end())
        {
            return nullptr;
        }

        return &iter->second;
    }

private:
    std::map<uint32_t,TraceNode> _aliveNode;

};

}
#endif /* COMMON_SRC_NODEPOOL_NODEPOOL_H_ */
