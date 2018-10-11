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
#include "data_sender.h"
#include <algorithm>
#include "pinpoint_error.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include "pinpoint_api.h"

using namespace Pinpoint::log;

namespace Pinpoint
{
    namespace Agent
    {

        //<editor-fold desc="UdpDataSender">

        UdpDataSender::UdpDataSender(boost::asio::io_service& agentIo,const std::string &name, const std::string &ip, uint32_t port)
                : DataSender(ip, port),
                  sendQueue(UDP_BUFFER_LEN),
				  io(agentIo),
				  socket_(agentIo),
				  timer_(agentIo),
				  m_sendCount(0)
        {
        	init();
        }

        UdpDataSender::~UdpDataSender()
        {

        }

        void UdpDataSender::init()
        {
            socket_.open(boost::asio::ip::udp::v4());
            boost::asio::ip::address add;

            try
            {
                add = boost::asio::ip::address::from_string(this->m_ip);
            }
            catch (std::exception &e)
            {
                LOGE(" ip error: ip=%s, err=%s", this->m_ip.c_str(), e.what());
                return;
            }
            catch (...)
            {
                LOGE(" ip error: ip=%s, err=%s" , this->m_ip.c_str(), "unknown");
                return;
            }

            endpoint_ = boost::asio::ip::udp::endpoint(add, short(this->m_port));
            timer_.expires_from_now(boost::posix_time::milliseconds(0));
            timer_.async_wait(boost::bind(&UdpDataSender::send_udp_packet, this));
        }

//        void UdpDataSender::stopTask()
//        {
////            io.stop();
//        }

        int32_t UdpDataSender::sendPacket(boost::shared_ptr<Packet> &packetPtr, int32_t timeout)
        {
            assert (timeout > 0 || (timeout < 0 && timeout == -1));

            int32_t err;

            if(packetPtr == NULL || packetPtr->getType() != PacketType::HEADLESS)
            {
                assert(false);
                return SUCCESS;
            }

            try
            {
                std::string codedData = boost::get<std::string>(packetPtr->getPacketData());
                packetPtr->setCodedData(codedData);
            }
            catch (std::exception& exception)
            {
                LOGE("udp send packet exception: ex=%s", exception.what());
                assert (false);
                return SUCCESS;
            }


            if (timeout == -1)
            {
                err = sendQueue.put(packetPtr);
            }
            else
            {
                err = sendQueue.put(packetPtr, (uint32_t)timeout);
            }

            m_sendCount++;

            return err;
        }

        void UdpDataSender::send_udp_packet()
        {
            typedef std::vector<PacketPtr> PacketPtrVec;
            PacketPtrVec packetPtrVec;

            int32_t err = sendQueue.getAll(packetPtrVec, MAX_GET_WAIT_MSEC);
            if (err != SUCCESS || packetPtrVec.empty())
            {

                timer_.expires_from_now(boost::posix_time::milliseconds(MAX_REFRESH_MSEC));

                goto _AGAIN;
            }

            for(PacketPtrVec::iterator ip = packetPtrVec.begin(); ip != packetPtrVec.end(); ++ip)
            {
                if ((*ip) == NULL)
                {
                    LOGE("get null packet pointer!!!");
                    assert(false);
                    continue;
                }
                try
                {
                    socket_.send_to(boost::asio::buffer((*ip)->getCodedData()), endpoint_);
                }
                catch (std::exception& e)
                {
                    LOGE("send udp packet failed. e=%s, length=%ld]", e.what(),(*ip)->getCodedData().length());
                }

            }


            timer_.expires_from_now(boost::posix_time::milliseconds(0)); // recheck it
	_AGAIN:
			timer_.async_wait(boost::bind(&UdpDataSender::send_udp_packet, this));

//            io.post(boost::bind(&UdpDataSender::send_udp_packet, this));
        }

        uint32_t UdpDataSender::getSendCount()
        {
            return m_sendCount;
        }

        int32_t UdpDataSender::getSocketId()
        {
            return socket_.is_open() ? socket_.native_handle() : -1;
        }

        //</editor-fold>

    }
}
