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
#include <assert.h>

#ifdef __linux__
#define likely(x)        __builtin_expect(!!(x), 1)
#define unlikely(x)      __builtin_expect(!!(x), 0)
#elif _WIN32

#else

#endif

#define MAX_VEC 512
#define LOG_SIZE 4096
#define IN_MSG_BUF_SIZE 4096
#define NAMING_SIZE 128
#define MAX_SPAN_SIZE 4096*100

typedef enum{
    RESPONSE_AGENT_INFO = 0,
    REQ_UPDATE_SPAN = 1
}MSG_TYPE;


typedef enum{
    E_LOGGING=0x1,
    E_DISABLE_GIL=0x2, // disable gil checking in python
    E_UTEST = 0x4
}AGENT_FLAG;

typedef int32_t NodeID;

#pragma pack (1)
typedef  struct {
    uint32_t type;
    uint32_t length;
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
    uint32_t    timeout_ms;  // always be 0
    long        trace_limit;  // change to long as python need long
    int         agent_type;
    uint8_t     inter_flag;
    VOID_FUNC   get_read_lock;
    VOID_FUNC   get_write_lock;
    VOID_FUNC   release_lock;
}PPAgentT;

typedef enum {
    E_OFFLINE = 0x1,
    E_TRACE_PASS =0x2,
    E_TRACE_BLOCK =0x4,
    E_READY = 0x8
}E_AGENT_STATUS;

/**
 * @brief at present only root checking
 */
typedef enum {
    E_CURRENT_LOC=0x0,
    E_ROOT_LOC=0x1
}E_NODE_LOC;
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

/**
 * @brief 
 * 
 * @return NodeID 
 */
NodeID pinpoint_get_per_thread_id(void);

void pinpoint_update_per_thread_id(NodeID id);

/**
 * start a trace (span). if current span is empty, create a span or else create a spanevent
 * @return
 */
NodeID pinpoint_start_trace(NodeID);
/**
 * the same as pinpoint_start_trace. BUT, end a span or a spanevent
 * @return
 */
NodeID pinpoint_end_trace(NodeID);

/**
 *  check id->traceNode is root
 * @param
 * @return 1: is root; 0: not root node;-1: A wrong id
 */
int pinpoint_trace_is_root(NodeID);

/**
 *  force end current trace, only called when callstack leaked
 * @return int 0 : means oK
 *             -1: exception found, check the log
 */
int pinpoint_force_end_trace(NodeID,int32_t timeout);

/**
 * pinpoint_add_clues, append a value into span[key]
 * @param key must be a string
 * @param value key must be a string
 */
void pinpoint_add_clues(NodeID _id,const char* key,const  char* value,E_NODE_LOC flag);
/**
 * pinpoint_add_clues, add  a key-value into span. span[key]=value
 * @param key must be a string
 * @param value key must be a string
 */
void pinpoint_add_clue(NodeID _id,const char* key,const  char* value,E_NODE_LOC flag);
/**
 *  add a key value into current trace. IF the trace is end, all data(key-value) will be free
 * @param key
 * @param value
 */
void pinpoint_set_context_key(NodeID _id,const char* key,const char* value);
/**
 * get the corresponding value with key(in current trace)
 * @param key
 * @return
 */
const char* pinpoint_get_context_key(NodeID _id,const char* key);


void pinpoint_set_context_long(NodeID _id,const char* key,long);

/**
 * @brief the value is a long type
 * 
 * @param _id  node id
 * @param key  string
 * @return int 1: failed
 *              0: success
 */
int pinpoint_get_context_long(NodeID _id,const char* key,long*);
/**
 * if tracelimit enable, check current trace state,
 * @param timestamp
 * @return 0, sampled or else, not sampled
 */
int check_tracelimit(int64_t);

/**
 * @brief setting current trace status
       typedef enum {
             E_OFFLINE = 0x1,
            E_TRACE_PASS =0x2,
            E_TRACE_BLOCK =0x4,
            E_READY = 0x8
        }E_AGENT_STATUS;
 * @param _id 
 * @param status 
 * @return int 
 */
int mark_current_trace_status(NodeID _id,int status);

/**
 * get an unique auto-increment id
 * NOTE: implement by shared memory, only valid in current host.
 * @return
 */
int64_t generate_unique_id(void);

/**
 * get the start time of collector-agent.Use to generate transactionID
 * @return
 */
uint64_t pinpoint_start_time(void);

/**
 * mark current span with error
 * @param msg
 * @param error_filename
 * @param error_lineno
 */
void catch_error(NodeID _id,const char* msg,const char* error_filename,uint32_t error_lineno);
typedef void(*log_msg_cb)(char*);
void register_error_cb(log_msg_cb error_cb);
void pp_trace(const char *format,...);
/**
 * NOTE: only for testcase
 */
void reset_unique_id(void);

void pinpoint_reset_store_layer(TraceStoreLayer* storeLayer);

#ifdef __cplusplus 
}
#endif
#endif /* COMMON_H_ */
