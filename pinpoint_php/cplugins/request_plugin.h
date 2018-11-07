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
#ifndef REQUEST_PLUGIN_H
#define REQUEST_PLUGIN_H

#include "pinpoint_api.h"
#include <boost/enable_shared_from_this.hpp>

class PhpRequestPlugin : public Pinpoint::Plugin::Plugin, public boost::enable_shared_from_this<PhpRequestPlugin>
{
public:
    PhpRequestPlugin();
    virtual int32_t init();
    virtual Pinpoint::Plugin::InterceptorPtrVector& getAllInterceptors();
    int32_t getRequestApi();
private:
    Pinpoint::Plugin::InterceptorPtrVector interceptors;
    int32_t requestApiId;
};

typedef boost::shared_ptr<PhpRequestPlugin> PhpRequestPluginPtr;


enum TraceState{
    E_STARTED = 0,
    E_STOP,
    E_IGNORE
};

class PhpRequestInterceptor : public Pinpoint::Plugin::Interceptor
{
public:
    PhpRequestInterceptor(const std::string& interceptedFuncName,
                          const PhpRequestPluginPtr& pluginPtr);
    virtual int32_t init();

protected:
    virtual void onBefore(uint64_t callId, Pinpoint::Plugin::FuncArgFetcher& argFetcher);
    virtual void onEnd(uint64_t callId,
                       Pinpoint::Plugin::FuncArgFetcher& argFetcher,
                       Pinpoint::Plugin::FuncResultFetcher& resultFetcher);

private:
    int32_t startTrace();
    int32_t endTrace();
    TraceState state;
    PhpRequestPluginPtr pluginPtr;
};

#endif
