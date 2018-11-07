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
#ifndef PINPOINT_PING_H
#define PINPOINT_PING_H

#define __STDC_LIMIT_MACROS 
#include <stdint.h>
#include "executor.h"
#include "data_sender.h"
#include "buffer.h"

namespace Pinpoint
{
    namespace Agent
    {
        class PinpointClient;
        class PingTask;

        class PinpointPingPongHandler : public boost::enable_shared_from_this<PinpointPingPongHandler>
        {
        public:
            friend class PingTask;

            PinpointPingPongHandler(boost::shared_ptr<ScheduledExecutor> &scheduledExecutor,
                                    boost::shared_ptr<PinpointClient> &client);
            int32_t init();

            int32_t startPing();

            void sendPong();

        private:
            boost::shared_ptr<ScheduledExecutor> scheduledExecutor;
            boost::shared_ptr<PinpointClient> client;
            ChannelBufferV2 channelBufferV2;
            ChannelBufferV2 pingChannelBufferV2;
            PacketPtr pongPacketPtr;
            boost::atomic_int pingCount;

            void sendPing(const PingPacket& ping);
        };

        class PingTask : public ExecutorTask
        {
        public:
            static const uint32_t pingInterval = 5 * 60 * 1000; // 5 minutes
            PingTask(const std::string &taskName, boost::shared_ptr<PinpointPingPongHandler> &handler);

            ~PingTask();

            virtual int32_t run();

        private:
            boost::shared_ptr<PinpointPingPongHandler> handler;
        };
    }
}

#endif
