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
#ifndef PINPOINT_INSPECTOR_H
#define PINPOINT_INSPECTOR_H

#include "pinpoint_api.h"
#include <boost/thread.hpp>


#define DEBUG_MODULE 0 //DEBUG_MODULE

typedef std::pair<Pinpoint::Plugin::InterceptorPtr, uint64_t> CurrentInterceptorInfo;

class InterceptorManager
{
public:
    virtual const Pinpoint::Plugin::InterceptorPtr& find(const std::string& funcName) = 0;
    virtual int32_t add(const Pinpoint::Plugin::InterceptorPtr& interceptorPtr) = 0;
    virtual int32_t del(const std::string& funcName) = 0;

    void setRequestInterceptorPtr(const Pinpoint::Plugin::InterceptorPtr& interceptorPtr);
    Pinpoint::Plugin::InterceptorPtr& getRequestInterceptorPtr();

    int32_t resetCurrentInterceptor(const Pinpoint::Plugin::InterceptorPtr& interceptorPtr, uint64_t callId);
    int32_t resetCurrentInterceptor();
    CurrentInterceptorInfo getCurrentInterceptorInfo();

    const static Pinpoint::Plugin::InterceptorPtr NULL_INTERCEPTOR;
    virtual ~InterceptorManager(){}

protected:
    Pinpoint::Plugin::InterceptorPtr requestInterceptorPtr;
    static boost::thread_specific_ptr<CurrentInterceptorInfo> currentInterceptorInfo;

};

class InterceptorManagerBasedMap : public InterceptorManager
{
public:
    virtual const Pinpoint::Plugin::InterceptorPtr& find(const std::string& funcName);
    virtual int32_t add(const Pinpoint::Plugin::InterceptorPtr& interceptorPtr);
    virtual int32_t del(const std::string& funcName);
    virtual ~InterceptorManagerBasedMap(){}
private:
    typedef std::map<std::string, Pinpoint::Plugin::InterceptorPtr> InterceptorMap;
    InterceptorMap interceptors;
};

typedef boost::shared_ptr<InterceptorManager> InterceptorManagerPtr;

typedef boost::function<void(void)> CallCB;

class PhpAop
{
public:
    static int32_t createInstance(const InterceptorManagerPtr& interceptorManagerPtr);
    static void freeAop();
    static PhpAop* getInstance();
    int32_t addInterceptorPtr(const Pinpoint::Plugin::InterceptorPtr& interceptorPtr);
    const Pinpoint::Plugin::InterceptorPtr& getInterceptorPtr(const std::string& funcName);
    const Pinpoint::Plugin::InterceptorPtr& getInterceptorPtr(const char* funcName);
    int32_t resetCurrentInterceptor(const Pinpoint::Plugin::InterceptorPtr& interceptorPtr, uint64_t callId);
    int32_t resetCurrentInterceptor();
    CurrentInterceptorInfo getCurrentInterceptorInfo();
    void setRequestInterceptorPtr(const Pinpoint::Plugin::InterceptorPtr& interceptorPtr);
    Pinpoint::Plugin::InterceptorPtr& getRequestInterceptorPtr();
    void addReqShutdownHandler(CallCB cb);
    void reqShutdownClean();

private:
    PhpAop(const InterceptorManagerPtr& interceptorManagerPtr);
    InterceptorManagerPtr interceptorManagerPtr;
    std::list<CallCB> reqDoneCleanList;
    static PhpAop* instance;
};

class SimpleInterceptor : public Pinpoint::Plugin::Interceptor
{
public:
    explicit SimpleInterceptor(const std::string& interceptedFuncName, int64_t line);
    virtual int32_t init();
protected:
    virtual void onBefore(uint64_t callId, Pinpoint::Plugin::FuncArgFetcher& argFetcher);
    virtual void onEnd(uint64_t callId, Pinpoint::Plugin::FuncArgFetcher& argFetcher, Pinpoint::Plugin::FuncResultFetcher& resultFetcher);
private:
    int64_t line;
    int32_t apiId;
};

#endif
