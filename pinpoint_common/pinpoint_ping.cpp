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
#include "pinpoint_ping.h"
#include "pinpoint_client.h"

namespace Pinpoint
{
    namespace Agent
    {
        PinpointPingPongHandler::PinpointPingPongHandler(boost::shared_ptr<ScheduledExecutor> &scheduledExecutor,
                                                         boost::shared_ptr<PinpointClient> &PinpointClient)
                : scheduledExecutor(scheduledExecutor), client(PinpointClient), pingCount(0)
        {

        }

        int32_t PinpointPingPongHandler::init()
        {
            try
            {
                channelBufferV2.reset();
                channelBufferV2.writeShort(PacketType::CONTROL_PONG);
                std::string pongData = channelBufferV2.getBuffer();
                channelBufferV2.reset();

                pongPacketPtr.reset(new Packet(PacketType::CONTROL_PONG, 1));
                PacketData& packetData = pongPacketPtr->getPacketData();
                packetData = pongData;
            }
            catch (std::exception& exception)
            {
                LOGE("get pongData throw: exception=%s", exception.what());
                return FAILED;
            }

            return SUCCESS;
        }

        void PinpointPingPongHandler::sendPong()
        {
            if (pongPacketPtr != NULL)
            {
                client->sendPacket(pongPacketPtr, 100);
            }
        }

        void PinpointPingPongHandler::sendPing(const PingPacket &ping)
        {
            LOGI("send ping: pingId=%d, stateVersion=%d, stateCode=%d",
                     ping.pingId, ping.stateVersion, ping.stateCode);

            try
            {
                pingChannelBufferV2.reset();
                pingChannelBufferV2.writeShort(PacketType::CONTROL_PING);

                std::string data;

                if (ping.pingId == -1)
                {
                    data = pingChannelBufferV2.getBuffer();
                    // todo: waiting to fix method for ping packet
                    LOGW("ignore pingId=-1 packet temporarily.");
                    return;
                }
                else
                {
                    pingChannelBufferV2.writeInt(ping.pingId);
                    pingChannelBufferV2.writeByte(ping.stateVersion);
                    pingChannelBufferV2.writeByte(ping.stateCode);
                    data = pingChannelBufferV2.getBuffer();
                }

                PacketPtr packetPtr(new Packet(PacketType::CONTROL_PING, 3));
                PacketData& packetData = packetPtr->getPacketData();
                packetData = data;
                client->sendPacket(packetPtr, 100);
            }
            catch (std::exception& exception)
            {
                LOGE("send ping throw: exception=%s", exception.what());
            }

        }

        int32_t PinpointPingPongHandler::startPing()
        {
            pingCount = 0;

            boost::shared_ptr<PinpointPingPongHandler> handler = shared_from_this();

            boost::shared_ptr<ExecutorTask> task(new PingTask("pingTask", handler));

            int32_t err = scheduledExecutor->addTask(task, 300, 1);

            return err;
        }


        PingTask::PingTask(const std::string &taskName, boost::shared_ptr<PinpointPingPongHandler> &handler)
                : ExecutorTask(taskName), handler(handler)
        {

        }

        PingTask::~PingTask()
        {

        }

        int32_t PingTask::run()
        {
            volatile SocketState* socketState = handler->client->getState();
            if (!SocketStateCodeOP::isRun(socketState->getCurrentState()))
            {
                return SUCCESS;
            }

            PingPacket pingPacket;
            pingPacket.pingId = handler->pingCount++;
            pingPacket.stateVersion = 0;
            pingPacket.stateCode = socketState->getCurrentState();
            handler->sendPing(pingPacket);

            try
            {
                boost::shared_ptr<ExecutorTask> task(new PingTask("pingTask", handler));

                int32_t err = handler->scheduledExecutor->addTask(task, pingInterval, 1);
                if (err != SUCCESS)
                {
                    LOGE("add ping task failed!");
                }
            }
            catch (std::exception& exception)
            {
                LOGE("add ping task throw: exception=%s", exception.what());
            }

            return SUCCESS;
        }
    }
}
