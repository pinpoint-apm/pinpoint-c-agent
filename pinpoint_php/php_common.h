/*******************************************************************************
 * Copyright 2018 NAVER Corp.
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
#ifndef PINPOINT_PHP_COMMON_H
#define PINPOINT_PHP_COMMON_H

#include "php_define_wapper.h"
#include "pinpoint_api.h"
#include "php_pinpoint.h"


#include <boost/thread.hpp>

#define PP_SERVER "_SERVER"
#define PP_HTTP_HOST "HTTP_HOST"
#define PP_HTTP_X_FORWARDED_HOST "HTTP_X_FORWARDED_HOST"
#define PP_REMOTE_ADDR "REMOTE_ADDR"
#define PP_REMOTE_PORT "REMOTE_PORT"
#define PP_SERVER_ADDR "SERVER_ADDR"
#define PP_SERVER_PORT "SERVER_PORT"
#define PP_REQUEST_URI "REQUEST_URI"

#define PP_NGINX_PROXY "Pinpoint-ProxyNginx"
#define PP_APACHE_PROXY "Pinpoint-ProxyApache"
#define PP_APP_PROXY "Pinpoint-ProxyApp"

#define PP_REDIRECT_STATUS "REDIRECT_STATUS"
#define MAX_STRING_FROM_ZVAL 1024

#define MAX_ANNOTATION_SIZE 10240                            // 10k
#define TER_MIN(a,b,c) (((a<b)&&(a<c))?(a):((b>c)?(c):(b)))
#define MAX_ARRAY_ELEMENT 10

#define AGENT_ERROR  (E_ERROR|E_PARSE|E_CORE_ERROR|E_COMPILE_ERROR|E_USER_ERROR|E_RECOVERABLE_ERROR)
#define AGENT_WARNG  (E_WARNING|E_NOTICE|E_CORE_WARNING|E_COMPILE_WARNING|E_USER_WARNING|E_USER_NOTICE|E_DEPRECATED|E_STRICT|E_USER_DEPRECATED)

enum UNIT_TEST_TYPE{E_NO_TEST=0,E_TSAPN=0x1,E_BGTASK=0x2};

#define PP_U_TRACE(msg,...) (PINPOINT_G(testCovered)?(fprintf(stderr,"%*s" msg"\n",PINPOINT_G(prs).stackDepth*2,"",##__VA_ARGS__)):(0));

#define PP_TRACE(msg,...)\
        do{\
            PP_U_TRACE(msg,##__VA_ARGS__);\
            LOGT(msg,##__VA_ARGS__);\
        }while(0)


using Pinpoint::Naming::eName;

typedef std::map<std::string,std::string> KV;
typedef std::vector<std::string> VecStr;
typedef std::vector<std::string>::iterator iVecStr;

extern int32_t get_php_request_headers(Pinpoint::Plugin::HeaderMap& headerMap);

extern int32_t get_php_response_headers(Pinpoint::Plugin::HeaderMap& headerMap);

extern std::string get_end_point();

extern std::string get_remote_addr();

extern std::string get_rpc();

/// not thread safe
const char* php_getcwd();

int file_exist_and_readable(std::string& fullName);

void get_plugins_by_php(Pinpoint::Plugin::PluginPtrVector &);

extern int32_t get_http_response_status();

extern bool get_proxy_http_header(std::string &,int&);

extern void init_evn_before_agent_real_startup();

extern std::string get_host_process_info(eName name);

const char PHP_REQUEST_FUNC_NAME[] = "_request";

extern bool is_interface_impl(zval* obj, const char* interface_name);

extern bool is_class_impl(zval* obj, const char* class_name);

extern const std::string zval_to_string(zval* value,uint32_t);

extern std::string path_join(std::string , std::string );

extern const Pinpoint::Plugin::ExceptionInfo get_exception_info(EG_EXP_TPYE exception);

void zval_to_Map(KV &map, zval* val, int32_t maxELemSize,int32_t maxDataSize);

void zval_args_to_vec(VecStr &vec, zval* val, int32_t maxELemSize,int32_t maxDataSize);

void map_to_str(const KV &map,std::string &str);

void vec_to_str(iVecStr istart,iVecStr iend,std::string& out);

int init_pinpoint_agent();

void start_pinpoint_agent();

void start_a_new_calltrace();

void end_current_calltrace();


class PhpRequestCounter
{
public:
    static const int64_t INVALID_COUNT = -1;
    static int64_t getCount();
    static void increment();
private:
    static boost::thread_specific_ptr<int64_t> pCounter;
    static int64_t* getPCounter();
};


class PObjectCache
{
public:
    PObjectCache();

    ~PObjectCache();

    int32_t store(zval* value, const char* classDefinitionPath = NULL);

    int32_t callMethod(const char* methodName, int32_t methodNameLen, zval* returnValue,
                       int32_t parameterCount, zval* arg1, zval* arg2);

    int32_t callMethod(const char* methodName, int32_t methodNameLen,
                        int32_t parameterCount, zval* arg1, zval* arg2);

    void reqShutDownHandler();
private:

    int32_t load();

    unsigned char *buf;
    uint64_t bufLen;
    char* classDefinitionPath;
    bool isCacheSuccess;
    zend_uchar type;
    std::string className;
    zval* obj;

    zval* zval_class_instance;

    int64_t requestNum;
};

class RunOriginExecute
{
private:
    static int iRunning;

    static int iTimes;
    int        tTimes;

public:
    RunOriginExecute(void)
    {
        // note if use want it become threadsafe
        // pls use threadself iRuning ...
        iRunning = 1;
        tTimes = iTimes++;
//        LOGD("DeclarePluginFun %d %ld", tTimes, gettid());
    }

    ~RunOriginExecute(void)
    {
        iRunning = 0;
//        LOGD("~DeclarePluginFun %d %ld", tTimes, gettid());
    }

    static void start(void)
    {
        iRunning = 1;
    }

    static void stop(void)
    {
        iRunning = 0;
    }

    static int running(void)
    {
        return iRunning;
    }

};

#endif
