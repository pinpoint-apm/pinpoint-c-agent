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
 * Trace.h
 *
 *  Created on: Sep 14, 2020
 *      Author: eeliu
 */

#ifndef COMMON_SRC_UTIL_HELPER_H_
#define COMMON_SRC_UTIL_HELPER_H_

#include <stdint.h>
#include <chrono>
#include <ratio>
#include <ctime>
#include <iostream>
#include <string.h>

#include "json/json.h"
#include "json/writer.h"


#include "common.h"
#include "NodePool/TraceNode.h"
#include "ConnectionPool/SpanConnectionPool.h"

namespace Helper{

using std::chrono::steady_clock;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;
using std::chrono::time_point_cast;
using std::chrono::time_point;
using NodePool::TraceNode;
using ConnectionPool::TransConnection;
typedef struct scope_time_trace
{
    steady_clock::time_point _start_time;
    std::string _funcName;
    scope_time_trace(const char* cfunc)
    {
        _start_time =  steady_clock::now();
        _funcName = cfunc;
    }
    ~scope_time_trace()
    {
        milliseconds elapsed = duration_cast<milliseconds>(steady_clock::now() - _start_time);
        pp_trace(" [%s] it's take [%ld] miliseconds",_funcName.c_str(),elapsed.count());
    }
}STT;


uint64_t get_current_msec_stamp();

std::string node_tree_to_string(Json::Value &value);

Json::Value merge_node_tree(TraceNode& root);

TransConnection getConnection();

void freeConnection(TransConnection& );

Json::Value merge_children(TraceNode& node);

}

#ifndef NDEBUG

#define ADDTRACE()  Helper::STT __stt__(__func__)
#else

#define ADDTRACE()

#endif


#endif /* COMMON_SRC_UTIL_HELPER_H_ */
