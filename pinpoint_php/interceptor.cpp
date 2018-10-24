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
#include <assert.h>

#include "aop_hook.h"
#include "php_common.h"
#include "interceptor.h"

using namespace Pinpoint::log;
using namespace Pinpoint::Plugin;



//<editor-fold desc="PhpAop">

PhpAop* PhpAop::instance;

int32_t PhpAop::createInstance(const InterceptorManagerPtr &interceptorManagerPtr)
{
    PINPOINT_ASSERT_RETURN ((interceptorManagerPtr != NULL), Pinpoint::FAILED);

    try
    {
        instance = new PhpAop(interceptorManagerPtr);
    }
    catch (std::bad_alloc&)
    {
        return Pinpoint::OUT_OF_MEMORY;
    }

    return SUCCESS;
}

void PhpAop::freeAop()
{
    if (instance != NULL)
    {
        delete instance;
        instance = NULL;
    }
}

PhpAop* PhpAop::getInstance()
{
    return instance;
}

PhpAop::PhpAop(const InterceptorManagerPtr &interceptorManagerPtr)
        : interceptorManagerPtr(interceptorManagerPtr)
{

}

int32_t PhpAop::addInterceptorPtr(const Pinpoint::Plugin::InterceptorPtr &interceptorPtr)
{
    assert (interceptorPtr != NULL);

    if (this->interceptorManagerPtr == NULL)
    {
        return Pinpoint::FAILED;
    }

    LOGD("add interceptor, name=%s", interceptorPtr->getInterceptedFuncName().c_str());

    return this->interceptorManagerPtr->add(interceptorPtr);
}

const InterceptorPtr& PhpAop::getInterceptorPtr(const std::string &funcName)
{
    return this->getInterceptorPtr(funcName.c_str());
}

const InterceptorPtr& PhpAop::getInterceptorPtr(const char* funcName)
{
    if (funcName == NULL)
    {
        return InterceptorManager::NULL_INTERCEPTOR;
    }

    return this->interceptorManagerPtr->find(funcName);
}

CurrentInterceptorInfo PhpAop::getCurrentInterceptorInfo()
{
    return interceptorManagerPtr->getCurrentInterceptorInfo();
}

int32_t PhpAop::resetCurrentInterceptor(const Pinpoint::Plugin::InterceptorPtr &interceptorPtr, uint64_t callId)
{
    return interceptorManagerPtr->resetCurrentInterceptor(interceptorPtr, callId);
}

int32_t PhpAop::resetCurrentInterceptor()
{
    return interceptorManagerPtr->resetCurrentInterceptor();
}

void PhpAop::setRequestInterceptorPtr(const Pinpoint::Plugin::InterceptorPtr &interceptorPtr)
{
    this->interceptorManagerPtr->setRequestInterceptorPtr(interceptorPtr);
}

Pinpoint::Plugin::InterceptorPtr& PhpAop::getRequestInterceptorPtr()
{
    return this->interceptorManagerPtr->getRequestInterceptorPtr();
}

void PhpAop::addReqShutdownHandler(CallCB cb)
{
    reqDoneCleanList.push_back(cb);
}

void PhpAop::reqShutdownClean()
{
    std::list<CallCB>::iterator it = reqDoneCleanList.begin();
    for(; it != reqDoneCleanList.end(); ++it)
    {
        (*it)();
    }

    reqDoneCleanList.clear();
    LOGD("reqShutdownClean called");
}

//</editor-fold>


//<editor-fold desc="InterceptorManager">

const Pinpoint::Plugin::InterceptorPtr InterceptorManager::NULL_INTERCEPTOR;
boost::thread_specific_ptr<CurrentInterceptorInfo> InterceptorManager::currentInterceptorInfo;

void InterceptorManager::setRequestInterceptorPtr(const Pinpoint::Plugin::InterceptorPtr &interceptorPtr)
{
    this->requestInterceptorPtr = interceptorPtr;
}

Pinpoint::Plugin::InterceptorPtr& InterceptorManager::getRequestInterceptorPtr()
{
    return this->requestInterceptorPtr;
}

int32_t InterceptorManager::resetCurrentInterceptor()
{
    if (currentInterceptorInfo.get() != NULL)
    {
        currentInterceptorInfo.get()->first.reset();
        currentInterceptorInfo.get()->second = INVALID_CALL_ID;
    }
    return SUCCESS;
}

int32_t InterceptorManager::resetCurrentInterceptor(const Pinpoint::Plugin::InterceptorPtr &interceptorPtr,
                                                    uint64_t callId)
{
    PINPOINT_ASSERT_RETURN((callId != INVALID_CALL_ID), Pinpoint::FAILED);
    PINPOINT_ASSERT_RETURN((interceptorPtr != NULL), Pinpoint::FAILED);

    try
    {
        if (currentInterceptorInfo.get() == NULL)
        {
            currentInterceptorInfo.reset(new CurrentInterceptorInfo);
        }
    }
    catch (std::bad_alloc&)
    {
        LOGE("new currentInterceptorPtr failed");
        return Pinpoint::FAILED;
    }

    currentInterceptorInfo.get()->first = interceptorPtr;
    currentInterceptorInfo.get()->second = callId;
    return SUCCESS;
}

CurrentInterceptorInfo InterceptorManager::getCurrentInterceptorInfo()
{
    if(currentInterceptorInfo.get() != NULL)
    {
        return *currentInterceptorInfo.get();
    }

    return CurrentInterceptorInfo(NULL_INTERCEPTOR, INVALID_CALL_ID);
}

//</editor-fold>

//<editor-fold desc="InterceptorManagerBasedMap">

const InterceptorPtr& InterceptorManagerBasedMap::find(const std::string &funcName)
{
    InterceptorMap::iterator ip = interceptors.find(funcName);
    if (ip == interceptors.end())
    {
        return NULL_INTERCEPTOR;
    }

    return ip->second;
}

int32_t InterceptorManagerBasedMap::add(const InterceptorPtr& interceptorPtr)
{
    assert (interceptorPtr != NULL);

    /// if name is exist, refuse to add it
    if(interceptors.find(interceptorPtr->getInterceptedFuncName()) == interceptors.end())
    {
        interceptors[interceptorPtr->getInterceptedFuncName()] = interceptorPtr;
    }else{
        LOGW("InterceptorManagerBasedMap: refuse update [%s]",interceptorPtr->getInterceptedFuncName().c_str());
        PP_U_TRACE("InterceptorManagerBasedMap: refuse update [%s]",interceptorPtr->getInterceptedFuncName().c_str());
    }

    return SUCCESS;
}

int32_t InterceptorManagerBasedMap::del(const std::string &funcName)
{
    interceptors.erase(funcName);
    return SUCCESS;
}


//</editor-fold>


//<editor-fold desc="SimpleInterceptor">
SimpleInterceptor::SimpleInterceptor(const std::string &interceptedFuncName, int64_t line)
        : Pinpoint::Plugin::Interceptor(interceptedFuncName), line(line),
          apiId(Pinpoint::Agent::INVALID_API_ID)
{

}

int32_t SimpleInterceptor::init()
{
    Pinpoint::Agent::AgentPtr& agentPtr = Pinpoint::Agent::Agent::getAgentPtr();
    PINPOINT_ASSERT_RETURN ((agentPtr != NULL), Pinpoint::FAILED);

    // 1, add api
    this->apiId = agentPtr->addApi(this->getInterceptedFuncName().c_str(),
                                   this->line,
                                   Pinpoint::API_DEFAULT);
    return SUCCESS;
}

void SimpleInterceptor::onBefore(uint64_t callId, Pinpoint::Plugin::FuncArgFetcher &argFetcher)
{
    if (!Pinpoint::Trace::Trace::isStarted())
    {
        return;
    }

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    PINPOINT_ASSERT (tracePtr != NULL);

    Pinpoint::Trace::SpanEventRecorderPtr  spanEventRecorderPtr = tracePtr->traceBlockBegin(callId);
    if (spanEventRecorderPtr == NULL)
    {
        LOGE("traceBlockBegin failed");
    }

    spanEventRecorderPtr->markBeforeTime();
    spanEventRecorderPtr->setServiceType(Pinpoint::PHP_METHOD_CALL);
    spanEventRecorderPtr->setApiId(this->apiId);



    try
    {
        AbstractPHPFuncArgFetcher &phpFuncArgFetcher = dynamic_cast<AbstractPHPFuncArgFetcher&>(argFetcher);

        std::string value;
        KV map;
        zval_to_Map(map,phpFuncArgFetcher.getArgs(),-1,MAX_ANNOTATION_SIZE);
        map_to_str(map,value);

        Pinpoint::Trace::AnnotationPtr annotationPtr =
              Pinpoint::Trace::Annotation::createAnnotation(Pinpoint::Plugin::INDEX_ARGS[0]);
        if (annotationPtr == NULL)
        {
          LOGE("createAnnotation failed!!!");
          return;
        }

        annotationPtr->addStringValue(value);

        spanEventRecorderPtr->addAnnotationPtr(annotationPtr);
        PP_U_TRACE("SimpleInterceptor->addAnnotation key:[%d] value:[%s] ",Pinpoint::Plugin::INDEX_ARGS[0],value.c_str());

#if 0
        VecStr vec;
        zval_args_to_vec(vec,phpFuncArgFetcher.getArgs(),-1,MAX_ANNOTATION_SIZE);


        int i=0;
        for(; i<vec.size() && i< sizeof(INDEX_ARGS) / sizeof(INDEX_ARGS[0]); i++)
        {
            Pinpoint::Trace::AnnotationPtr annotationPtr =
                    Pinpoint::Trace::Annotation::createAnnotation(Pinpoint::Plugin::INDEX_ARGS[i]);
            if (annotationPtr == NULL)
            {
                LOGE("createAnnotation failed!!!");
                return;
            }

            LOGI("%d %s ",Pinpoint::Plugin::INDEX_ARGS[i],vec[i].c_str());

            annotationPtr->addStringValue(vec[i]);

            spanEventRecorderPtr->addAnnotationPtr(annotationPtr);
        }

        if(i<vec.size())
        {

        }


        std::string value;
        vec_to_str(vec.begin(),vec.end(),value);

        Pinpoint::Trace::AnnotationPtr annotationPtr =
              Pinpoint::Trace::Annotation::createAnnotation(Pinpoint::Plugin::INDEX_ARGS[0]);
        if (annotationPtr == NULL)
        {
          LOGE("createAnnotation failed!!!");
          return;
        }

        annotationPtr->addStringValue(value);

        spanEventRecorderPtr->addAnnotationPtr(annotationPtr);

#endif
    }
    catch (std::exception& e)
    {
        LOGE("get args throw: e=%s", e.what());
    }
}

void SimpleInterceptor::onEnd(uint64_t callId,
                              Pinpoint::Plugin::FuncArgFetcher &argFetcher,
                              Pinpoint::Plugin::FuncResultFetcher &resultFetcher)
{
    if (!Pinpoint::Trace::Trace::isStarted())
    {
        return;
    }

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    PINPOINT_ASSERT (tracePtr != NULL);

    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = tracePtr->getSpanEventRecorderPtr(callId);
    if (spanEventRecorderPtr == NULL)
    {
        return;
    }

    spanEventRecorderPtr->markAfterTime();

    try
    {

        AbstractPHPResultFetcher &phpFuncResultFetcher = dynamic_cast<AbstractPHPResultFetcher &>(resultFetcher);

        // NULL in php 5.3 when exceptoin catched
        // if  EG(exception) found ignore the return value, event it is NULL
        if (phpFuncResultFetcher.getResult() != NULL && EG(exception) ==NULL)
        {
            Pinpoint::Trace::AnnotationPtr annotationPtr =
                    Pinpoint::Trace::Annotation::createAnnotation(
                            Pinpoint::Trace::AnnotationKey::RETURN_DATA);
            if (annotationPtr == NULL)
            {
                LOGE("createAnnotation failed!!!");
                goto error_and_exit;
            }

            std::string value = zval_to_string(phpFuncResultFetcher.getResult(),MAX_ANNOTATION_SIZE);
            annotationPtr->addStringValue(value);
            spanEventRecorderPtr->addAnnotationPtr(annotationPtr);

            PP_U_TRACE("SimpleInterceptor->addAnnotation key:[%d] value:[%s] ",Pinpoint::Trace::AnnotationKey::RETURN_DATA,value.c_str());
        }
    }
    catch (std::exception& e)
    {
        LOGE("get args throw: e=%s", e.what());
    }
error_and_exit:
    tracePtr->traceBlockEnd(spanEventRecorderPtr);
}
//</editor-fold>



