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
#ifndef PINPOINT_AGENT_DATA_SENDER_H
#define PINPOINT_AGENT_DATA_SENDER_H

#include "data_sender.h"
#include "thrift/gen-cpp/Pinpoint_types.h"
#include "serializer.h"
#include <boost/enable_shared_from_this.hpp>

namespace Pinpoint
{
    namespace Agent
    {

        class AgentDataSender;

        class AgentDataTask : public ExecutorTask
        {
        public:
            static const uint32_t DEFAULT_AGENT_INFO_REFRESH_INTERVAL_MS = 24 * 60 * 60 * 1000;

            AgentDataTask(const std::string &taskName, boost::shared_ptr<AgentDataSender> &sender);

            ~AgentDataTask();

            virtual int32_t run();

        private:
            boost::shared_ptr<AgentDataSender> sender;
        };

        class AgentDataSender : public boost::enable_shared_from_this<AgentDataSender>
        {
        public:
            friend class AgentDataTask;

            AgentDataSender(boost::shared_ptr<ScheduledExecutor> &scheduledExecutor,
                            boost::shared_ptr<DataSender> &dataSender);

            ~AgentDataSender();

            int32_t init();

            int32_t start();

        private:
            boost::shared_ptr<ScheduledExecutor> scheduledExecutor;
            boost::shared_ptr<DataSender> dataSender;

            TBasePtr createTAgentInfo();

            int32_t send();
        };

    }
}

#endif
