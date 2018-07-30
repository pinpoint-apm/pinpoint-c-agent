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
#ifndef PINPOINT_AGENT_MONITOR_SENDER_H
#define PINPOINT_AGENT_MONITOR_SENDER_H


#include "thrift/gen-cpp/Pinpoint_types.h"
#include "executor.h"
#include "data_sender.h"
#include "serializer.h"

namespace Pinpoint
{
    namespace Agent
    {

        class AgentMonitorSender;

        class AgentMonitorBatchTask : public ExecutorTask
        {
        public:
            static const uint16_t BATCH_NUM = 6;

            AgentMonitorBatchTask(const std::string &taskName, boost::shared_ptr<AgentMonitorSender> &sender,
                                  uint32_t batchNum);

            ~AgentMonitorBatchTask();

            virtual int32_t run();

        private:
            boost::shared_ptr<AgentMonitorSender> sender;
            uint32_t batchNum;
            uint32_t count;
            std::vector<TAgentStat> stats;
        };

        class AgentMonitorSender : public boost::enable_shared_from_this<AgentMonitorSender>
        {
        public:
            friend class AgentMonitorBatchTask;

            AgentMonitorSender(boost::shared_ptr<ScheduledExecutor> &scheduledExecutor,
                               boost::shared_ptr<DataSender> &dataSender);

            ~AgentMonitorSender();

            int32_t init();

            int32_t start();

        private:
            boost::shared_ptr<ScheduledExecutor> scheduledExecutor;
            boost::shared_ptr<DataSender> dataSender;

            HeaderTBaseSerializer serializer;

            TAgentStat collectAgentStat();

            int32_t send(const std::vector<TAgentStat> &stats);
        };

    }
}

#endif
