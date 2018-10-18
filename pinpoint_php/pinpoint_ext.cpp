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


static void start_pinpoint_agent();
static void start_pinpoint_agent_async();
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

STD_PHP_INI_ENTRY("pinpoint_agent.config_full_name", "", PHP_INI_SYSTEM,
        OnUpdateString,configFileName,zend_pinpoint_globals,pinpoint_globals)

STD_PHP_INI_ENTRY("pinpoint_agent.suppress_exp",  "0", PHP_INI_SYSTEM,
        OnUpdateBool, ignExp, zend_pinpoint_globals, pinpoint_globals)

PHP_INI_END()

#ifdef COMPILE_DL_PINPOINT
ZEND_GET_MODULE(pinpoint)
#endif

// we can not turn on aop in a request
volatile bool is_aop_turn_on = false;
boost::shared_ptr<boost::thread> agent_start_thread_ptr;

static void php_pinpoint_init_globals(zend_pinpoint_globals *_pinpoint_globals)
{
    _pinpoint_globals->configFileName = NULL;
    _pinpoint_globals->ignExp= 1;
}

PHP_MINIT_FUNCTION(pinpoint)
{
    ZEND_INIT_MODULE_GLOBALS(pinpoint, php_pinpoint_init_globals, NULL);

    REGISTER_INI_ENTRIES();


    if(strcmp(PINPOINT_G(configFileName),"") == 0) // not find ini file
    {
        // no effect for PHP
        return SUCCESS;
    }

    int32_t err = SUCCESS;
    try
    {
        Pinpoint::Configuration::ConfFileObject fileobj(PINPOINT_G(configFileName));
        Pinpoint::Configuration::Config config(fileobj);
        init_evn_before_agent_real_startup(config);

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
       fprintf(stderr,"pinpoint startup error "
                        "please check config file %s !!!\n", PINPOINT_G(configFileName));
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
    REGISTER_LONG_CONSTANT("PINPOINT_PHP_SERVER_TYPE", Pinpoint::PHP, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("PINPOINT_PHP_RPC_TYPE", Pinpoint::PHP_METHOD_CALL, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("PINPOINT_PHP_REMOTE", Pinpoint::PHP_REMOTE_METHOD, CONST_CS | CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("PINPOINT_ANNOTATION_RETURN", Pinpoint::Trace::AnnotationKey::RETURN_DATA, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("PINPOINT_ANNOTATION_ARGS", Pinpoint::Trace::AnnotationKey::ARGS0, CONST_CS | CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("PINPOINT_DEBUG", PP_LOG_DEBUG, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("PINPOINT_INFO", PP_LOG_INFO, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("PINPOINT_WARN", PP_LOG_WARNING, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("PINPOINT_ERROR", PP_LOG_ERROR, CONST_CS | CONST_PERSISTENT);

    REGISTER_STRING_CONSTANT("PINPOINT_SAMPLE_HTTP_HEADER", const_cast<char*>(Pinpoint::HTTP_SAMPLED), CONST_CS | CONST_PERSISTENT);
    REGISTER_STRING_CONSTANT("PINPOINT_SAMPLE_FALSE", const_cast<char*>(Pinpoint::Agent::Sampling::SAMPLING_RATE_FALSE), CONST_CS | CONST_PERSISTENT);

    // register hook
    // fix 151
    turn_on_aop();

    return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(pinpoint)
{
    if(agent_start_thread_ptr)
    {
        agent_start_thread_ptr->join();
    }

    AgentPtr agentPtr = Agent::getAgentPtr();
    if (agentPtr != NULL)
    {
        agentPtr->stop();
    }

    if (is_aop_turn_on)
    {
        turn_off_aop();
    }

    UNREGISTER_INI_ENTRIES();

#ifdef HAVE_GCOV
    __gcov_flush();
#endif

    return SUCCESS;
}


PHP_RINIT_FUNCTION(pinpoint)
{
    PhpRequestCounter::increment();
    memset(&PINPOINT_G(prs),0,sizeof(PRS));

    AgentPtr agentPtr = Agent::getAgentPtr();
    PINPOINT_ASSERT_RETURN((agentPtr != NULL), SUCCESS);

    if (!is_aop_turn_on && agentPtr->getAgentStatus() == Pinpoint::Agent::AGENT_STARTED)
    {
        is_aop_turn_on = true;
        RunOriginExecute::stop();
    }

    if (agentPtr->getAgentStatus() == Pinpoint::Agent::AGENT_PRE_INITED)
    {
        // Most of PHP configurations are not thread-safety.
        // So we read plugins that are wrote by php synchronized.
        load_php_interface_plugins();
        start_pinpoint_agent_async();
    }

    if (is_aop_turn_on && agentPtr->getAgentStatus() == Pinpoint::Agent::AGENT_STARTED)
    {
        // call longjmp: destructor is not called ...
        RunOriginExecute::stop();

        Pinpoint::Plugin::HttpHeader* header = Pinpoint::Plugin::RequestHeader::getCurrentRequestHeader();
        if (header != NULL)
        {
            Pinpoint::Plugin::HeaderMap headerMap;
            if (get_php_request_headers(headerMap) == SUCCESS)
            {
                header->updateHeader(headerMap);
            }
        }

        PhpAop *aop = PhpAop::getInstance();
        PINPOINT_ASSERT_RETURN ((aop != NULL), SUCCESS);

        Pinpoint::Plugin::InterceptorPtr interceptorPtr = aop->getRequestInterceptorPtr();

        if (interceptorPtr != NULL)
        {
            uint64_t call_id = interceptorPtr->assignCallId();
            aop->resetCurrentInterceptor(interceptorPtr, call_id);
            interceptorPtr->before(call_id, Pinpoint::Plugin::notSupportedFuncArgFetcher);
        }

    }

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(pinpoint)
{
    AgentPtr agentPtr = Agent::getAgentPtr();
    PINPOINT_ASSERT_RETURN ((agentPtr != NULL), SUCCESS);

    PhpAop *aop = PhpAop::getInstance();

    if (is_aop_turn_on && agentPtr->getAgentStatus() == Pinpoint::Agent::AGENT_STARTED)
    {
        PINPOINT_ASSERT_RETURN ((aop != NULL), SUCCESS);
        Pinpoint::Plugin::InterceptorPtr requestInterceptorPtr = aop->getRequestInterceptorPtr();

        // maybe user call exit
        CurrentInterceptorInfo currentInterceptorInfo = aop->getCurrentInterceptorInfo();
        Pinpoint::Plugin::InterceptorPtr interceptorPtr = currentInterceptorInfo.first;
        uint64_t call_id = currentInterceptorInfo.second;
        if (interceptorPtr != NULL && interceptorPtr != requestInterceptorPtr)
        {
            PINPOINT_ASSERT_RETURN((call_id != Pinpoint::Plugin::INVALID_CALL_ID), FAILED);
            Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
            if (tracePtr != NULL)
            {
                Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = tracePtr->getSpanEventRecorderPtr(call_id);
                if (spanEventRecorderPtr != NULL)
                {
                    spanEventRecorderPtr->markAfterTime();
                    tracePtr->traceBlockEnd(spanEventRecorderPtr);
                }
            }
        }

        Pinpoint::Plugin::HttpHeader* header = Pinpoint::Plugin::ResponseHeader::getCurrentResponseHeader();
        if (header != NULL)
        {
            Pinpoint::Plugin::HeaderMap headerMap;
            if (get_php_response_headers(headerMap) == SUCCESS)
            {
                header->updateHeader(headerMap);
            }
        }

        if (requestInterceptorPtr != NULL)
        {
            requestInterceptorPtr->end(Pinpoint::Plugin::IGNORE_CALL_ID,
                                       Pinpoint::Plugin::notSupportedFuncArgFetcher,
                                       Pinpoint::Plugin::notSupportedFuncResultFetcher);
        }


        aop->resetCurrentInterceptor();
        aop->reqShutdownClean();
    }

#ifdef HAVE_GCOV
    __gcov_flush();
#endif

    return SUCCESS;
}

static void start_pinpoint_agent()
{
    AgentPtr agentPtr = Agent::getAgentPtr();
    PINPOINT_ASSERT (agentPtr != NULL);

    int32_t err;

    PluginPtrVector pluginPtrVector;

    PhpPluginManager *phpPluginManager = PhpPluginManager::getInstance();
    if (phpPluginManager == NULL)
    {
        LOGE("get PhpPluginManager failed.");
        return;
    }

    err = phpPluginManager->registerPlugins();
    if (err != SUCCESS)
    {
        LOGE("registerPlugins failed.");
        return;
    }

    PluginPtrVector& v1 = phpPluginManager->getAllPlugins();

    pluginPtrVector.insert(pluginPtrVector.end(), v1.begin(), v1.end());

    LOGT("c++ plugin count=%d", v1.size());

    PhpInterfacePluginManager* interfacePluginManager = PhpInterfacePluginManager::getManager();
    PINPOINT_ASSERT (interfacePluginManager != NULL);

    PluginPtrVector& v2 = interfacePluginManager->getAllPlugins();

    pluginPtrVector.insert(pluginPtrVector.end(), v2.begin(), v2.end());
    LOGT("php plugin count=%d", v2.size());

    LOGT("all plugins count = %d", pluginPtrVector.size());

    err = agentPtr->init(pluginPtrVector);
    if (err != SUCCESS)
    {
        LOGE("init Agent failed!");
        return;
    }

    err = agentPtr->start();

    LOGI(" pinpoint agent start !!! code = %d", err);
}

static void load_php_interface_plugins()
{
    TSRMLS_FETCH();

    int32_t err;

    Pinpoint::Agent::AgentConfigArgsPtr contextPtr =
            Pinpoint::Agent::PinpointAgentContext::getContextPtr()->agentConfigArgsPtr;
    PINPOINT_ASSERT(contextPtr != NULL);

    const char* phpCreatePluginsFileName = contextPtr->pluginFileName.c_str();
    std::string pluginsDir =  contextPtr->pluginDir;

    /* interfaces */
    zend_file_handle *prepend_file_p;
    zend_file_handle prepend_file;
    memset(&prepend_file, 0, sizeof(zend_file_handle));

    const std::string p = path_join(pluginsDir, phpCreatePluginsFileName);
    LOGD("plugins full path %s", p.c_str());

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
//    zend_try{
    err = zend_execute_scripts(ZEND_INCLUDE TSRMLS_CC, NULL, 1, prepend_file_p);
    if (err != SUCCESS)
    {
        LOGE("register php interface plugins failed.");
    }
//    } zend_catch{
//        LOGE("zend_execute_scripts failed. Maybe your plugins have syntax error. Please see PHP log to check the error.");
//    } zend_end_try();
    CG(start_lineno) = orig_start_lineno;
}


static void start_pinpoint_agent_async()
{

    Pinpoint::Agent::PinpointAgentContextPtr& contextPtr = Pinpoint::Agent::PinpointAgentContext::getContextPtr();
    PINPOINT_ASSERT (contextPtr != NULL);

    contextPtr->ip = get_host_process_info(Pinpoint::Naming::SERVER_ADDR);
    contextPtr->ports = get_host_process_info(Pinpoint::Naming::SERVER_PORT);
    contextPtr->hostname = get_host_process_info(Pinpoint::Naming::HTTP_HOST);

    agent_start_thread_ptr.reset(new boost::thread(start_pinpoint_agent));
}


