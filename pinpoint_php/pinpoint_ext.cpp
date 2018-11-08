////////////////////////////////////////////////////////////////////////////////
// Copyright 2018 NAVER Corp.
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Zend/zend_interfaces.h>
#include "php.h"
#include "php_ini.h"
#include "SAPI.h"
#include "zend_extensions.h"
#include "zend_exceptions.h"
#include "php_pinpoint.h"
#include "pinpoint_api.h"
#include "aop_hook.h"
#include "php_common.h"
#include "php_plugin.h"
#include "php_interfaces.h"

using  Pinpoint::FAILED;
using  Pinpoint::Agent::Agent;
using  Pinpoint::Agent::AgentPtr;
using  Pinpoint::Agent::AgentFunction;
using  Pinpoint::Plugin::Plugin;
using  Pinpoint::Plugin::PluginPtrVector;


static void load_php_interface_plugins();


#ifdef HAVE_GCOV
 extern "C"  void __gcov_flush(void); 
#endif

//<editor-fold desc="Pinpoint function">

ZEND_BEGIN_ARG_INFO_EX(arginfo_add_plugin, 0, 0, 2)
    ZEND_ARG_INFO(0, plugin)
    ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_add_api, 0, 0, 2)
    ZEND_ARG_INFO(0, api_info)
    ZEND_ARG_INFO(0, line_no)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_log, 0, 0, 2)
    ZEND_ARG_INFO(0, level)
    ZEND_ARG_INFO(0, info)
ZEND_END_ARG_INFO()

const static zend_function_entry pinpoint_functions[] = {
    PHP_FE(pinpoint_start_trace, NULL)
    PHP_FE(pinpoint_get_current_trace, NULL)
    PHP_FE(pinpoint_end_trace, NULL)
    PHP_FE(pinpoint_add_plugin, NULL)
    PHP_FE(pinpoint_add_api, arginfo_add_api)
    PHP_FE(pinpoint_log, arginfo_log)

    PHP_FE(pinpint_aop_reload,NULL)
    PHP_FE(pinpoint_data_thread_start,NULL)

    PHP_FE(pinpoint_start_calltrace ,NULL)
    PHP_FE(pinpoint_end_calltrace,NULL)

    {NULL, NULL, NULL, 0, 0}
};
//</editor-fold>

zend_module_entry pinpoint_module_entry =
{
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_PINPOINT_EXTNAME,
    pinpoint_functions,
    PHP_MINIT(pinpoint),
    PHP_MSHUTDOWN(pinpoint),
    PHP_RINIT(pinpoint),
    PHP_RSHUTDOWN(pinpoint),
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_PINPOINT_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};


//extern zend_module_entry pinpoint_module_entry;
//#define phpext_pinpoint_prt &pinpoint_module_entry

ZEND_DECLARE_MODULE_GLOBALS(pinpoint)

PHP_INI_BEGIN()

// discard
//STD_PHP_INI_ENTRY("pinpoint_agent.config_full_name", "", PHP_INI_SYSTEM,OnUpdateString,configFileName,zend_pinpoint_globals,pinpoint_globals)

STD_PHP_INI_ENTRY("pinpoint_agent.pluginsRootPath", "", PHP_INI_SYSTEM,
        OnUpdateString,pluginsRootPath,zend_pinpoint_globals,pinpoint_globals)
STD_PHP_INI_ENTRY("pinpoint_agent.entryFilename", "", PHP_INI_SYSTEM,
        OnUpdateString,entryFilename,zend_pinpoint_globals,pinpoint_globals)

STD_PHP_INI_ENTRY("pinpoint_agent.trace_exception",  "0", PHP_INI_SYSTEM,
        OnUpdateBool, trace_exception, zend_pinpoint_globals, pinpoint_globals)

STD_PHP_INI_ENTRY("pinpoint_agent.unittest",  "0", PHP_INI_SYSTEM,
        OnUpdateBool, unittest, zend_pinpoint_globals, pinpoint_globals)

STD_PHP_INI_ENTRY("pinpoint_agent.pinpoint_enable","false",PHP_INI_SYSTEM,
        OnUpdateBool,module_enable,zend_pinpoint_globals,pinpoint_globals)

STD_PHP_INI_ENTRY("profiler.proxy.http.header.enable",  "true", PHP_INI_SYSTEM,
        OnUpdateBool, proxy_headers, zend_pinpoint_globals, pinpoint_globals)

        // log
STD_PHP_INI_ENTRY("pinpoint.common.LogFileRootPath", "/tmp/", PHP_INI_SYSTEM,
        OnUpdateString, logFileRootPath, zend_pinpoint_globals, pinpoint_globals)
STD_PHP_INI_ENTRY("pinpoint.common.PPLogLevel", "DEBUG", PHP_INI_SYSTEM,
        OnUpdateString, PPLogLevel, zend_pinpoint_globals, pinpoint_globals)

        // agentid
STD_PHP_INI_ENTRY("pinpoint.common.AgentID","uninitiated agentid", PHP_INI_SYSTEM,
        OnUpdateString,agentID,zend_pinpoint_globals,pinpoint_globals)
STD_PHP_INI_ENTRY("pinpoint.common.ApplicationName","uninitiated applicationName", PHP_INI_SYSTEM,
        OnUpdateString,applicationName,zend_pinpoint_globals,pinpoint_globals)

        // collector info
STD_PHP_INI_ENTRY("pinpoint.common.CollectorSpanIp","127.0.0.1", PHP_INI_SYSTEM,
        OnUpdateString,collectorSpanIp,zend_pinpoint_globals,pinpoint_globals)
STD_PHP_INI_ENTRY("pinpoint.common.CollectorSpanPort","8000", PHP_INI_SYSTEM,
        OnUpdateLong,CollectorSpanPort,zend_pinpoint_globals,pinpoint_globals)

STD_PHP_INI_ENTRY("pinpoint.common.CollectorStatIp","127.0.0.1", PHP_INI_SYSTEM,
        OnUpdateString,CollectorStatIp,zend_pinpoint_globals,pinpoint_globals)
STD_PHP_INI_ENTRY("pinpoint.common.CollectorStatPort","8000", PHP_INI_SYSTEM,
        OnUpdateLong,CollectorStatPort,zend_pinpoint_globals,pinpoint_globals)

STD_PHP_INI_ENTRY("pinpoint.common.CollectorTcpIp","127.0.0.1", PHP_INI_SYSTEM,
        OnUpdateString,CollectorTcpIp,zend_pinpoint_globals,pinpoint_globals)
STD_PHP_INI_ENTRY("pinpoint.common.CollectorTcpPort","8000", PHP_INI_SYSTEM,
        OnUpdateLong,CollectorTcpPort,zend_pinpoint_globals,pinpoint_globals)

        //Plugins exclude and include
STD_PHP_INI_ENTRY("pinpoint.common.PluginExclude","", PHP_INI_SYSTEM,
        OnUpdateString,PluginExclude,zend_pinpoint_globals,pinpoint_globals)
STD_PHP_INI_ENTRY("pinpoint.common.PluginInclude","", PHP_INI_SYSTEM,
        OnUpdateString,PluginInclude,zend_pinpoint_globals,pinpoint_globals)

        // timeout
STD_PHP_INI_ENTRY("pinpoint.common.ReconTimeOut","5",PHP_INI_SYSTEM,
        OnUpdateLong,reconInterval,zend_pinpoint_globals,pinpoint_globals)

        // Trace
STD_PHP_INI_ENTRY("pinpoint.common.TraceLimit","-1",PHP_INI_SYSTEM,
        OnUpdateLong,TraceLimit,zend_pinpoint_globals,pinpoint_globals)
STD_PHP_INI_ENTRY("pinpoint.common.SkipTraceTime","-1",PHP_INI_SYSTEM,
        OnUpdateLong,SkipTraceTime,zend_pinpoint_globals,pinpoint_globals)

        // API
STD_PHP_INI_ENTRY("pinpoint.common.ApiTableFile","",PHP_INI_SYSTEM,
        OnUpdateString,ApiTableFile,zend_pinpoint_globals,pinpoint_globals)

PHP_INI_END()

#ifdef COMPILE_DL_PINPOINT
ZEND_GET_MODULE(pinpoint)
#endif


#define IS_MODULE_ENABLE()\
do{\
    if(PINPOINT_G(module_enable) == 0) return SUCCESS;\
}while(0)

void print_ini()
{
//    LOGD("configFileName:%s",PINPOINT_G(configFileName));
    LOGD("trace_exception:%d",PINPOINT_G(trace_exception));
    LOGD("unittest:%d",PINPOINT_G(unittest));
    LOGD("pluginsRootPath:%d",PINPOINT_G(pluginsRootPath));
    LOGD("entryFilename:%d",PINPOINT_G(entryFilename));
    LOGD("proxy.http.header.enable:%d",PINPOINT_G(proxy_headers));

    LOGD("common.agentID:%s",PINPOINT_G(agentID));
    LOGD("common.applicationName:%s",PINPOINT_G(applicationName));
    LOGD("common.collectorSpanIp:%s",PINPOINT_G(collectorSpanIp));
    LOGD("common.CollectorSpanPort:%l",PINPOINT_G(CollectorSpanPort));
    LOGD("common.CollectorStatIp:%s",PINPOINT_G(CollectorStatIp));
    LOGD("common.CollectorStatPort:%l",PINPOINT_G(CollectorStatPort));
    LOGD("common.CollectorTcpIp:%s",PINPOINT_G(CollectorTcpIp));
    LOGD("common.CollectorTcpPort:%l",PINPOINT_G(CollectorTcpPort));
    LOGD("common.PluginExclude:%s",PINPOINT_G(PluginExclude));
    LOGD("common.PluginInclude:%s",PINPOINT_G(PluginInclude));

    LOGD("common.TraceLimit:%d",PINPOINT_G(TraceLimit));
    LOGD("common.SkipTraceTime:%d",PINPOINT_G(SkipTraceTime));

    LOGD("common.reconInterval:%l",PINPOINT_G(reconInterval));
    LOGD("common.ApiTableFile:%s",PINPOINT_G(ApiTableFile));
}


static void php_pinpoint_init_globals(zend_pinpoint_globals *_pinpoint_globals)
{
//    _pinpoint_globals->trace_exception= 1;
}

PHP_MINIT_FUNCTION(pinpoint)
{
    ZEND_INIT_MODULE_GLOBALS(pinpoint, php_pinpoint_init_globals, NULL);

    REGISTER_INI_ENTRIES();

    IS_MODULE_ENABLE();

    if( PINPOINT_G(pluginsRootPath) == NULL){
        strncpy(PINPOINT_G(pluginsAbsolutePath),expand_filepath(PINPOINT_G(pluginsRootPath),NULL),MAXPATHLEN);
    }


    int32_t err = SUCCESS;
    try
    {
        init_evn_before_agent_real_startup();

        err = register_pinpoint_class();
        if (err != SUCCESS)
        {
            LOGE("register_pinpoint_class failed!");
            return FAILED;
        }

        err = Agent::createAgent();
        if (err != SUCCESS)
        {
            LOGE("Create Agent failed!");
            return FAILED;
        }

        AgentPtr agentPtr = Agent::getAgentPtr();
        PINPOINT_ASSERT_RETURN ((agentPtr != NULL), FAILED);

        AgentFunction agentFunction;
        agentFunction.logOutputFunc = NULL;
        agentFunction.addInterceptorFunc = add_interceptor;
        agentFunction.getHostProcessInfo = get_host_process_info;

#define IS_SET(offset) (strlen(PINPOINT_G(offset))==0?(0):(1))

        Pinpoint::Agent::AgentConfigArgs config= {
                PINPOINT_G(agentID),
                PINPOINT_G(applicationName),
                PINPOINT_G(collectorSpanIp),
                PINPOINT_G(CollectorSpanPort),
                PINPOINT_G(CollectorStatIp),
                PINPOINT_G(CollectorStatPort),
                PINPOINT_G(CollectorTcpIp),
                PINPOINT_G(CollectorTcpPort),
                PINPOINT_G(PluginInclude),
                PINPOINT_G(PluginExclude),
                IS_SET(ApiTableFile),
                PINPOINT_G(ApiTableFile),
                PINPOINT_G(TraceLimit),
                PINPOINT_G(SkipTraceTime),
                PINPOINT_G(reconInterval),
                IS_SET(PluginInclude),
                IS_SET(PluginExclude),
        };

        err = agentPtr->preInit(Pinpoint::Agent::PHP_AGENT_TYPE,
                                agentFunction,
                                config);
        if (err != SUCCESS)
        {
            LOGE("agentPtr->preInit() failed!");
            return FAILED;
        }

    }
    catch (std::invalid_argument& ex)
    {
       fprintf(stderr,"pinpoint startup error please check php.ini file  !!!\n");
       return FAILED;
    }
    catch (...)
    {
       fprintf(stderr,"pinpoint startup error with unknown error!!! \n");
       return FAILED;
    }

    err = init_aop();
    if (err != SUCCESS)
    {
        LOGE("init aop failed!");
        return FAILED;
    }

    /* constant */
    REGISTER_LONG_CONSTANT("PINPOINT_PHP_SERVER_TYPE", (long)Pinpoint::PHP, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("PINPOINT_PHP_RPC_TYPE", (long)Pinpoint::PHP_METHOD_CALL, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("PINPOINT_PHP_REMOTE", (long)Pinpoint::PHP_REMOTE_METHOD, CONST_CS | CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("PINPOINT_ANNOTATION_RETURN", (long)Pinpoint::Trace::AnnotationKey::RETURN_DATA, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("PINPOINT_ANNOTATION_ARGS", (long)Pinpoint::Trace::AnnotationKey::ARGS0, CONST_CS | CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("PINPOINT_DEBUG", PP_LOG_DEBUG, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("PINPOINT_INFO", PP_LOG_INFO, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("PINPOINT_WARN", PP_LOG_WARNING, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("PINPOINT_ERROR", PP_LOG_ERROR, CONST_CS | CONST_PERSISTENT);

    REGISTER_STRING_CONSTANT("PINPOINT_SAMPLE_HTTP_HEADER", const_cast<char*>(Pinpoint::HTTP_SAMPLED), CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("PINPOINT_SAMPLE_FALSE", const_cast<char*>(Pinpoint::Agent::Sampling::SAMPLING_RATE_FALSE), CONST_CS | CONST_PERSISTENT);

    turn_on_aop();
    print_ini();
    LOGI("pinpoint module initialized");
    return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(pinpoint)
{
    IS_MODULE_ENABLE();
    LOGI("pinpoint module shutdown ");

    AgentPtr agentPtr = Agent::getAgentPtr();
    if (agentPtr != NULL)
    {
        agentPtr->stop();
    }

    turn_off_aop();

    UNREGISTER_INI_ENTRIES();

#ifdef HAVE_GCOV
    __gcov_flush();
#endif


    return SUCCESS;
}

/// only valid in php-fpm
PHP_RINIT_FUNCTION(pinpoint)
{
    IS_MODULE_ENABLE();

    AgentPtr agentPtr = Agent::getAgentPtr();
    PINPOINT_ASSERT_RETURN((agentPtr != NULL), SUCCESS);

    if (agentPtr->getAgentStatus() == Pinpoint::Agent::AGENT_PRE_INITED)
    {
        // Most of PHP configurations are not thread-safety.
        // So we read plugins that are wrote by php synchronized.
        load_php_interface_plugins();

        if(init_pinpoint_agent() != SUCCESS)
        {
            return -1;
        }

        if( PINPOINT_G(unittest) == 0) // unittest not enable
        {
            start_pinpoint_agent();
        }

        start_a_new_calltrace();
    }


    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(pinpoint)
{
    IS_MODULE_ENABLE();
    end_current_calltrace();

#ifdef HAVE_GCOV
    __gcov_flush();
#endif

    return SUCCESS;
}

static void load_php_interface_plugins()
{
    TSRMLS_FETCH();

    const char* phpCreatePluginsFileName = PINPOINT_G(entryFilename);
    std::string pluginsDir =  PINPOINT_G(pluginsAbsolutePath);

    /* interfaces */
    zend_file_handle *prepend_file_p;
    zend_file_handle prepend_file;
    memset(&prepend_file, 0, sizeof(zend_file_handle));

    std::string p = path_join(pluginsDir, phpCreatePluginsFileName);
    LOGD("plugins full path %s", p.c_str());

    if(file_exist_and_readable(p) == -1)
    {
        LOGW("\n\n");
        LOGW("------------------------------------------------------------------------------------------------------------------------------------");
        LOGW("------ %s can't read it ---------------------",p.c_str());
        LOGW("------------------------------------------------------------------------------------------------------------------------------------");
        LOGW("\n\n");
        return ;
    }

#if PHP_VERSION_ID < 50400
    prepend_file.filename = const_cast<char*>(p.c_str());
#else
    prepend_file.filename = p.c_str();
#endif
    prepend_file.opened_path = NULL;
    prepend_file.free_filename = 0;
    prepend_file.type = ZEND_HANDLE_FILENAME;
    prepend_file_p = &prepend_file;

    int orig_start_lineno = CG(start_lineno);
    CG(start_lineno) = 0;

    zend_try{
        zend_execute_scripts(ZEND_INCLUDE TSRMLS_CC, NULL, 1, prepend_file_p);
    } zend_catch{
        LOGE("\n\n");
        LOGE("------------------------------------------------------------------------------------------------------------------------------------");
        LOGE("------------your plugins have syntax error. Please see PHP log to check the error.--------------------------------------------------");
        LOGE("------------------------------------------------------------------------------------------------------------------------------------");
        LOGE("\n\n");
    } zend_end_try();

    CG(start_lineno) = orig_start_lineno;
}


