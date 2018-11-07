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
#include "hand_shake_sender.h"
#include "pinpoint_agent_context.h"
#include "serializer.h"

using namespace Pinpoint::utils;
using namespace Pinpoint::log;

namespace Pinpoint
{
    namespace Agent
    {

        HandshakeResponseCode HandshakeResponseCode::SUCCESS(0, 0, "Success.");
        HandshakeResponseCode HandshakeResponseCode::SIMPLEX_COMMUNICATION(0, 1, "Simplex Connection successfully established.");
        HandshakeResponseCode HandshakeResponseCode::DUPLEX_COMMUNICATION(0, 2, "Duplex Connection successfully established.");
        HandshakeResponseCode HandshakeResponseCode::ALREADY_KNOWN(1, 0, "Already Known.");
        HandshakeResponseCode HandshakeResponseCode::ALREADY_SIMPLEX_COMMUNICATION(1, 1, "Already Simplex Connection established.");
        HandshakeResponseCode HandshakeResponseCode::ALREADY_DUPLEX_COMMUNICATION(1, 2, "Already Duplex Connection established.");
        HandshakeResponseCode HandshakeResponseCode::PROPERTY_ERROR(2, 0, "Property error.");
        HandshakeResponseCode HandshakeResponseCode::PROTOCOL_ERROR(3, 0, "Illegal protocol error.");
        HandshakeResponseCode HandshakeResponseCode::UNKNOWN_ERROR(4, 0, "Unknown Error.");
        HandshakeResponseCode HandshakeResponseCode::UNKNOWN_CODE(-1, -1, "Unknown Code.");

        HandShakeTask::HandShakeTask(const std::string &taskName, boost::shared_ptr<HandShakeSender> &sender)
                : ExecutorTask(taskName), sender(sender)
        {

        }

        HandShakeTask::~HandShakeTask()
        {

        }

        int32_t HandShakeTask::run()
        {
            if (sender->status == HandShakeSender::STATE_STARTED)
            {
                sender->send();

                if (sender->handShakeCount < HandShakeSender::HANDSHAKE_RETRY_COUNT)
                {
                    try
                    {

                        boost::shared_ptr<ExecutorTask> task(new HandShakeTask("handShakeTask", sender));

                        sender->scheduledExecutor->addTask(task, 60, 1);
                    }
                    catch (std::exception& exception)
                    {
                        LOGE("add handshake task throw: e=%s", exception.what());
                        sender->status = HandShakeSender::STATE_FINISHED;
                    }

                }
                else
                {
                    LOGE("handshake abort!!!");
                    sender->status = HandShakeSender::STATE_FINISHED;
                }
            }

            return SUCCESS;
        }


        const std::string HandShakeSender::CODE("code");
        const std::string HandShakeSender::SUB_CODE("subCode");

        HandShakeSender::HandShakeSender(boost::shared_ptr<ScheduledExecutor> scheduledExecutor,
                                         boost::shared_ptr<DataSender> dataSender)
                : scheduledExecutor(scheduledExecutor), dataSender(dataSender), handShakeCount(0),
                  status(STATE_INIT), responseCode(NULL)
        {

        }

        HandShakeSender::~HandShakeSender()
        {

        }


        int32_t HandShakeSender::init()
        {
            return SUCCESS;
        }

        int32_t HandShakeSender::start()
        {
            this->status = STATE_STARTED;
            this->handShakeCount = 0;

            boost::shared_ptr<HandShakeSender> sender = shared_from_this();

            boost::shared_ptr<ExecutorTask> task(new HandShakeTask("handShakeTask", sender));

            // handshake right now
            int32_t err = scheduledExecutor->addTask(task, 1, 1);

            return err;
        }

        int32_t HandShakeSender::send()
        {
            PinpointAgentContextPtr contextPtr = PinpointAgentContext::getContextPtr();

            PINPOINT_ASSERT_RETURN((contextPtr != NULL), FAILED);

            try
            {
                encoder.startEncoder();

                encoder.addString("socketId");
                encoder.addInt32(dataSender->getSocketId());

                encoder.addString("hostName");
                encoder.addString(contextPtr->hostname);

                encoder.addString("supportServer");
                encoder.addBool(true);

                encoder.addString("ip");
                encoder.addString(contextPtr->ip);

                encoder.addString("agentId");
                encoder.addString(contextPtr->agentId);

                encoder.addString("applicationName");
                encoder.addString(contextPtr->applicationName);

                encoder.addString("serviceType");
                encoder.addInt32(contextPtr->serviceType);

                encoder.addString("pid");
                encoder.addInt32(contextPtr->mainProcessPid);

                encoder.addString("version");
                encoder.addString(contextPtr->agentVersion);

                encoder.addString("startTimestamp");
                encoder.addInt64((int64_t)contextPtr->startTimestamp);

                std::string data = encoder.endEncoder();


                // add pinpoint channel header
                ChannelBufferV2 buffer;
                buffer.writeShort(PacketType::CONTROL_HANDSHAKE);
                buffer.writeInt(handShakeCount++);
                buffer.appendPayload(data);


                PacketPtr packetPtr(new Packet(PacketType::CONTROL_HANDSHAKE, 3));
                PacketData &packetData = packetPtr->getPacketData();
                packetData = buffer.getBuffer();

                dataSender->sendPacket(packetPtr, 100);
            }
            catch (std::exception& exception)
            {
                LOGE("send handshake throw: exception=%s", exception.what());
            }


            return SUCCESS;
        }

        int32_t HandShakeSender::responseHandler(ResponsePacket &responsePacket)
        {
            PINPOINT_ASSERT_RETURN((responsePacket.type == PacketType::CONTROL_HANDSHAKE_RESPONSE), FAILED);

            ControlMessagePtr controlMessagePtr;
            try
            {
                decoder.setBuffer(boost::get<std::string>(responsePacket.body));
                controlMessagePtr = decoder.decode();
            }
            catch (std::exception& exception)
            {
                LOGE("decode HandShake response failed!!! exception=%s", exception.what());
                return FAILED;
            }

            PINPOINT_ASSERT_RETURN ((controlMessagePtr != NULL
                                     && controlMessagePtr->type == ControlMessage::CONTROL_MESSAGE_MAP
                                     && controlMessagePtr->data.ref != NULL), FAILED);

            LOGI("get control message: %s", controlMessagePtr->toString().c_str());

            if (controlMessagePtr != NULL
                && controlMessagePtr->type == ControlMessage::CONTROL_MESSAGE_MAP
                && controlMessagePtr->data.ref != NULL)
            {

                ControlMessageMap* controlMessageMap = (ControlMessageMap*)controlMessagePtr->data.ref;
                if (controlMessageMap != NULL)
                {
                    int32_t code = -1;
                    int32_t subCode = -1;

                    ControlMessagePtr codeMessagePtr =
                            controlMessageMap->get(ControlMessage::CONTROL_MESSAGE_STRING, (void*)&CODE);
                    ControlMessagePtr subCodeMessagePtr =
                            controlMessageMap->get(ControlMessage::CONTROL_MESSAGE_STRING, (void*)&SUB_CODE);

                    if (codeMessagePtr != NULL && codeMessagePtr->type == ControlMessage::CONTROL_MESSAGE_LONG)
                    {
                        code = codeMessagePtr->data.i;
                    }

                    if (subCodeMessagePtr != NULL && subCodeMessagePtr->type == ControlMessage::CONTROL_MESSAGE_LONG)
                    {
                        subCode = subCodeMessagePtr->data.i;
                    }

                    HandshakeResponseCode* responseCodeRawPtr = HandshakeResponseCode::get(code, subCode);
                    LOGI("handshake response code = %s", responseCodeRawPtr->toString().c_str());

                    HandshakeResponseCode* old = responseCode.exchange(responseCodeRawPtr);
                    if (old != NULL)
                    {
                        LOGI("old handshake response code = %s", old->toString().c_str());
                    }


                    status = STATE_FINISHED;

                }

            }
            else
            {
                PINPOINT_ASSERT_RETURN(false, FAILED)
                LOGE("get control message error!");
            }

            status = STATE_FINISHED;

            return SUCCESS;

        }

    }
}

