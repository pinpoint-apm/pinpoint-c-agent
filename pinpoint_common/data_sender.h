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
#ifndef PINPOINT_DATA_SENDER_H
#define PINPOINT_DATA_SENDER_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>
#include "stdint.h"
#include "executor.h"
#include "utility.h"

namespace Pinpoint
{
    namespace Agent
    {

        class PacketType
        {
        public:
            // low 16bits are java definition
            static const int32_t APPLICATION_SEND = 1;
            static const int32_t APPLICATION_TRACE_SEND = 2;
            static const int32_t APPLICATION_TRACE_SEND_ACK = 3;


            static const int32_t APPLICATION_REQUEST = 5;
            static const int32_t APPLICATION_RESPONSE = 6;


            static const int32_t APPLICATION_STREAM_CREATE = 10;
            static const int32_t APPLICATION_STREAM_CREATE_SUCCESS = 12;
            static const int32_t APPLICATION_STREAM_CREATE_FAIL = 14;

            static const int32_t APPLICATION_STREAM_CLOSE = 15;

            static const int32_t APPLICATION_STREAM_PING = 17;
            static const int32_t APPLICATION_STREAM_PONG = 18;

            static const int32_t APPLICATION_STREAM_RESPONSE = 20;

            static const int32_t CONTROL_CLIENT_CLOSE = 100;
            static const int32_t CONTROL_SERVER_CLOSE = 110;

            static const int32_t CONTROL_HANDSHAKE = 150;
            static const int32_t CONTROL_HANDSHAKE_RESPONSE = 151;

            static const int32_t CONTROL_PING = 200;
            static const int32_t CONTROL_PONG = 201;

            static const int32_t UNKNOWN = 500;

            // high 16bits are c/c++ definition
            static const int32_t HEADLESS = (1 << 16);

        };

        typedef boost::variant<std::string, TBasePtr> PacketData;
        class Packet
        {
        public:

            Packet(int32_t type, uint16_t retryCount)
                    : type(type), coded(false), retryCount(retryCount)
            {

            }

            PacketData& getPacketData() { return data; }

            std::string&  getCodedData() { return codedData; }

            void setCodedData(const std::string& codedData)
            {
                this->codedData = codedData;
                this->coded = true;
            }

            bool isCode()
            {
                return this->coded;
            }

            const int32_t getType() { return type; }

            const bool retry()
            { return retryCount == 0 ? false : retryCount-- > 0;}

            void setFuture(boost::shared_ptr<Future>& future)
            {
                this->future = future;
            }

            virtual void onSuccess()
            {
                if (future != NULL)
                {
                    future->onSuccess();
                    // avoid cycle reference
                    future.reset();
                }
            }

            virtual void onError()
            {
                if (future != NULL)
                {
                    future->onError();
                    future.reset();
                }
            }

            virtual ~Packet() {}


        protected:
            int32_t type;
            PacketData data; // raw data
            bool coded;
            std::string codedData; // coded data. the data that will be sent to collector
            uint16_t retryCount;
            boost::shared_ptr<Future> future;
        };


        typedef boost::shared_ptr<Packet> PacketPtr;


        struct PingPacket
        {
            int32_t pingId;
            int8_t stateVersion;
            int8_t stateCode;
        };

        typedef boost::variant<int32_t, PingPacket> ResponseHeader;
        typedef boost::variant<int32_t, std::string> ResponseBody;

        struct ResponsePacket
        {
            int32_t type;
            ResponseHeader header;
            ResponseBody body;
        };

        typedef boost::shared_ptr<ResponsePacket> ResponsePacketPtr;


        class DataSender
        {
        public:
            virtual ~DataSender()
            { };

            virtual int32_t sendPacket(boost::shared_ptr<Packet> &packetPtr, int32_t timeout) = 0;

            virtual uint32_t getSendCount() = 0;

            virtual int32_t getSocketId() = 0;

            virtual void   init() = 0;

            virtual void   stop()=0;

        protected:
            DataSender(const std::string &ip, uint32_t port) : m_ip(ip), m_port(port)
            { };

            std::string m_ip;
            uint32_t m_port;

        private:
            explicit DataSender(const DataSender&);
            DataSender& operator=(const DataSender&);
        };

        typedef boost::shared_ptr<DataSender> DataSenderPtr;


        class UdpDataSender : public DataSender
        {
        public:
            UdpDataSender(boost::asio::io_service&,const std::string &name, const std::string &ip, uint32_t port);

            int32_t sendPacket(boost::shared_ptr<Packet> &packetPtr, int32_t timeout);

            virtual ~UdpDataSender();

            virtual uint32_t getSendCount();

            virtual int32_t getSocketId();

            virtual void stop();

            virtual void init();

            static const uint32_t MAX_REFRESH_MSEC = 100;
            static const uint32_t UDP_BUFFER_LEN = 1000;
            static const uint32_t MAX_GET_WAIT_MSEC = 100;
        private:
            SynchronizedFixedLengthQueue<boost::shared_ptr<Packet> > sendQueue;
            boost::asio::io_service& io;
            boost::asio::ip::udp::socket socket_;
            boost::asio::ip::udp::endpoint endpoint_;
            boost::asio::deadline_timer timer_;

            boost::atomic<uint32_t> m_sendCount;



            void send_udp_packet();

        };
    }
}

#endif
