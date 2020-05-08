/*******************************************************************************
 * Copyright 2019 NAVER Corp
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations under
 * the License.
 ******************************************************************************/
#ifndef COMMON_H_
#define COMMON_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __linux__
#define likely(x)        __builtin_expect(!!(x), 1)
#define unlikely(x)      __builtin_expect(!!(x), 0)
#elif _WIN32

#else

#endif

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


typedef enum{
    E_LOGGING=0x1,
    E_DISABLE_GIL=0x2 // disable gil checking in python
}AGENT_FLAG;


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

///
typedef struct trace_store_layer{
    void(*set_cur_trace_cb)(void*);
    void*(*get_cur_trace_cb)(void);
}TraceStoreLayer;

#define LOG_SIZE 4096
typedef void (*VOID_FUNC)(void);
typedef struct pp_agent_s{
    const char* co_host; // tcp:ip:port should support dns
    uint  timeout_ms;  // always be 0
    long   trace_limit;  // change to long as python need long
    int   agent_type;
    uint8_t inter_flag;
    VOID_FUNC get_read_lock;
    VOID_FUNC get_write_lock;
    VOID_FUNC release_lock;
}PPAgentT;

enum E_ANGET_STATUS{
    E_OFFLINE = 0x1,
    E_TRACE_PASS =0x2,
    E_TRACE_BLOCK =0x4
};

/**
 *pinpoint_start_trace
 *pinpoint_end_trace
 *pinpoint_add_clue
 *pinpoint_add_clues
 *pinpoint_unique_id
 *pinpoint_tracelimit $timestamp
 *pinpoint_drop_trace
 *pinpoint_app_name
 *pinpoint_app_id
 *pinpoint_start_time
 */
#ifdef __cplusplus 
extern "C"{
#endif

extern PPAgentT global_agent_info;

int32_t pinpoint_start_trace(void);
int32_t pinpoint_end_trace(void);
void pinpoint_add_clues(const  char* key,const  char* value);
void pinpoint_add_clue(const  char* key,const  char* value);
void pinpoint_set_special_key(const char* key,const char* value);
const char* pinpoint_get_special_key(const char* key);
bool check_tracelimit(int64_t timestamp);
void pinpoint_force_flush_span(uint32_t timeout);
int64_t generate_unique_id(void);
void pinpoint_drop_trace(void);
const char* pinpoint_app_id(void);
const char* pinpoint_app_name(void);
uint64_t pinpoint_start_time(void);
void enable_trace_utest(void);
void catch_error(const char* msg,const char* error_filename,uint error_lineno);
typedef void(*log_error_cb)(char*);
void register_error_cb(log_error_cb error_cb);
void pp_trace(const char *format,...);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Inner API
void pinpoint_reset_store_layer(TraceStoreLayer* storeLayer);
void* create_or_reuse_agent(void);
void give_back_agent(void *agent);

#ifdef __cplusplus 
}
#endif
#endif /* COMMON_H_ */
