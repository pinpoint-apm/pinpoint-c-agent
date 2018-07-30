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
///* Include the required headers from httpd */
////#define AP_HAVE_DESIGNATED_INITIALIZER 1
//
//#include "httpd.h"
//#include "http_core.h"
//#include "http_protocol.h"
//#include "http_request.h"
//#include "http_config.h"
//#include "pinpoint_api.h"
//
//using std::string ;
//static std::map<string, string> _moduleInfo;
//
//#define DEFAULT_LOGPATH "/tmp"
//#define DEFAULT_LOGLEVER "DEBUG"
//
//static int32_t httpd_api = Pinpoint::API_UNDEFINED;
//static int32_t httpd_proxy_api = Pinpoint::API_UNDEFINED;
//
//#define IS_PINPOINT_STARTED_AND_RETURN(return_value) do \
//{\
//    Pinpoint::Agent::AgentPtr agentPtr = Pinpoint::Agent::Agent::getAgentPtr(); \
//    if (agentPtr == NULL || agentPtr->getAgentStatus() != Pinpoint::Agent::AGENT_STARTED) \
//    { \
//        return return_value; \
//    } \
//} while(0)
//
//#define IS_PINPOINT_STARTED_AND_RETURN_VOID() do \
//{\
//    Pinpoint::Agent::AgentPtr agentPtr = Pinpoint::Agent::Agent::getAgentPtr(); \
//    if (agentPtr == NULL || agentPtr->getAgentStatus() != Pinpoint::Agent::AGENT_STARTED) \
//    { \
//        return; \
//    } \
//} while(0)
//
//typedef struct
//{
//    const char *pinpoint_conf_file_path;
//} pinpoint_config;
//
//static pinpoint_config config = {NULL};
//
//static const char *pinpoint_set_conf_file_path(cmd_parms *cmd, void *cfg, const char *arg)
//{
//    config.pinpoint_conf_file_path = arg;
//    return NULL;
//}
//
//static const command_rec pinpoint_directives[] =
//        {
//                AP_INIT_TAKE1("PinpointConfigFilePath", (cmd_func)pinpoint_set_conf_file_path, NULL, RSRC_CONF,
//                              "Set pinpoint config file path."),
//                {NULL}
//        };
//
//static void init_log_from_ConfTree(Pinpoint::Configuration::Config &config)
//{
//    _moduleInfo["LogFileRootPath"] = config.readString( "common.LogFileRootPath",
//                                                        DEFAULT_LOGPATH);
//
//    _moduleInfo["PPLogLevel"] = config.readString( "common.PPLogLevel",
//                                                   DEFAULT_LOGLEVER);
//
//    Pinpoint::log::set_logging_file(_moduleInfo["LogFileRootPath"].c_str());
//    Pinpoint::log::set_log_level(_moduleInfo["PPLogLevel"].c_str());
//}
//
//void init_evn_before_agent_real_startup(Pinpoint::Configuration::Config &config)
//{
//    init_log_from_ConfTree(config);
//    LOGD(" start agent ...");
//}
//
//
//static void register_hooks(apr_pool_t *pool);
//
//static void pinpoint_child_init(apr_pool_t *pool, server_rec *s);
///* The four occasions to deal with pinpoint request/response*/
//static int request_in_handler(request_rec *r);
//static int request_sendto_backend_handler(request_rec *r);
//static int response_from_backend_handler(request_rec *r);
//static int response_out_handler(request_rec *r);
//
//static int32_t get_httpd_request_headers(request_rec *r, Pinpoint::Plugin::HeaderMap& headerMap);
//
//extern "C" {
//module AP_MODULE_DECLARE_DATA pinpoint_module =
//        {
//                STANDARD20_MODULE_STUFF,
//                NULL,            // Per-directory configuration handler
//                NULL,            // Merge handler for per-directory configurations
//                NULL,            // Per-server configuration handler
//                NULL,            // Merge handler for per-server configurations
//                pinpoint_directives,            // Any directives we may have for httpd
//                register_hooks   // Our hook registering function
//        };
//}
//
//
///* register_hooks: Adds a hook to the httpd process */
//static void register_hooks(apr_pool_t *pool)
//{
//
//    static const char *const asz[] = {"mod_proxy.c", NULL};
//
//    ap_hook_child_init(pinpoint_child_init, NULL, NULL, APR_HOOK_MIDDLE);
//
//    /* 1, request enter. */
//    ap_hook_post_read_request(request_in_handler, NULL, NULL, APR_HOOK_REALLY_FIRST);
//    /* 2, before request is sent to backend(Tomcat). */
//    ap_hook_handler(request_sendto_backend_handler, NULL, asz, APR_HOOK_MIDDLE);
//    /* 3, after get response from backend(Tomcat). */
//    ap_hook_log_transaction(response_from_backend_handler, NULL, NULL, APR_HOOK_REALLY_FIRST);
//    /* 4, before send response . */
//    ap_hook_log_transaction(response_out_handler, NULL, NULL, APR_HOOK_REALLY_LAST);
//}
//
//static void pinpoint_child_init(apr_pool_t *pool, server_rec *s)
//{
//    using namespace Pinpoint::Agent;
//
//    const char* configFileName = config.pinpoint_conf_file_path;
//
//    if(strcmp(configFileName,"") == 0) // not find ini file
//    {
//        return;
//    }
//
//    Pinpoint::Configuration::ConfFileObject fileobj(configFileName);
//
//    Pinpoint::Configuration::Config config(fileobj);
//
//    init_evn_before_agent_real_startup(config);
//
////    update_host_process_info();
//    int32_t err = Agent::createAgent();
//    if (err != Pinpoint::SUCCESS)
//    {
//        LOGE("Create Agent Failure!");
//        return;
//    }
//
//    AgentPtr agentPtr = Agent::getAgentPtr();
//    assert (agentPtr != NULL);
//
//    AgentFunction agentFunction;
//    //agentFunction.turnOnAOPFunc = turn_on_aop;
////    agentFunction.turnOffAOPFunc = turn_off_aop;
//    agentFunction.logOutputFunc = NULL;
//    agentFunction.addInterceptorFunc = NULL;
//    agentFunction.getHostProcessInfo = NULL;
//
//
//    err = agentPtr->preInit(Pinpoint::Agent::HTTPD_AGENT_TYPE,
//                            agentFunction,
//                            config);
//    if (err != Pinpoint::SUCCESS)
//    {
//        LOGE("agentPtr->preInit() Failure!");
//        return;
//    }
//
//    // no plugin for apache
//    Pinpoint::Plugin::PluginPtrVector pluginPtrVector;
//
//    err = agentPtr->init(pluginPtrVector);
//    if (err != Pinpoint::SUCCESS)
//    {
//        LOGE("init Agent Failure!");
//        return;
//    }
//
//    // register api
//
//    httpd_api = agentPtr->addApi("httpd process", -1, Pinpoint::API_WEB_REQUEST);
//    LOGI("httpd_api=%d", httpd_api);
//
//    httpd_proxy_api = agentPtr->addApi("httpd proxy", -1);
//    LOGI("httpd_proxy_api=%d", httpd_proxy_api);
//
//
//    err = agentPtr->start();
//
//    LOGI(" pinpoint agent start !!! err = %d", err);
//
//
//}
//
//static int request_in_handler(request_rec *r)
//{
//    IS_PINPOINT_STARTED_AND_RETURN(DECLINED);
//
//    Pinpoint::Plugin::HttpHeader* pHeader = Pinpoint::Plugin::RequestHeader::getCurrentRequestHeader();
//    if (pHeader != NULL)
//    {
//        Pinpoint::Plugin::HeaderMap headerMap;
//        if (get_httpd_request_headers(r, headerMap) == Pinpoint::SUCCESS)
//        {
//            pHeader->updateHeader(headerMap);
//        }
//    }
//
//    int32_t err = Pinpoint::Trace::Trace::startTrace(pHeader);
//    if (err == Pinpoint::SAMPLING_IGNORE)
//    {
//        return DECLINED;
//    }
//    else if (err != Pinpoint::SUCCESS)
//    {
//        LOGE("startTrace failed.");
//        return DECLINED;
//    }
//
//    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
//
//    assert (tracePtr != NULL);
//
//    tracePtr->setApiId(httpd_api);
//
//    // todo:
////    std::string endPoint = get_end_point();
////    if (endPoint != "")
////    {
////        tracePtr->setEndPoint(endPoint);
////    }
////
////    std::string remoteAddr = get_remote_addr();
////    if (remoteAddr != "")
////    {
////        tracePtr->setRemoteAddr(remoteAddr);
////    }
////
////    std::string rpc = get_rpc();
////    if (rpc != "")
////    {
////        tracePtr->setRpc(rpc);
////    }
//
//    return DECLINED;
//}
//
//static int request_sendto_backend_handler(request_rec *r)
//{
//    IS_PINPOINT_STARTED_AND_RETURN(DECLINED);
//
//    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
//    if (tracePtr == NULL)
//    {
//        return DECLINED;
//    }
//
//    // todo: how to know if the request will be send to backend. (if request static file??)
//    Pinpoint::Plugin::HeaderMap pinpointHeader;
//    int64_t nextSpanId;
//    int32_t err = tracePtr->getNextSpanInfo(pinpointHeader, nextSpanId);
//    if (err != Pinpoint::SUCCESS)
//    {
//        LOGE("request_sendto_backend_handler getNextSpanInfo failed.");
//        return DECLINED;
//    }
//
//    for(Pinpoint::Plugin::HeaderMap::iterator ip = pinpointHeader.begin(); ip != pinpointHeader.end(); ++ip)
//    {
//        apr_table_set(r->headers_in, ip->first.c_str(), ip->second.c_str());
//    }
//
//    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = tracePtr->traceBlockBegin();
//    if (spanEventRecorderPtr == NULL)
//    {
//        LOGE("traceBlockBegin failed");
//    }
//
//    spanEventRecorderPtr->setNextSpanId(nextSpanId);
//    spanEventRecorderPtr->markBeforeTime();
//    spanEventRecorderPtr->setServiceType(Pinpoint::HTTPD_PROXY);
//    spanEventRecorderPtr->setApiId(httpd_proxy_api);
//
//    return DECLINED;
//}
//
//static int response_from_backend_handler(request_rec *r)
//{
//    IS_PINPOINT_STARTED_AND_RETURN(DECLINED);
//
//    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
//    if (tracePtr == NULL)
//    {
//        return DECLINED;
//    }
//
//    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = tracePtr->getSpanEventRecorderPtr(0);
//    if (spanEventRecorderPtr == NULL)
//    {
//        return DECLINED;
//    }
//    spanEventRecorderPtr->markAfterTime();
//    tracePtr->traceBlockEnd(spanEventRecorderPtr);
//
//    return DECLINED;
//}
//
//static int response_out_handler(request_rec *r)
//{
//    IS_PINPOINT_STARTED_AND_RETURN(DECLINED);
//
//    if (Pinpoint::Trace::Trace::isStarted())
//    {
//        int32_t err = Pinpoint::Trace::Trace::endTrace();
//        if (err != Pinpoint::SUCCESS)
//        {
//            LOGE("endTrace failed: err = %d", err);
//        }
//    }
//
//    return DECLINED;
//}
//
//static int32_t get_httpd_request_headers(request_rec *r, Pinpoint::Plugin::HeaderMap& headerMap)
//{
//    apr_array_header_t * pp = (apr_array_header_t *) apr_table_elts(r->headers_in);
//    for (int i = 0; i < pp->nelts; i++)
//    {
//        apr_table_entry_t *ele = (apr_table_entry_t *)pp->elts+i;
//        if (ele->key != NULL && ele->val != NULL)
//        {
//            headerMap[std::string(ele->key)] = std::string(ele->val);
//        }
//    }
//    return Pinpoint::SUCCESS;
//}
