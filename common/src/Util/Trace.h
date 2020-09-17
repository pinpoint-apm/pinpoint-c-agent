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

#ifndef COMMON_SRC_UTIL_TRACE_H_
#define COMMON_SRC_UTIL_TRACE_H_

#include <stdint.h>
#include <chrono>
#include <ratio>
#include <ctime>
#include "common.h"
using std::chrono::steady_clock;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
namespace Trace{

typedef struct scope_time_trace
{
    steady_clock::time_point _start_time;
    std::string func;
    scope_time_trace(const char* cfunc)
    {
        _start_time =  steady_clock::now();
        func = cfunc;
    }
    ~scope_time_trace()
    {
        milliseconds elapsed = duration_cast<milliseconds>(steady_clock::now() - _start_time);
        pp_trace(" [%s] it's take [%ld] miliseconds",func.c_str(),elapsed.count());
    }
}STT;

}

#ifndef NDEBUG

#define ADDTRACE()  Trace::STT __stt__(__func__)
#else

#define ADDTRACE()

#endif


#endif /* COMMON_SRC_UTIL_TRACE_H_ */
