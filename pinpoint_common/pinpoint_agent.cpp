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
#include <cstdio>
#include <stdint.h>
#include <cassert>

#include "pinpoint_agent.h"
#include "pinpoint_error.h"
#include "pinpoint_agent_context.h"
#include "log_utility.h"
#include "api_data_sender.h"
#include "log.h"
#include "utility.h"
#include "pinpoint_client.h"
#include <boost/algorithm/string.hpp>



namespace Pinpoint
{

    namespace Agent
    {
        using namespace Pinpoint::log;


        TraceCount::TraceCount()
        {
            this->count = 0;
            this->startTime = boost::posix_time::second_clock::universal_time();
            LOGD("start=%s", boost::posix_time::to_simple_string(startTime).c_str());
        }

        bool TraceCount::beyondOneSecond(const boost::posix_time::ptime& start, const boost::posix_time::ptime& end)
        {
            LOGD("beyondOneSecond: start=%s", boost::posix_time::to_simple_string(start).c_str());
            LOGD("beyondOneSecond: end=%s", boost::posix_time::to_simple_string(end).c_str());
            int second = (end - start).total_seconds();
            LOGD("beyondOneSecond: second=%d", second);
            return second > 1;
        }

        void TraceCount::increment()
        {
            boost::posix_time::ptime time_now = boost::posix_time::second_clock::universal_time();

            if (beyondOneSecond(this->startTime, time_now))
            {
                /* start new count */
                this->startTime = time_now;
                this->count = 1;
            }
            else
            {
                this->count++;
            }
        }

        uint32_t TraceCount::getCount()
        {
            return this->count;
        }

        const char* Sampling::SAMPLING_RATE_PREFIX = "s";
        const char* Sampling::SAMPLING_RATE_FALSE = "s0";
        const char* Sampling::SAMPLING_RATE_TRUE = "s1";


        DefaultSampling::DefaultSampling(int32_t traceLimit, int32_t skipTraceTime)
        {
            this->traceLimit = traceLimit;
            this->skipTraceTime = skipTraceTime;
            this->enableSkipTraceTimeSampling = (skipTraceTime >= 0);
            this->enableTraceLimitSampling = (traceLimit >= 0);
            this->isContinuedTrace = false;
        }


        DefaultSampling::~DefaultSampling()
        {

        }

        void DefaultSampling::startNewTrace(bool isContinuedTrace)
        {
            this->isContinuedTrace = isContinuedTrace;
            if (this->traceCountTls.get() == NULL)
            {
                try
                {
                    this->traceCountTls.reset(new TraceCount);
                }
                catch (...)
                {
                    LOGE("create TraceCount failed.");
                    return;
                }
            }

            this->traceCountTls.get()->increment();
        }

        bool DefaultSampling::checkSkipTraceTime(int32_t elapse)
        {
            if (this->isContinuedTrace)
            {
                return true;
            }

            if(!this->enableSkipTraceTimeSampling)
            {
                return true;
            }
            return elapse >= this->skipTraceTime;
        }

        bool DefaultSampling::checkTraceLimit()
        {
            if (this->isContinuedTrace)
            {
                return true;
            }

            // call startNewTrace first
            if (this->traceCountTls.get() == NULL)
            {
                return false;
            }

            if(!this->enableTraceLimitSampling)
            {
                return true;
            }

            return this->traceCountTls.get()->getCount() <= (uint32_t)this->traceLimit;
        }


        //<editor-fold desc="PinpointAgent">

        PinpointAgent::PinpointAgent()
        {
            status = (volatile AgentStatus)AGENT_CREATED;
        }

        SamplingPtr & PinpointAgent::getSamplingPtr()
        {
            return this->samplingPtr;
        }

        PinpointAgent::~PinpointAgent()
        {
            stop();
        }

        int32_t PinpointAgent::sendTrace(const Trace::TracePtr &tracePtr)
        {
            if (this->traceDataSender != NULL)
            {
                return this->traceDataSender->send(tracePtr);
            }
            return FAILED;
        }

        int32_t PinpointAgent::preInit(AgentType agentType,
                                       const AgentFunction &agentFunction,
                                       Configuration::Config& conf)
        {
            int32_t err;

            this->agentFunction = agentFunction;

            try
            {
                args.reset(new AgentConfigArgs());
            }
            catch(std::bad_alloc& ba )
            {
                LOGI(" %s",ba.what());
                this->status = (volatile AgentStatus)AGENT_PRE_INIT_FAILURE;
                return FAILED;
            }

            args->assignArgs(&conf);

            err = PinpointAgentContext::initContext();
            if (err != SUCCESS)
            {
                LOGE("create PinpointAgentContext failed.");
                this->status = (volatile AgentStatus)AGENT_PRE_INIT_FAILURE;
                return FAILED;
            }

            PinpointAgentContextPtr& contextPtr = PinpointAgentContext::getContextPtr();
            PINPOINT_ASSERT_RETURN((contextPtr != NULL), FAILED);

            contextPtr->agentConfigArgsPtr = args;
            contextPtr->agentId = args->agentId;
            contextPtr->applicationName = args->applicationName;
            contextPtr->startTimestamp = utils::get_current_microsec_stamp64();
            contextPtr->serviceType = detectServiceType(agentType);

            contextPtr->agentVersion = PINPOINT_AGENT_VERSION;
            if (agentFunction.getHostProcessInfo != NULL)
            {
                contextPtr->ip = agentFunction.getHostProcessInfo(Pinpoint::Naming::SERVER_ADDR);
                contextPtr->ports = agentFunction.getHostProcessInfo(Pinpoint::Naming::SERVER_PORT);
                contextPtr->hostname = agentFunction.getHostProcessInfo(Pinpoint::Naming::HTTP_HOST);
            }

            /* ApiDataManager */
            err = ApiDataManager::initApiDataManager();
            if (err != SUCCESS)
            {
                LOGE("initApiDataManager failed.");
                this->status = (volatile AgentStatus)AGENT_PRE_INIT_FAILURE;
                return err;
            }

            err = StringDataManager::initStringDataManager();
            if (err != SUCCESS)
            {
                LOGE("initStringDataManager failed");
                this->status = (volatile AgentStatus)AGENT_PRE_INIT_FAILURE;
                return err;
            }

            this->status = (volatile AgentStatus)AGENT_PRE_INITED;
            return err;
        }

        int32_t PinpointAgent::init(Plugin::PluginPtrVector &pluginPtrVector)
        {
            if (this->status != AGENT_PRE_INITED)
            {
                return FAILED;
            }

            int32_t err = SUCCESS;

            PinpointAgentContextPtr contextPtr = PinpointAgentContext::getContextPtr();

            PINPOINT_ASSERT_RETURN((contextPtr != NULL), FAILED);

        	Pinpoint::Agent::OS_process_id_t mainPid;
        	Pinpoint::Agent::MainProcessChecker* checker = Pinpoint::Agent::MainProcessChecker::createChecker();
        	checker->setMaxFreeTime(60);
        	checker->isMainProcess(mainPid);

        	contextPtr->mainProcessPid = (uint32_t)mainPid;

            try
            {
                statUdpSender.reset(new UdpDataSender(TaskDispatcher::getInstance().getAsio(),"collectorStatSender",
                                                      args->collectorStatIp,
                                                      args->collectorStatPort));

                spanUdpSender.reset(new UdpDataSender(TaskDispatcher::getInstance().getAsio(),"collectorSpanSender",
                                                      args->collectorSpanIp,
                                                      args->collectorSpanPort));

                /* init scheduledExecutor */
                scheduledExecutor.reset(new ScheduledExecutor(TaskDispatcher::getInstance().getAsio(),"scheduledExecutor"));

                pinpointClientPtr.reset(new PinpointClient(TaskDispatcher::getInstance().getAsio(),"pinpoint-client",
                                                           args->collectorTcpIp,
                                                           args->collectorTcpPort,
                                                           scheduledExecutor,
                                                           args->reconInterval));

                boost::shared_ptr<DataSender> tcpDataSender =
                        boost::dynamic_pointer_cast<DataSender>(pinpointClientPtr);

                /* apiDataSender */
                apiDataSender.reset(new ApiDataSender(tcpDataSender));

                /* stringDataSender */
                stringDataSender.reset(new StringDataSender(tcpDataSender));

                /* agentDataSender */
                agentDataSender.reset(new AgentDataSender(scheduledExecutor, tcpDataSender));

                /* agentStatSender */
                agentMonitorSender.reset(new AgentMonitorSender(scheduledExecutor, statUdpSender));


                /* traceDataSender */
                traceDataSender.reset(new TraceDataSender(spanUdpSender));

                /* sampling */
                samplingPtr.reset(new DefaultSampling(args->traceLimit, args->skipTraceTime));
            }
            catch (std::bad_alloc&)
            {
                this->status = (volatile AgentStatus)AGENT_INIT_FAILURE;
                return FAILED;
            }


            err = agentDataSender->init();
            if (err != SUCCESS)
            {
                this->status = (volatile AgentStatus)AGENT_INIT_FAILURE;
                return err;
            }

            err = agentMonitorSender->init();
            if (err != SUCCESS)
            {
                this->status = (volatile AgentStatus)AGENT_INIT_FAILURE;
                return err;
            }

            err = traceDataSender->init();
            if (err != SUCCESS)
            {
                this->status = (volatile AgentStatus)AGENT_INIT_FAILURE;
                return err;
            }

            err = apiDataSender->init();
            if (err != SUCCESS)
            {
                this->status = (volatile AgentStatus)AGENT_INIT_FAILURE;
                return err;
            }

            err = stringDataSender->init();
            if (err != SUCCESS)
            {
                this->status = (volatile AgentStatus)AGENT_INIT_FAILURE;
                return err;
            }

            err = addPredefinedType();
            if (err != SUCCESS)
            {
                this->status = (volatile AgentStatus)AGENT_INIT_FAILURE;
                return err;
            }

            /* register plugins */
            err = registerPlugins(pluginPtrVector);
            if (err != SUCCESS)
            {
                this->status = (volatile AgentStatus)AGENT_INIT_FAILURE;
                return err;
            }

            pinpointClientPtr->init();

            this->status = (volatile AgentStatus)AGENT_INITED;
            return err;

        }


        int32_t PinpointAgent::start()
        {
            if (this->status != AGENT_INITED)
            {
                return FAILED;
            }

            try
            {

//#if 0
                agentDataSender->start();
                agentMonitorSender->start();
                traceDataSender->start();
                apiDataSender->start();
                stringDataSender->start();

//                boost::dynamic_pointer_cast<Executor>(pinpointClientPtr)->start();
//                boost::dynamic_pointer_cast<Executor>(statUdpSender)->start();
//                boost::dynamic_pointer_cast<Executor>(spanUdpSender)->start();

//                scheduledExecutor->start();
//#endif

                TaskDispatcher::getInstance().start();

                this->status = (volatile AgentStatus)AGENT_STARTED;

                return SUCCESS;
            }
            catch (std::exception& exception)
            {
                LOGE("PinpointAgent::start() throw: e=%s", exception.what());
                this->status = (volatile AgentStatus)AGENT_START_FAILURE;
                return FAILED;
            }

        }

        int32_t PinpointAgent::stop()
        {
            if (this->status != AGENT_STARTED)
            {
                status = (volatile AgentStatus)AGENT_STOPPED;
                return SUCCESS;
            }

            int32_t err = SUCCESS;

//            boost::dynamic_pointer_cast<Executor>(pinpointClientPtr)->stop();
//            boost::dynamic_pointer_cast<Executor>(statUdpSender)->stop();
//            boost::dynamic_pointer_cast<Executor>(spanUdpSender)->stop();
            // todo
//            scheduledExecutor->stop();

            status = (volatile AgentStatus)AGENT_STOPPED;

            return err;
        }

        int32_t PinpointAgent::addApi(const char *api_info, int32_t line, ApiType type)
        {
            ApiDataManagerPtr& manager = ApiDataManager::getInstance();
            PINPOINT_ASSERT_RETURN((manager != NULL), INVALID_API_ID);
            if (status >= AGENT_STARTED)
            {
                LOGE("You should call this function before agent init.")
                return INVALID_API_ID;
            }

            return manager->addApi(std::string(api_info), line, type);
        }

        int32_t PinpointAgent::addString(const std::string &stringValue)
        {
            StringDataManagerPtr& manager = StringDataManager::getInstance();
            PINPOINT_ASSERT_RETURN((manager != NULL), INVALID_STRING_ID);

            if (status >= AGENT_CREATED)
            {
                LOGE("You should call this function before agent init.")
                return INVALID_STRING_ID;
            }

            return manager->addString(stringValue);
        }

        volatile AgentStatus PinpointAgent::getAgentStatus() const
        {
            return this->status;
        }

        int16_t PinpointAgent::detectServiceType(AgentType agentType)
        {
            switch (agentType)
            {
                case PHP_AGENT_TYPE:
                    return PHP;
                case HTTPD_AGENT_TYPE:
                    return HTTPD_SERVICE;
                default:
                    PINPOINT_ASSERT_RETURN(false, UNKNOWN_SERVICE);
            }

        }

        int32_t PinpointAgent::registerPlugins(Plugin::PluginPtrVector &pluginPtrVector)
        {
            int32_t err = SUCCESS;
            LOGI("PinpointAgent::registerPlugins len(plugins)=%d", pluginPtrVector.size());
            LOGI("PinpointAgent::registerPlugins include=%s", this->args->pluginInclude.c_str());
            LOGI("PinpointAgent::registerPlugins exclude=%s", this->args->pluginExclude.c_str());

            std::vector<std::string> includes;
            if (this->args->pluginIncludeIsSet)
            {
                utils::split_string(this->args->pluginInclude, includes, ";");
                std::for_each(includes.begin(), includes.end(),
                              boost::bind(&boost::trim<std::string>, _1, std::locale()));
            }


            std::vector<std::string> excludes;
            if (this->args->pluginExcludeIsSet)
            {
                utils::split_string(this->args->pluginExclude, excludes, ";");
                std::for_each(excludes.begin(), excludes.end(),
                              boost::bind(&boost::trim<std::string>, _1, std::locale()));
            }

            std::set<std::string> includeSet(includes.begin(), includes.end());
            std::set<std::string> excludeSet(excludes.begin(), excludes.end());

            Plugin::PluginPtrVector::iterator ip;
            for (ip = pluginPtrVector.begin(); ip != pluginPtrVector.end(); ++ip)
            {
                Plugin::PluginPtr& pluginPtr = (*ip);

                LOGT("plugin=%s", pluginPtr->getName().c_str());
                if (this->args->pluginIncludeIsSet)
                {
                    if (includeSet.find(pluginPtr->getName()) == includeSet.end())
                    {
                        LOGI("plugin=%s not in includes, ignore!", pluginPtr->getName().c_str());
                        continue;
                    }
                }

                if (this->args->pluginExcludeIsSet)
                {
                    if (excludeSet.find(pluginPtr->getName()) != excludeSet.end())
                    {
                        LOGI("plugin=%s in excludes, ignore!", pluginPtr->getName().c_str());
                        continue;
                    }
                }

                err = pluginPtr->init();
                if (err != SUCCESS)
                {
                    LOGE("plugin=%s init failed! err=%d", err);
                    continue;
                }

                Plugin::InterceptorPtrVector& interceptorPtrVector = pluginPtr->getAllInterceptors();
                Plugin::InterceptorPtrVector::iterator iip;
                for (iip = interceptorPtrVector.begin(); iip != interceptorPtrVector.end(); ++iip)
                {
                    Plugin::InterceptorPtr& interceptorPtr = (*iip);

//                    LOGI("prepare to interceptor %s init", interceptorPtr->getInterceptedFuncName().c_str());
                    err = interceptorPtr->init();
                    if (err != SUCCESS)
                    {
                        LOGE("interceptor %s init failed.", interceptorPtr->getInterceptedFuncName().c_str());
                        continue;
                    }

                    if (!this->agentFunction.addInterceptorFunc.empty())
                    {
                        err = this->agentFunction.addInterceptorFunc(interceptorPtr);
                        if (err != SUCCESS)
                        {
                            LOGE("add %s interceptor failed. err=%d",
                                 interceptorPtr->getInterceptedFuncName().c_str(), err);
                            continue;
                        }
                    }
                }
            }

            return SUCCESS;
        }

        int32_t PinpointAgent::addPredefinedType()
        {
            StringDataManagerPtr& manager = StringDataManager::getInstance();
            PINPOINT_ASSERT_RETURN((manager != NULL), FAILED);

            Plugin::EXCEPTION_STRING_ID = manager->addString(Plugin::EXCEPTION_STRING);
            Plugin::ERROR_STRING_ID = manager->addString(Plugin::ERROR_STRING);
            Plugin::WARNINGS_STRING_ID = manager->addString(Plugin::WARNINGS_STRING);
            
            return SUCCESS;
        }

        //</editor-fold>

        //<editor-fold desc="Agent">

        AgentPtr Agent::agentPtr;

        int32_t Agent::createAgent()
        {
            try
            {
                Agent::agentPtr.reset(new PinpointAgent());
            }
            catch (std::bad_alloc &)
            {
                LOGE("create pinpointAgent failed");
                return FAILED;
            }
            return SUCCESS;
        }

        AgentPtr &Agent::getAgentPtr()
        {
            return Agent::agentPtr;
        }
        //</editor-fold>

    }
}
