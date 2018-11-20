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
#define __STDC_LIMIT_MACROS 
#include <stdint.h>



#if PHP_VERSION_ID < 50500
#include "zend.h"
#endif

#include "php_common.h"
#include "../cplugins/internel_functions_plugin.h"

#include "zend_types.h"

using namespace Pinpoint::Trace;
using namespace Pinpoint::log;
using namespace Pinpoint::Agent;

using Pinpoint::FAILED;

PhpInternalFunctionsPlugin::PhpInternalFunctionsPlugin()
        : phpInfoApiId(INVALID_API_ID)
{

}

int32_t PhpInternalFunctionsPlugin::init()
{
    AgentPtr& agentPtr = Agent::getAgentPtr();
    assert (agentPtr != NULL);
    Pinpoint::Plugin::InterceptorPtr interceptorPtr;

#if defined(DEBUG) || defined(TEST_SIMULATE)
    // 1, add api
    this->phpInfoApiId = agentPtr->addApi("phpinfo", -1, Pinpoint::API_DEFAULT);

    // 2, add interceptor
    try
    {
        interceptorPtr.reset(new PhpInfoInterceptor(shared_from_this()));
        this->interceptors.push_back(interceptorPtr);
    }
    catch (std::bad_alloc&)
    {
        LOGE("create PhpInfoInterceptor failed.");
        return FAILED;
    }
#endif

    if(PINPOINT_G(RedefineAgentMain)[0] != 0)
    {
        interceptorPtr.reset( new AgentMainRedefineInterceptor(shared_from_this(),PINPOINT_G(RedefineAgentMain)));
        this->interceptors.push_back( interceptorPtr);
    }


    return SUCCESS;
}


Pinpoint::Plugin::InterceptorPtrVector& PhpInternalFunctionsPlugin::getAllInterceptors()
{
    return this->interceptors;
}

int32_t PhpInternalFunctionsPlugin::getPhpInfoApiId()
{
    return this->phpInfoApiId;
}


const std::string PhpInfoInterceptor::PHP_INFO_FUNCTION_NAME = "phpinfo";

PhpInfoInterceptor::PhpInfoInterceptor(const PhpInternalFunctionsPluginPtr &pluginPtr)
        : Pinpoint::Plugin::Interceptor(PHP_INFO_FUNCTION_NAME), pluginPtr(pluginPtr)
{

}


void PhpInfoInterceptor::onBefore(uint64_t callId, Pinpoint::Plugin::FuncArgFetcher& argFetcher)
{
    if (!Trace::isStarted())
    {
        LOGW("why trace is started");
        return;
    }

    TracePtr tracePtr = Trace::getCurrentTrace();
    PINPOINT_ASSERT (tracePtr != NULL);

    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = tracePtr->traceBlockBegin(callId);
    if (spanEventRecorderPtr == NULL)
    {
        LOGE("traceBlockBegin failed");
        return;
    }

    spanEventRecorderPtr->markBeforeTime();
    spanEventRecorderPtr->setServiceType(Pinpoint::PHP_METHOD_CALL);
    spanEventRecorderPtr->setApiId(pluginPtr->getPhpInfoApiId());
}

void PhpInfoInterceptor::onEnd(uint64_t callId, Pinpoint::Plugin::FuncArgFetcher& argFetcher,
                               Pinpoint::Plugin::FuncResultFetcher& resultFetcher)
{
    if (!Trace::isStarted())
    {
        LOGW("trace is not started, why stop it");
        return;
    }

    TracePtr tracePtr = Trace::getCurrentTrace();
    PINPOINT_ASSERT (tracePtr != NULL);

    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = tracePtr->getSpanEventRecorderPtr(callId);
    if (spanEventRecorderPtr == NULL)
    {
        LOGE("getSpanEventRecorderPtr get NULL!");
        return;
    }

    spanEventRecorderPtr->markAfterTime();

    // for test
    AnnotationPtr annotationPtr =
            Annotation::createAnnotation(AnnotationKey::PHP_RETURN);
    if (annotationPtr != NULL)
    {
        annotationPtr->addIntValue(0);
        spanEventRecorderPtr->addAnnotationPtr(annotationPtr);
    }

    tracePtr->traceBlockEnd(spanEventRecorderPtr);
}


AgentMainRedefineInterceptor::AgentMainRedefineInterceptor(const PhpInternalFunctionsPluginPtr& _pluginPtr,
        std::string func):
                Pinpoint::Plugin::Interceptor(func),
                pluginPtr(_pluginPtr)
{
    LOGD("agent main had redefined by [%s]",func.c_str());
}

 void AgentMainRedefineInterceptor::onBefore(uint64_t callId, Pinpoint::Plugin::FuncArgFetcher& argFetcher)
 {
     using  Pinpoint::Agent::Agent;
     using  Pinpoint::Agent::AgentPtr;
     AgentPtr agentPtr = Agent::getAgentPtr();

     if (agentPtr->getAgentStatus() != Pinpoint::Agent::AGENT_INITED){
         PP_U_TRACE("The agentstatus said agent had initialized");
         LOGD("The agentstatus said agent had initialized");
         return ;
     }

     start_pinpoint_agent();
 }
