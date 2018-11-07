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
#ifndef PINPOINT_COMMON_AGENT_H
#define PINPOINT_COMMON_AGENT_H

#define __STDC_LIMIT_MACROS 
#include <stdint.h>
#include "executor.h"
#include "data_sender.h"
#include "agent_data_sender.h"
#include "agent_monitor_sender.h"
#include "hand_shake_sender.h"
#include "trace_data_sender.h"
#include "api_data_sender.h"
#include "pinpoint_api.h"
#include "string_data_sender.h"
#include "pinpoint_client.h"

#include <boost/property_tree/ptree.hpp>


namespace Pinpoint
{
    namespace Agent
    {
        class TraceCount
        {
        public:
            TraceCount();
            void increment();
            uint32_t getCount();
        private:
            static bool beyondOneSecond(const boost::posix_time::ptime& start, const boost::posix_time::ptime& end);
            boost::posix_time::ptime startTime;
            uint32_t count;
        };

        class DefaultSampling : public Sampling
        {
        public:
            DefaultSampling(int32_t traceLimit, int32_t skipTraceTime);
            bool checkTraceLimit();
            bool checkSkipTraceTime(int32_t elapse);
            void startNewTrace(bool isContinuedTrace);
            ~DefaultSampling();
        private:
            int32_t traceLimit;
            bool enableTraceLimitSampling;
            int32_t skipTraceTime; /* ms */
            bool enableSkipTraceTimeSampling;
            boost::thread_specific_ptr<TraceCount> traceCountTls;
            bool isContinuedTrace;
        };

        class PinpointAgent : public Agent
        {
        public:
            friend class Agent;
            int32_t preInit(AgentType agentType,
                            const AgentFunction& agentFunction,
                            struct AgentConfigArgs& conf);
            int32_t init(Plugin::PluginPtrVector& pluginPtrVector);

            int32_t start();
            int32_t stop();
            int32_t addApi(const char* api_info, int32_t line, ApiType type);
            int32_t addString(const std::string& stringValue);

            int32_t updatePlugins(Plugin::PluginPtrVector& pluginPtrVector);

            void     asyStopAllTask();
            volatile AgentStatus getAgentStatus() const;

            virtual SamplingPtr& getSamplingPtr();
            virtual int32_t sendTrace(const Trace::TracePtr &tracePtr);

            virtual ~PinpointAgent();

        private:
            PinpointAgent();
            static int16_t detectServiceType(AgentType agentType);
            static int32_t addPredefinedType();
            AgentFunction agentFunction;
            boost::shared_ptr<ScheduledExecutor> scheduledExecutor;
            boost::shared_ptr<DataSender> statUdpSender;
            boost::shared_ptr<DataSender> spanUdpSender;

            PinpointClientPtr pinpointClientPtr;
            boost::shared_ptr<AgentDataSender> agentDataSender;
            boost::shared_ptr<AgentMonitorSender> agentMonitorSender;
            boost::shared_ptr<TraceDataSender> traceDataSender;
            boost::shared_ptr<ApiDataSender> apiDataSender;
            boost::shared_ptr<StringDataSender> stringDataSender;
            SamplingPtr samplingPtr;
            volatile AgentStatus status;
            AgentConfigArgsPtr args;
        };

        typedef boost::shared_ptr<PinpointAgent> PinpointAgentPtr;
    }
}

#endif
