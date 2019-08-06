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
#ifndef PHP_PINPOINT_PHP_H
#define PHP_PINPOINT_PHP_H

#include "common.h"
#include "config.h"
//#include "pinpoint_helper.h"
#include <json/json.h>

extern zend_module_entry pinpoint_php_module_entry;
#define phpext_pinpoint_php_ptr &pinpoint_php_module_entry

#define PHP_PINPOINT_PHP_VERSION "0.2.0"

#ifdef PHP_WIN32
#	define PHP_PINPOINT_PHP_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_PINPOINT_PHP_API __attribute__ ((visibility("default")))
#else
#	define PHP_PINPOINT_PHP_API
#endif


#ifdef ZTS
#include "TSRM.h"
#endif

///todo
/// # must be null every request
///     root_value, cur_value
///


#define MAX_VEC 512
#define LOG_SIZE 4096
#define IN_MSG_BUF_SIZE 4096
#define NAMING_SIZE 128
#define SOBJ_ADDRESS "/dev/shm/pinpoint-php.shm"

typedef enum{
    RESPONSE_AGENT_INFO = 0,
    REQ_UPDATE_SPAN = 1
}MSG_TYPE;

typedef struct{
    uint type;
    uint length;
}__attribute__((aligned(1))) Header;



typedef struct trans_layer_t TransLayer;
typedef int  (*TransHandleCB)(TransLayer*);

class Chunks;

typedef struct trans_layer_t{
    int           c_fd;      // collector fd, use to send data;
    Chunks*        chunks; // A fixed size for span [0,MAX_VEC]
    TransHandleCB socket_read_cb;
    TransHandleCB socket_write_cb;
    char           in_buf[IN_MSG_BUF_SIZE];
}TransLayer;


typedef struct collector_agent_s{
    uint64_t start_time;
    char*   appid;
    char*   appname;
}CollectorAgentInfo;

typedef struct shared_object_s{
    void* region;
    int length;
    char  address[NAMING_SIZE];
}SharedObject_T;

enum{
    UNIQUE_ID_OFFSET = 0,
    TRACE_LIMIT= 8,
};

ZEND_BEGIN_MODULE_GLOBALS(pinpoint_php)
    char*  co_host; // tcp:ip:port should support dns
    void*  root;    // Json::Value root
    void*  writer;
    CollectorAgentInfo agent_info;
    uint  w_timeout_ms;
    zend_bool   utest_flag;
    zend_bool   limit;
    int   tracelimit;
    void*  call_stack;
    TransLayer t_layer;
    char logBuffer[LOG_SIZE];
    SharedObject_T shared_obj;
    uint64_t fetal_error_time;
ZEND_END_MODULE_GLOBALS(pinpoint_php)

extern ZEND_DECLARE_MODULE_GLOBALS(pinpoint_php);

#ifdef ZTS
#define PPG(v) TSRMG(pinpoint_php_globals_id, zend_pinpoint_php_globals *, v)
#else
#define PPG(v) (pinpoint_php_globals.v)
#endif

#if defined(ZTS) && defined(COMPILE_DL_PINPOINT_PHP)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

PHP_MINIT_FUNCTION(pinpoint_php);
PHP_MSHUTDOWN_FUNCTION(pinpoint_php);
PHP_RINIT_FUNCTION(pinpoint_php);
PHP_RSHUTDOWN_FUNCTION(pinpoint_php);
PHP_MINFO_FUNCTION(pinpoint_php);
#endif	/* PHP_PINPOINT_PHP_H */
