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
#include "agent_monitor_sender.h"
#include "pinpoint_agent_context.h"

using namespace Pinpoint::utils;
using namespace Pinpoint::log;

namespace Pinpoint
{
    namespace Agent
    {

        AgentMonitorBatchTask::AgentMonitorBatchTask(const std::string &taskName,
                                                     boost::shared_ptr<AgentMonitorSender> &sender,
                                                     uint32_t batchNum)
                : ExecutorTask(taskName), sender(sender), batchNum(batchNum), count(0)
        {
            stats.reserve(batchNum);
        }

        AgentMonitorBatchTask::~AgentMonitorBatchTask()
        {

        }

        int32_t AgentMonitorBatchTask::run()
        {
            int err = SUCCESS;

            stats.push_back(sender->collectAgentStat());
            count++;

            if (count == batchNum)
            {
                PINPOINT_ASSERT_RETURN((stats.size() == batchNum), FAILED);
                count = 0;
                err = sender->send(stats);
                stats.clear();
            }

            return err;
        }


        AgentMonitorSender::AgentMonitorSender(boost::shared_ptr<ScheduledExecutor> &scheduledExecutor,
                                               boost::shared_ptr<DataSender> &dataSender)
                : scheduledExecutor(scheduledExecutor), dataSender(dataSender)
        {

        }

        AgentMonitorSender::~AgentMonitorSender()
        {

        }

        int32_t AgentMonitorSender::init()
        {
            return SUCCESS;
        }

        int32_t AgentMonitorSender::start()
        {
            boost::shared_ptr<AgentMonitorSender> sender = shared_from_this();

            boost::shared_ptr<ExecutorTask> task(new AgentMonitorBatchTask("agentMonitorBatchTask", sender,
                                                                           AgentMonitorBatchTask::BATCH_NUM));

            int32_t err = scheduledExecutor->addTask(task, 5000, -1);

            return err;
        }

        TAgentStat AgentMonitorSender::collectAgentStat()
        {
            TAgentStat stat;

            PinpointAgentContextPtr contextPtr = PinpointAgentContext::getContextPtr();

            assert (contextPtr != NULL);

            stat.__set_agentId(contextPtr->agentId);
            stat.__set_startTimestamp((int64_t)contextPtr->startTimestamp);

            // todo
            TCpuLoad cpuLoad;
            cpuLoad.__set_systemCpuLoad(0);
            stat.__set_cpuLoad(cpuLoad);

            return stat;
        }

        int32_t AgentMonitorSender::send(const std::vector<TAgentStat> &stats)
        {
            MainProcessChecker* mainProcessChecker = MainProcessChecker::createChecker();
            PINPOINT_ASSERT_RETURN((mainProcessChecker != NULL), FAILED);

            OS_process_id_t mainPid = 0;
            if (!mainProcessChecker->isMainProcess(mainPid))
            {
                return SUCCESS;
            }

            PinpointAgentContextPtr contextPtr = PinpointAgentContext::getContextPtr();
            PINPOINT_ASSERT_RETURN((contextPtr != NULL), FAILED);

            try
            {
                TAgentStatBatch statBatch;
                statBatch.__set_agentId(contextPtr->agentId);
                statBatch.__set_startTimestamp((int32_t)contextPtr->startTimestamp);
                statBatch.__set_agentStats(stats);

                LOGT("TAgentStatBatch: [%s]", utils::TBaseToString(statBatch).c_str());

                std::string data;

                int32_t err = serializer.serializer(statBatch, data);
                if (err != SUCCESS)
                {
                    LOGE("serialize statBatch failed: TAgentStatBatch: [%s]", utils::TBaseToString(statBatch).c_str());
                    return FAILED;
                }

                PacketPtr packetPtr(new Packet(PacketType::HEADLESS, 1));
                PacketData &packetData = packetPtr->getPacketData();
                packetData = data;

                dataSender->sendPacket(packetPtr, 100);
            }
            catch (std::exception& exception)
            {
                LOGE("send TAgentStatBatch throw: exception=%s", exception.what());
            }

            return SUCCESS;
        }

    }
}
