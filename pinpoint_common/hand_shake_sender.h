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
#ifndef PINPOINT_HAND_SHAKE_SENDER_H
#define PINPOINT_HAND_SHAKE_SENDER_H

#include "data_sender.h"
#include "thrift/gen-cpp/Pinpoint_types.h"
#include "serializer.h"
#include <boost/enable_shared_from_this.hpp>

namespace Pinpoint
{
    namespace Agent
    {
        class HandshakeResponseCode;
//        typedef boost::shared_ptr<HandshakeResponseCode> HandshakeResponseCodePtr;

        class HandshakeResponseCode
        {
        public:
            int32_t getCode() {return code;}
            int32_t getSubCode() {return subCode;}
            const std::string& getCodeMessage() {return codeMessage;}

            static HandshakeResponseCode SUCCESS;
            static HandshakeResponseCode SIMPLEX_COMMUNICATION;
            static HandshakeResponseCode DUPLEX_COMMUNICATION;
            static HandshakeResponseCode ALREADY_KNOWN;
            static HandshakeResponseCode ALREADY_SIMPLEX_COMMUNICATION;
            static HandshakeResponseCode ALREADY_DUPLEX_COMMUNICATION;
            static HandshakeResponseCode PROPERTY_ERROR;
            static HandshakeResponseCode PROTOCOL_ERROR;
            static HandshakeResponseCode UNKNOWN_ERROR;
            static HandshakeResponseCode UNKNOWN_CODE;

            static HandshakeResponseCode* get(int32_t code, int32_t subCode)
            {
                if(code == 0 && subCode == 0)
                {
                    return &HandshakeResponseCode::SUCCESS;
                }
                else if (code == 0 && subCode == 1)
                {
                    return &HandshakeResponseCode::SIMPLEX_COMMUNICATION;
                }
                else if (code == 0 && subCode == 2)
                {
                    return &HandshakeResponseCode::DUPLEX_COMMUNICATION;
                }
                else if (code == 1 && subCode == 0)
                {
                    return &HandshakeResponseCode::ALREADY_KNOWN;
                }
                else if (code == 1 && subCode == 1)
                {
                    return &HandshakeResponseCode::ALREADY_SIMPLEX_COMMUNICATION;
                }
                else if (code == 1 && subCode == 2)
                {
                    return &HandshakeResponseCode::ALREADY_DUPLEX_COMMUNICATION;
                }
                else if (code == 2 && subCode == 0)
                {
                    return &HandshakeResponseCode::PROPERTY_ERROR;
                }
                else if (code == 3 && subCode == 0)
                {
                    return &HandshakeResponseCode::PROTOCOL_ERROR;
                }
                else if (code == 4 && subCode == 0)
                {
                    return &HandshakeResponseCode::UNKNOWN_ERROR;
                }
                else
                {
                    return &HandshakeResponseCode::UNKNOWN_CODE;
                }
            }

            std::string toString()
            {
                return this->codeMessage;
            }

        protected:
            HandshakeResponseCode(int32_t code, int32_t subCode, const std::string& codeMessage)
                    : code(code), subCode(subCode), codeMessage(codeMessage)
            {

            }
        private:
            int32_t code;
            int32_t subCode;
            std::string codeMessage;
        };


        class HandShakeSender;

        class HandShakeTask : public ExecutorTask
        {
        public:
            HandShakeTask(const std::string &taskName, boost::shared_ptr<HandShakeSender> &sender);

            ~HandShakeTask();

            virtual int32_t run();

        private:
            boost::shared_ptr<HandShakeSender> sender;
        };

        class HandShakeSender : public boost::enable_shared_from_this<HandShakeSender>
        {
        public:
            friend class HandShakeTask;

            static const int32_t STATE_INIT = 0;
            static const int32_t  STATE_STARTED = 1;
            static const int32_t  STATE_FINISHED = 2;

            static const int32_t HANDSHAKE_RETRY_COUNT = 10;

            static const std::string CODE;
            static const std::string SUB_CODE;

            HandShakeSender(boost::shared_ptr<ScheduledExecutor> &scheduledExecutor,
                            boost::shared_ptr<DataSender> &dataSender);

            ~HandShakeSender();

            int32_t init();

            int32_t start();

            int32_t getStatus() { return status; }
            HandshakeResponseCode* getHandshakeResponseCode() { return responseCode;}

            int32_t responseHandler(ResponsePacket& responsePacket);

        private:
            boost::shared_ptr<ScheduledExecutor> scheduledExecutor;
            boost::shared_ptr<DataSender> dataSender;

            ControlMessageEncoder encoder;
            ControlMessageDecoder decoder;
            boost::atomic_int handShakeCount;
            boost::atomic_int status;
            boost::atomic<HandshakeResponseCode*> responseCode;

            int32_t send();
        };

    }
}

#endif
