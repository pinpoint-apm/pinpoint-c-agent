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


using Pinpoint::Naming::eName;

typedef std::map<std::string,std::string> KV;
typedef std::vector<std::string> VecStr;
typedef std::vector<std::string>::iterator iVecStr;

extern int32_t get_php_request_headers(Pinpoint::Plugin::HeaderMap& headerMap);

extern int32_t get_php_response_headers(Pinpoint::Plugin::HeaderMap& headerMap);

extern std::string get_end_point();

extern std::string get_remote_addr();

extern std::string get_rpc();

extern int32_t get_http_response_status();

extern bool get_proxy_http_header(std::string &,int&);

extern void init_evn_before_agent_real_startup(Pinpoint::Configuration::Config&);

extern std::string get_host_process_info(eName name);

const char PHP_REQUEST_FUNC_NAME[] = "_request";

extern bool is_interface_impl(zval* obj, const char* interface_name);

extern bool is_class_impl(zval* obj, const char* class_name);

extern const std::string zval_to_string(zval* value,int32_t);

extern const std::string path_join(std::string , std::string );

extern const Pinpoint::Plugin::ExceptionInfo get_exception_info(EG_EXP_TPYE exception);

void zval_to_Map(KV &map, zval* val, int32_t maxELemSize,int32_t maxDataSize);

void zval_args_to_vec(VecStr &vec, zval* val, int32_t maxELemSize,int32_t maxDataSize);

void map_to_str(const KV &map,std::string &str);

void vec_to_str(iVecStr istart,iVecStr iend,std::string& out);

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
    size_t bufLen;
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
        LOGD("DeclarePluginFun %d %ld", tTimes, gettid());
    }

    ~RunOriginExecute(void)
    {
        iRunning = 0;
        LOGD("~DeclarePluginFun %d %ld", tTimes, gettid());
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

extern AgentConf g_AgentConf;
#endif
