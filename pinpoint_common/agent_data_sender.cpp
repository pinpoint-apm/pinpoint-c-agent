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
#include "agent_data_sender.h"
#include "pinpoint_agent_context.h"
#include "buffer.h"
#include "data_sender.h"

using namespace Pinpoint::utils;

namespace Pinpoint
{
    namespace Agent
    {

        AgentDataTask::AgentDataTask(const std::string &taskName, boost::shared_ptr<AgentDataSender> &sender)
                : ExecutorTask(taskName), sender(sender)
        {
            PINPOINT_ASSERT(sender != NULL);
        }

        AgentDataTask::~AgentDataTask()
        {

        }

        int32_t AgentDataTask::run()
        {
            PINPOINT_ASSERT_RETURN((sender != NULL), FAILED);
            PINPOINT_ASSERT_RETURN((sender->scheduledExecutor != NULL), FAILED);

            int32_t err = sender->send();

            try
            {
                boost::shared_ptr<ExecutorTask> task(new AgentDataTask("agentDataTask", sender));

                err = sender->scheduledExecutor->addTask(task, DEFAULT_AGENT_INFO_REFRESH_INTERVAL_MS, 1);
                if (err != SUCCESS)
                {
                    LOGE("add agentData task failed!");
                }
            }
            catch (std::exception& exception)
            {
                LOGE("add agentData task throw: exception=%s", exception.what());
            }

            return err;
        }

        AgentDataSender::AgentDataSender(boost::shared_ptr<ScheduledExecutor> &scheduledExecutor,
                                         boost::shared_ptr<DataSender> &dataSender)
                : scheduledExecutor(scheduledExecutor), dataSender(dataSender)
        {

        }

        AgentDataSender::~AgentDataSender()
        {

        }

        int32_t AgentDataSender::init()
        {
            return SUCCESS;
        }

        TBasePtr AgentDataSender::createTAgentInfo()
        {
            TBasePtr tBasePtr;
            TAgentInfo *tAgentInfo = new TAgentInfo;

            PinpointAgentContextPtr contextPtr = PinpointAgentContext::getContextPtr();

            PINPOINT_ASSERT_RETURN((contextPtr != NULL), tBasePtr);

            tAgentInfo->__set_agentId(contextPtr->agentId);
            tAgentInfo->__set_agentVersion(contextPtr->agentVersion);
            tAgentInfo->__set_startTimestamp((int64_t)contextPtr->startTimestamp);
            tAgentInfo->__set_hostname(contextPtr->hostname);
            tAgentInfo->__set_pid(contextPtr->mainProcessPid);
            tAgentInfo->__set_ports(contextPtr->ports);
            tAgentInfo->__set_applicationName(contextPtr->applicationName);
            tAgentInfo->__set_serviceType(contextPtr->serviceType);
            tAgentInfo->__set_ip(contextPtr->ip);

            tBasePtr.reset(tAgentInfo);
            return tBasePtr;
        }

        int32_t AgentDataSender::send()
        {
            MainProcessChecker* mainProcessChecker = MainProcessChecker::createChecker();
            PINPOINT_ASSERT_RETURN((mainProcessChecker != NULL), FAILED);

            OS_process_id_t  mainPid = -1;
            if (!mainProcessChecker->isMainProcess(mainPid))
            {
                return SUCCESS;
            }

            try
            {
                TBasePtr tBasePtr = createTAgentInfo();

                PacketPtr packetPtr(new Packet(PacketType::APPLICATION_REQUEST, 3));
                PacketData& packetData = packetPtr->getPacketData();
                packetData = tBasePtr;

                LOGI("TAgentInfo: [%s]", utils::TBaseToString(packetData).c_str());

                dataSender->sendPacket(packetPtr, 100);
            }
            catch (std::exception exception)
            {
                LOGE("send agentData exception. exception=%s", exception.what());
            }

            return SUCCESS;
        }

        int32_t AgentDataSender::start()
        {
            boost::shared_ptr<AgentDataSender> sender = shared_from_this();
            int32_t err = 0;

            try
            {
                boost::shared_ptr<ExecutorTask> task(new AgentDataTask("agentDataTask", sender));

                err = scheduledExecutor->addTask(task, 1, 1);
            }
            catch (std::exception& exception)
            {
                LOGE("start agentData task throw: exception=%s", exception.what());
                return FAILED;
            }

            return err;

        }

    }
}

