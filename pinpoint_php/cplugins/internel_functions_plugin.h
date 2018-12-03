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
#ifndef INTERNAL_FUNCTIONS_PLUGIN_H
#define INTERNAL_FUNCTIONS_PLUGIN_H

#include "pinpoint_api.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

class PhpInternalFunctionsPlugin : public Pinpoint::Plugin::Plugin, public boost::enable_shared_from_this<PhpInternalFunctionsPlugin>
{
public:
    PhpInternalFunctionsPlugin();
    virtual int32_t init();
    virtual Pinpoint::Plugin::InterceptorPtrVector& getAllInterceptors();
    int32_t getPhpInfoApiId();
private:
    Pinpoint::Plugin::InterceptorPtrVector interceptors;
    int32_t phpInfoApiId;
};

typedef boost::shared_ptr<PhpInternalFunctionsPlugin> PhpInternalFunctionsPluginPtr;

class PhpInfoInterceptor : public Pinpoint::Plugin::Interceptor
{
public:
    static const std::string PHP_INFO_FUNCTION_NAME;
    PhpInfoInterceptor(const PhpInternalFunctionsPluginPtr& pluginPtr);

protected:
    virtual void onBefore(uint64_t callId, Pinpoint::Plugin::FuncArgFetcher& argFetcher);
    virtual void onEnd(uint64_t callId,
                       Pinpoint::Plugin::FuncArgFetcher& argFetcher,
                       Pinpoint::Plugin::FuncResultFetcher& resultFetcher);
private:
    PhpInternalFunctionsPluginPtr pluginPtr;
};

#if 0
class AgentMainRedefineInterceptor : public Pinpoint::Plugin::Interceptor
{
public:
    AgentMainRedefineInterceptor(const PhpInternalFunctionsPluginPtr& _pluginPtr,std::string func);

protected:
    virtual void onBefore(uint64_t callId, Pinpoint::Plugin::FuncArgFetcher& argFetcher);
    virtual void onEnd(uint64_t callId,
                       Pinpoint::Plugin::FuncArgFetcher& argFetcher,
                       Pinpoint::Plugin::FuncResultFetcher& resultFetcher){
        /// do nothing
    }
private:
    PhpInternalFunctionsPluginPtr pluginPtr;
};
#endif


#endif

