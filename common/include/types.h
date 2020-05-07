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
 * types.h
 *
 *  Created on: Mar 12, 2020
 *      Author: test
 */

#ifndef COMMON_SRC_TYPES_H_
#define COMMON_SRC_TYPES_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

//fix #129
#ifndef uint
#define uint unsigned int
#endif

#define MAX_VEC 512
#define LOG_SIZE 4096
#define IN_MSG_BUF_SIZE 4096
#define NAMING_SIZE 128
#define PHP 1500

typedef enum{
    RESPONSE_AGENT_INFO = 0,
    REQ_UPDATE_SPAN = 1
}MSG_TYPE;

#pragma pack (1)
typedef  struct {
    uint type;
    uint length;
}Header;
#pragma pack ()

typedef struct collector_agent_s{
    uint64_t start_time;
    char*   appid;
    char*   appname;
}CollectorAgentInfo;

#define LOG_SIZE 4096

typedef void (*VOID_FUNC)(void);
typedef struct pp_agent_s{
    const char* co_host; // tcp:ip:port should support dns
    uint  timeout_ms;  // always be 0
    long   trace_limit;  // change to long as python need long
    int   agent_type;
    uint8_t debug_report;
    VOID_FUNC get_read_lock;
    VOID_FUNC get_write_lock;
    VOID_FUNC release_lock;
}PPAgentT;
typedef void(*log_error_cb)(char*);

extern PPAgentT global_agent_info;

#endif /* COMMON_SRC_TYPES_H_ */
