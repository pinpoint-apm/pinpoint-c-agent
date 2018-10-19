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
#ifndef PINPOINT_CLIENT_H
#define PINPOINT_CLIENT_H

#include "stdint.h"
#include "string"
#include "pinpoint_api.h"
#include "executor.h"
#include "data_sender.h"
#include "hand_shake_sender.h"
#include "pinpoint_ping.h"

namespace Pinpoint
{
    namespace Agent
    {
        typedef int8_t SocketStateCode;
        class SocketStateCodeOP
        {
        public:
            static const SocketStateCode NONE = 1;
            static const SocketStateCode BEING_CONNECT = 2;
            static const SocketStateCode CONNECTED = 3;
            static const SocketStateCode CONNECT_FAILED = 6;
            static const SocketStateCode IGNORE = 9;
            static const SocketStateCode RUN_WITHOUT_HANDSHAKE = 10;
            static const SocketStateCode RUN_SIMPLEX = 11;
            static const SocketStateCode RUN_DUPLEX = 12;
            static const SocketStateCode BEING_CLOSE_BY_CLIENT = 20;
            static const SocketStateCode CLOSED_BY_CLIENT = 22;
            static const SocketStateCode UNEXPECTED_CLOSE_BY_CLIENT = 26;
            static const SocketStateCode BEING_CLOSE_BY_SERVER = 30;
            static const SocketStateCode CLOSED_BY_SERVER = 32;
            static const SocketStateCode UNEXPECTED_CLOSE_BY_SERVER = 36;
            static const SocketStateCode ERROR_UNKNOWN = 40;
            static const SocketStateCode ERROR_ILLEGAL_STATE_CHANGE = 41;
            static const SocketStateCode ERROR_SYNC_STATE_SESSION = 42;

            static bool canChangeState(SocketStateCode state, SocketStateCode nextState);
            static bool isBeforeConnected(SocketStateCode state);
            static bool isRun(SocketStateCode state);
            static bool isRunDuplex(SocketStateCode state);
            static bool onClose(SocketStateCode state);
            static bool isClosed(SocketStateCode state);
            static bool isError(SocketStateCode state);
        };

        class SocketStateChangeResult
        {
        public:
            SocketStateChangeResult(bool changed, SocketStateCode beforeState, SocketStateCode currentState, SocketStateCode updateWantedState)
            {
                this->changed = changed;
                this->beforeState = beforeState;
                this->currentState = currentState;
                this->updateWantedState = updateWantedState;
            }

            bool  isChange()
            {
                return changed;
            }

            SocketStateCode getBeforeState()
            {
                return beforeState;
            }

            SocketStateCode getCurrentState()
            {
                return currentState;
            }

            SocketStateCode getUpdateWantedState()
            {
                return updateWantedState;
            }

            std::string toString()
            {
                utils::PStream pStream;
                try
                {
                    pStream = utils::FormatConverter::getPStream();
                }
                catch (...)
                {
                    return "SocketStateChangeResult toString failed.";
                }

                *pStream << "Socket state change ";

                if (changed) {
                    *pStream << "success";
                } else {
                    *pStream << "fail";
                }

                *pStream << "(updateWanted:";
                *pStream << updateWantedState;

                *pStream << " ,before:";
                *pStream << beforeState;

                *pStream << " ,current:";
                *pStream << currentState;

                *pStream << ").";

                return pStream->str();
            }

        private:
            bool changed;
            SocketStateCode beforeState;
            SocketStateCode currentState;
            SocketStateCode updateWantedState;

        };

        class SocketState
        {
        public:
            SocketState()
            {
                beforeState = SocketStateCodeOP::NONE;
                currentState = SocketStateCodeOP::NONE;
            }

            SocketStateChangeResult toBeingConnect() volatile
            {
                SocketStateCode nextState = SocketStateCodeOP::BEING_CONNECT;
                return to(nextState);
            }

            SocketStateChangeResult toConnected() volatile
            {
                SocketStateCode nextState = SocketStateCodeOP::CONNECTED;
                return to(nextState);
            }

            SocketStateChangeResult toConnectFailed() volatile
            {
                SocketStateCode nextState = SocketStateCodeOP::CONNECT_FAILED;
                return to(nextState);
            }

            SocketStateChangeResult toIgnore() volatile
            {
                SocketStateCode nextState = SocketStateCodeOP::IGNORE;
                return to(nextState);
            }

            SocketStateChangeResult toRunWithoutHandshake() volatile
            {
                SocketStateCode nextState = SocketStateCodeOP::RUN_WITHOUT_HANDSHAKE;
                return to(nextState);
            }

            SocketStateChangeResult toRunSimplex() volatile
            {
                SocketStateCode nextState = SocketStateCodeOP::RUN_SIMPLEX;
                return to(nextState);
            }

            SocketStateChangeResult toRunDuplex() volatile
            {
                SocketStateCode nextState = SocketStateCodeOP::RUN_DUPLEX;
                return to(nextState);
            }

            SocketStateChangeResult toBeingCloseByClient() volatile
            {
                SocketStateCode nextState = SocketStateCodeOP::BEING_CLOSE_BY_CLIENT;
                return to(nextState);
            }

            SocketStateChangeResult toClosedByClient() volatile
            {
                SocketStateCode nextState = SocketStateCodeOP::CLOSED_BY_CLIENT;
                return to(nextState);
            }

            SocketStateChangeResult toUnexpectedCloseByClient() volatile
            {
                SocketStateCode nextState = SocketStateCodeOP::UNEXPECTED_CLOSE_BY_CLIENT;
                return to(nextState);
            }

            SocketStateChangeResult toBeingCloseByServer() volatile
            {
                SocketStateCode nextState = SocketStateCodeOP::BEING_CLOSE_BY_SERVER;
                return to(nextState);
            }

            SocketStateChangeResult toClosedByServer() volatile
            {
                SocketStateCode nextState = SocketStateCodeOP::CLOSED_BY_SERVER;
                return to(nextState);
            }

            SocketStateChangeResult toUnexpectedCloseByServer() volatile
            {
                SocketStateCode nextState = SocketStateCodeOP::UNEXPECTED_CLOSE_BY_SERVER;
                return to(nextState);
            }

            SocketStateChangeResult toUnknownError() volatile
            {
                SocketStateCode nextState = SocketStateCodeOP::ERROR_UNKNOWN;
                return to(nextState);
            }

            SocketStateChangeResult toSyncStateSessionError() volatile
            {
                SocketStateCode nextState = SocketStateCodeOP::ERROR_SYNC_STATE_SESSION;
                return to(nextState);
            }

            SocketStateCode getCurrentState() volatile
            {
                return currentState;
            }

            std::string toString() volatile
            {
                utils::PStream pStream;
                try
                {
                    pStream = utils::FormatConverter::getPStream();
                }
                catch (...)
                {
                    return "SocketState toString failed.";
                }

                *pStream << "SocketState";
                *pStream << "(";
                *pStream << beforeState;
                *pStream << "->";
                *pStream << currentState;
                *pStream << ")";

                return pStream->str();
            }

        private:
            volatile SocketStateCode beforeState;
            volatile SocketStateCode currentState;

            SocketStateChangeResult to(SocketStateCode nextState) volatile
            {
                bool enable = SocketStateCodeOP::canChangeState(this->currentState, nextState);
                if (enable)
                {
                    this->beforeState = this->currentState;
                    this->currentState = nextState;
                    LOGI("state change: %d => %d", this->beforeState, this->currentState);
                    return SocketStateChangeResult(true, beforeState, currentState, nextState);
                }
                return SocketStateChangeResult(false, beforeState, currentState, nextState);
            }
        };

        typedef std::map<int32_t, PacketPtr> WaitResponseQueue;

        class PinpointClient : public DataSender, public boost::enable_shared_from_this<PinpointClient>
        {
        public:
            friend class TcpDataSenderV2;
            PinpointClient(boost::asio::io_service& ,const std::string &executorName,
                           const std::string& ip,
                           uint32_t port,
                           boost::shared_ptr<ScheduledExecutor>& scheduledExecutor,
                           uint32_t reconInter);
            // timeout no use
            int32_t sendPacket(boost::shared_ptr<Packet> &packetPtr, int32_t timeout);
            uint32_t getSendCount() {return sendCount;}
            int32_t getSocketId();
            boost::asio::io_service& getIO() {return this->io_;}
            volatile SocketState* getState() {return &state;}
#ifdef RUN_TEST
            boost::asio::ip::tcp::socket& getSocket() { return this->socket_; };
#endif

            virtual void init();

            virtual void stop();

        protected:
            static const uint32_t TCP_RESPONSE_HEADER_LEN = 2;
            static const uint32_t READ_BUFFER_LEN = 1024 * 64;
            static const uint32_t WRITE_BUFFER_LEN = 1024 * 64;
            static const uint32_t SEND_QUEUE_LEN = 1024 * 10;

#ifdef RUN_TEST
            static const uint32_t WAIT_RESPONSE_QUEUE_LEN = 2;
#else
            static const uint32_t WAIT_RESPONSE_QUEUE_LEN = 200;
#endif
            // not thread safe
            int32_t sendPacket_(boost::shared_ptr<Packet> &packetPtr, int32_t timeout);

        private:
            PinpointClient(const PinpointClient&);
            PinpointClient& operator=(const PinpointClient&);
            utils::FixedLengthQueue<PacketPtr> sendQueue;
            WaitResponseQueue waitResponseQueue;
            boost::atomic<uint32_t> sendCount;
            boost::atomic<uint32_t> requestCount;
            boost::asio::io_service &io_;
            boost::asio::ip::tcp::socket socket_;
            boost::asio::ip::tcp::endpoint endpoint_;
            boost::asio::deadline_timer timer_;
            uint32_t interval;
            volatile SocketState state; // java server's definition
            bool isWriting;
            ChannelBufferV2 readBuffer;
            ResponsePacket responsePacket;
            HeaderTBaseDeSerializer deserializer;

            boost::shared_ptr<ScheduledExecutor> scheduledExecutor;
            boost::shared_ptr<HandShakeSender> handShakeSender;
            boost::shared_ptr<PinpointPingPongHandler> pingPongHandler;

            HeaderTBaseSerializer serializer;

            void start_connect_timer();
            void try_connect();
            void connect_timer_event(const boost::system::error_code& /*e*/);
            void write_timer_event(const boost::system::error_code& /*e*/);

            void handle_connect_event(const boost::system::error_code &ec);

            void handle_error(const boost::system::error_code &error);

            void start_write();
            void handle_write(const boost::system::error_code& error, PacketPtr& packetPtr);

            void start_read();
//            void handle_read_packet(const boost::system::error_code& error,
//                                    std::size_t bytes_transferred);
            void handle_read_type(const boost::system::error_code& error);
            void handle_read_header(const boost::system::error_code& error);
            void handle_read_body(const boost::system::error_code& error);

            void responseHandler();
            int32_t requestResponseHandler();

            void doRequest(PacketPtr& packetPtr);

            ///E_EXIT force close the TCP connection
            ///E_CLOSE initialized state
            enum E_NState {E_EXIT,E_CLOSE,E_CONNECTING,E_CONNECTED,E_WRITTING,E_READING} ;

            E_NState nstate;

        };
        typedef boost::shared_ptr<PinpointClient> PinpointClientPtr;
    }
}

#endif
