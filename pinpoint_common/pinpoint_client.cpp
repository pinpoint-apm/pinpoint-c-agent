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
#include <assert.h>
#include "pinpoint_client.h"
#include "pinpoint_api.h"
#include "data_sender.h"

namespace Pinpoint
{
    namespace Agent
    {
        //<editor-fold desc="SocketStateCodeOP">

        bool SocketStateCodeOP::canChangeState(SocketStateCode state, SocketStateCode nextState)
        {
            switch (nextState)
            {
                case BEING_CONNECT:
                    return state == NONE;
                case CONNECTED:
                    return state == NONE || state == BEING_CONNECT || state == CONNECT_FAILED;
                case CONNECT_FAILED:
                    return state == BEING_CONNECT || state == RUN_WITHOUT_HANDSHAKE
                           || state == RUN_SIMPLEX || state == RUN_DUPLEX;;
                case IGNORE:
                    return state == CONNECTED;
                case RUN_WITHOUT_HANDSHAKE:
                    return state == CONNECTED;
                case RUN_SIMPLEX:
                    return state == RUN_WITHOUT_HANDSHAKE;
                case RUN_DUPLEX:
                    return state == RUN_WITHOUT_HANDSHAKE;
                case BEING_CLOSE_BY_CLIENT:
                    return state == RUN_WITHOUT_HANDSHAKE || state == RUN_SIMPLEX || state == RUN_DUPLEX;
                case CLOSED_BY_CLIENT:
                    return state == BEING_CLOSE_BY_CLIENT;
                case UNEXPECTED_CLOSE_BY_CLIENT:
                    return state == CONNECTED || state == RUN_WITHOUT_HANDSHAKE
                           || state == RUN_SIMPLEX || state == RUN_DUPLEX;
                case BEING_CLOSE_BY_SERVER:
                    return state == RUN_WITHOUT_HANDSHAKE || state == RUN_SIMPLEX || state == RUN_DUPLEX;
                case CLOSED_BY_SERVER:
                    return state == BEING_CLOSE_BY_SERVER;
                case UNEXPECTED_CLOSE_BY_SERVER:
                    return state == RUN_WITHOUT_HANDSHAKE || state == RUN_SIMPLEX || state == RUN_DUPLEX;
                default:
                    LOGE("error state change: %d to %d", state, nextState);
                    assert(false);
                    return false;
            }
        }

        bool SocketStateCodeOP::isBeforeConnected(SocketStateCode state)
        {
            switch (state)
            {
                case NONE:
                case BEING_CONNECT:
                    return true;
                default:
                    return false;
            }
        }

        bool SocketStateCodeOP::isRun(SocketStateCode state)
        {
            switch (state)
            {
                case RUN_WITHOUT_HANDSHAKE:
                case RUN_SIMPLEX:
                case RUN_DUPLEX:
                    return true;
                default:
                    return false;
            }
        }

        bool SocketStateCodeOP::isRunDuplex(SocketStateCode state)
        {
            return state == RUN_DUPLEX;
        }

        bool SocketStateCodeOP::onClose(SocketStateCode state)
        {
            switch (state)
            {
                case BEING_CLOSE_BY_CLIENT:
                case BEING_CLOSE_BY_SERVER:
                    return true;
                default:
                    return false;
            }
        }

        bool SocketStateCodeOP::isClosed(SocketStateCode state)
        {
            switch (state) {
                case CLOSED_BY_CLIENT:
                case UNEXPECTED_CLOSE_BY_CLIENT:
                case CLOSED_BY_SERVER:
                case UNEXPECTED_CLOSE_BY_SERVER:
                case ERROR_UNKNOWN:
                case ERROR_ILLEGAL_STATE_CHANGE:
                case ERROR_SYNC_STATE_SESSION:
                    return true;
                default:
                    return false;
            }
        }

        bool SocketStateCodeOP::isError(SocketStateCode state)
        {
            switch (state) {
                case ERROR_ILLEGAL_STATE_CHANGE:
                case ERROR_UNKNOWN:
                    return true;
                default:
                    return false;
            }
        }
        //</editor-fold>

        //<editor-fold desc="PinpointClient">

        PinpointClient::PinpointClient(boost::asio::io_service& agentIo,const std::string &executorName,
                                       const std::string& ip,
                                       uint32_t port,
                                       boost::shared_ptr<ScheduledExecutor>& scheduledExecutor,
                                       uint32_t reconInter)
                : DataSender(ip, port),
                  sendQueue(SEND_QUEUE_LEN),
				  sendCount(0),
				  requestCount(0),
				  io_(agentIo),
//				  work(agentIo),
				  socket_(agentIo),
				  timer_(agentIo),
                  interval(reconInter),
				  state(),
				  isWriting(false),
				  scheduledExecutor(scheduledExecutor)
        {
            nstate  = E_CLOSE;

        }

        void PinpointClient::init()
        {
            try
            {

                PinpointClientPtr pinpointClientPtr = shared_from_this();
                DataSenderPtr dataSenderPtr =  boost::dynamic_pointer_cast<DataSender>(pinpointClientPtr);

                handShakeSender.reset(new HandShakeSender(scheduledExecutor,
                                                          dataSenderPtr));

                if (handShakeSender->init() != SUCCESS)
                {
                    LOGE("handShakeSender init failed!!!");
                    return;
                }

                pingPongHandler.reset(new PinpointPingPongHandler(scheduledExecutor,
                		pinpointClientPtr));

                if (pingPongHandler->init() != SUCCESS)
                {
                    LOGE("pingPongHandler init failed!!!");
                    return;
                }


                boost::asio::ip::address add;
                add = boost::asio::ip::address::from_string(this->DataSender::m_ip);
                endpoint_ = boost::asio::ip::tcp::endpoint(add, short(this->DataSender::m_port));
                try_connect();
            }
            catch (std::exception& exception)
            {
                LOGE("PinpointClient::executeTask exception=%s", exception.what());
            }
        }

        void PinpointClient::write_timer_event(const boost::system::error_code& /*e*/)
        {
            start_write();
        }

        void PinpointClient::connect_timer_event(const boost::system::error_code& /*e*/)
        {
            try_connect();
        }

        void PinpointClient::try_connect()
        {


            LOGI("start_connect: %s:%d", this->DataSender::m_ip.c_str(), this->DataSender::m_port);

            try
            {
                socket_.open(boost::asio::ip::tcp::v4());

                boost::asio::socket_base::keep_alive keep_alive(true);
                socket_.set_option(keep_alive);

                boost::asio::socket_base::receive_buffer_size receive_buffer_size(READ_BUFFER_LEN);
                socket_.set_option(receive_buffer_size);

                boost::asio::socket_base::send_buffer_size send_buffer_size(WRITE_BUFFER_LEN);
                socket_.set_option(send_buffer_size);

                boost::asio::ip::tcp::no_delay no_delay(true);
                socket_.set_option(no_delay);

                socket_.async_connect(endpoint_,
                                      boost::bind(&PinpointClient::handle_connect_event,
                                                  this, _1));
                nstate = E_CONNECTING;

            }
            catch (std::exception& exception)
            {
                LOGE("PinpointClient::start_connect get exception = %s", exception.what());
                assert(false);
            }

        }


        void PinpointClient::handle_error(const boost::system::error_code &ec)
        {
            // close current socket

            if(nstate == E_CONNECTING || nstate ==  E_EXIT )
            {
                return ;
            }

            this->socket_.close();

            // retry + timer
            timer_.cancel();
            timer_.expires_from_now(boost::posix_time::seconds(interval));
            timer_.async_wait(boost::bind(&PinpointClient::connect_timer_event,this,_1));
            LOGE("connect %s:%u timeout:%d failed try again",endpoint_.address().to_string().c_str(), endpoint_.port(),interval);
            nstate = E_CONNECTING;

        }

        void PinpointClient::handle_connect_event(const boost::system::error_code &ec)
        {

            if (ec)
            {
                handle_error(ec);
            }
            else
            {
                state.toConnected();
                LOGI(" Connect [%s:%d] success.",this->DataSender::m_ip.c_str(),this->DataSender::m_port);
                state.toRunWithoutHandshake();
                try
                {
                    handShakeSender->start();

                    pingPongHandler->startPing();
                }
                catch (std::exception& exception)
                {
                    LOGE("PinpointClient::handle_connect throw: %s", exception.what());
                }

                // remove connect timer
                timer_.cancel();
                nstate = E_CONNECTED;

                // start read and write event
                start_write();
                start_read();
            }
        }

        int32_t PinpointClient::sendPacket_(boost::shared_ptr<Packet> &packetPtr, int32_t timeout)
        {
            if (packetPtr == NULL)
            {
                assert(false);
                return SUCCESS;
            }

            try
            {
                int32_t err = SUCCESS;

                switch (packetPtr->getType())
                {
                    case PacketType::APPLICATION_REQUEST:
                        doRequest(packetPtr);
                        break;
                    case PacketType::CONTROL_HANDSHAKE:
                    case PacketType::CONTROL_PING:
                    case PacketType::CONTROL_PONG:
#ifdef RUN_TEST
                    case PacketType::HEADLESS:
#endif
                        packetPtr->setCodedData(
                                boost::get<std::string>(packetPtr->getPacketData()));
                        err = sendQueue.push_back(packetPtr);
                        if (err != SUCCESS)
                        {
                            LOGE("sendQueue.push_back failed! err=%d", err);
                        }
                        break;

                    default:
                        LOGE("send unknown type packet. type=%d", packetPtr->getType());
                        assert (false);
                        break;
                }
            }
            catch (std::exception& exception)
            {
                LOGE("PinpointClient::sendPacket_ exception=%s", exception.what());
            }

            return SUCCESS;
        }

        void PinpointClient::doRequest(PacketPtr &packetPtr)
        {
            assert (packetPtr->getType() == PacketType::APPLICATION_REQUEST);

            try
            {
                TBasePtr tBasePtr = boost::get<TBasePtr>(packetPtr->getPacketData());
                if (tBasePtr == NULL)
                {
                    assert (false);
                    return;
                }

                int32_t requestId = this->requestCount++;

                LOGD("doRequest: requestId=%d, tBase=[%s]", requestId,
                         utils::TBaseToString2(tBasePtr).c_str());

                std::string data;
                int32_t err;

                err = serializer.serializer(*tBasePtr, data);
                if (err != SUCCESS)
                {
                    LOGE("serializer tBase failed");
                    return;
                }
                // add request header
                ChannelBufferV2 buffer;
                buffer.writeShort(PacketType::APPLICATION_REQUEST);
                buffer.writeInt(requestId);
                buffer.appendPayload(data);

                packetPtr->setCodedData(buffer.getBuffer());

                err = sendQueue.push_back(packetPtr);
                if (err != SUCCESS)
                {
                    LOGW("sendQueue.push_back failed! err=%d", err);
                    return;
                }

                if (waitResponseQueue.size() > WAIT_RESPONSE_QUEUE_LEN)
                {
                    LOGW("waitResponseQueue full! ignore response!");
                }
                else
                {
                    waitResponseQueue[requestId] = packetPtr;
                }
            }
            catch (std::exception& exception)
            {
                LOGE("doRequest exception=%s", exception.what());
            }

        }

        int32_t PinpointClient::sendPacket(boost::shared_ptr<Packet> &packetPtr, int32_t timeout)
        {
            try
            {
                sendCount++;
                io_.post(boost::bind(&PinpointClient::sendPacket_, this, packetPtr, timeout));
                io_.post(boost::bind(&PinpointClient::start_write, this));
            }
            catch (std::exception& exception)
            {
                LOGE("PinpointClient::sendPacket io.post throw exception=%s.", exception.what());
                return FAILED;
            }

            return SUCCESS;
        }

        void PinpointClient::start_write()
        {

            if (nstate != E_CONNECTED )
            {
                // no needs to cancel write_timer_event, timer is  connect_timer_event
            	LOGD("connection not ready, wait for next time");
                return;
            }

            if (sendQueue.empty())
            {
                 return ;
            }

            try
            {
                PacketPtr packetPtr;
                int32_t err = sendQueue.pop_front(packetPtr);
                if (err != SUCCESS)
                {
                    LOGE("sendQueue.pop_front failed! err=%d", err);
                    return;
                }


                if (packetPtr->isCode())
                {
                    isWriting = true;
                    boost::asio::async_write(socket_,
                                             boost::asio::buffer(packetPtr->getCodedData()),
                                             boost::bind(&PinpointClient::handle_write, this,
                                                         boost::asio::placeholders::error, packetPtr));
                }
            }
            catch (std::exception& exception)
            {
                LOGE("PinpointClient::start_write throw exception=%s.", exception.what());
            }

        }

        void PinpointClient::handle_write(const boost::system::error_code &error, PacketPtr& packetPtr)
        {

            if (!error)
            {
            	LOGD("send [%d bytes] to collector",packetPtr->getCodedData().length());
                start_write();
            }
            else
            {
                LOGW("write failed: error=%s", error.message().c_str());

                if (packetPtr->retry())
                {
                    int32_t err = sendQueue.push_front(packetPtr);
                    if (err != SUCCESS)
                    {
                        LOGW("sendQueue.push_front failed! err=%d", err);
                    }
                }

                if ((boost::asio::error::eof == error) ||
                    (boost::asio::error::connection_reset == error) ||
                    (boost::asio::error::broken_pipe == error) ||
                    (boost::asio::error::bad_descriptor == error))
                {
                    LOGW("connect broken: %s:%d", this->DataSender::m_ip.c_str(), this->DataSender::m_port);
                    handle_error(error);

                }
                else
                {
                    LOGI("handle_write: %s ",error.message().c_str());
                    assert(0);
                }
            }

        }

        void PinpointClient::start_read()
        {
            // Caution: if read error, stop read.
            // we get packets one by one, if one packet fails to be decoded, we can not decode others.

            try
            {
                readBuffer.reset();
                if (!readBuffer.ensureWriteCapacity(READ_BUFFER_LEN))
                {
                    LOGE("ensureWriteCapacity failed.");
                    return;
                }

                boost::asio::async_read(socket_,
                                        boost::asio::buffer(readBuffer.getWriteAddr(TCP_RESPONSE_HEADER_LEN), TCP_RESPONSE_HEADER_LEN),
                                        boost::bind(&PinpointClient::handle_read_type, this,
                                                    boost::asio::placeholders::error));
            }
            catch (std::exception& exception)
            {
                LOGE("boost::asio::async_read throw exception=%s.", exception.what());
            }

        }

        void PinpointClient::handle_read_type(const boost::system::error_code &error)
        {

            if (!error)
            {
                try
                {
                    int16_t type = readBuffer.readShort();
                    responsePacket.type = type;
                    uint32_t header_size = 0;
                    switch (type)
                    {
                        case PacketType::APPLICATION_SEND:
                        case PacketType::CONTROL_CLIENT_CLOSE:
                        case PacketType::CONTROL_SERVER_CLOSE:
                            // header: payLoad
                            header_size = 4;
                            break;
                        case PacketType::CONTROL_HANDSHAKE_RESPONSE:
                        case PacketType::CONTROL_HANDSHAKE:
                        case PacketType::APPLICATION_RESPONSE:
                        case PacketType::APPLICATION_REQUEST:
                            // header: requestId + payLoad
                            header_size = 4 + 4;
                            break;
                        case PacketType::APPLICATION_STREAM_CREATE:
                        case PacketType::APPLICATION_STREAM_CLOSE:
                        case PacketType::APPLICATION_STREAM_CREATE_SUCCESS:
                        case PacketType::APPLICATION_STREAM_CREATE_FAIL:
                        case PacketType::APPLICATION_STREAM_RESPONSE:
                        case PacketType::APPLICATION_STREAM_PING:
                        case PacketType::APPLICATION_STREAM_PONG:
                            // header: streamChannelId
                            header_size = 4;
                            break;
                        case PacketType::CONTROL_PING:
                            responseHandler();
                            return;
                        case PacketType::CONTROL_PONG:
                            LOGD("get Pong");
                            responseHandler();
                            return;
                        default:
                            // todo: tcp protocol between agent and collector has bug(decode ping packet)
                            // so if we get unknown type, just ignore it and stop to read
                            LOGE("get unknown type=%d", type);
                            assert(false);
                            return;
                    }

                    boost::asio::async_read(socket_,
                                            boost::asio::buffer(readBuffer.getWriteAddr(header_size), header_size),
                                            boost::bind(&PinpointClient::handle_read_header, this,
                                                        boost::asio::placeholders::error));
                }
                catch (std::exception &e)
                {
                    LOGE("handle_read_type throw: type=%d, e=%s", responsePacket.type, e.what());
                    return;
                }

            }
            else
            {
                LOGE("handle_read_type error: err=%s", error.message().c_str());
                handle_error(error);
            }
        }

        void PinpointClient::handle_read_header(const boost::system::error_code &error)
        {

            if (!error)
            {
                try
                {
                    int32_t bodyLength;
                    switch (this->responsePacket.type)
                    {
                        case PacketType::APPLICATION_SEND:
                        case PacketType::CONTROL_CLIENT_CLOSE:
                        case PacketType::CONTROL_SERVER_CLOSE:
                            bodyLength = readBuffer.peekPayLoadLength();
                            break;
                        case PacketType::CONTROL_HANDSHAKE_RESPONSE:
                        case PacketType::APPLICATION_RESPONSE:
                        case PacketType::CONTROL_HANDSHAKE:
                        case PacketType::APPLICATION_REQUEST:
                            responsePacket.header = readBuffer.readInt();
                            // body: requestId + payLoad
                            bodyLength = readBuffer.peekPayLoadLength();
                            break;
                        case PacketType::APPLICATION_STREAM_CLOSE:
                        case PacketType::APPLICATION_STREAM_CREATE_FAIL:
                            responsePacket.header = readBuffer.readInt();
                            // body: short code
                            bodyLength = 2;
                            break;
                        case PacketType::APPLICATION_STREAM_CREATE:
                        case PacketType::APPLICATION_STREAM_RESPONSE:
                            responsePacket.header = readBuffer.readInt();
                            // body: payload
                            bodyLength = readBuffer.peekPayLoadLength();
                            break;
                        case PacketType::APPLICATION_STREAM_PING:
                        case PacketType::APPLICATION_STREAM_PONG:
                            responsePacket.header = readBuffer.readInt();
                            // body: int
                            bodyLength = 4;
                            break;
                        case PacketType::APPLICATION_STREAM_CREATE_SUCCESS:
                            responsePacket.header = readBuffer.readInt();
                            responseHandler();
                            return;
                        default:
                            LOGE("get unknown type=%d", this->responsePacket.type);
                            assert(false);
                            return;
                    }

                    boost::asio::async_read(socket_,
                                            boost::asio::buffer(readBuffer.getWriteAddr((uint32_t)bodyLength),
                                                                (size_t)bodyLength),
                                            boost::bind(&PinpointClient::handle_read_body, this,
                                                        boost::asio::placeholders::error));
                }
                catch (std::exception &e)
                {
                    LOGE("handle_read_header throw: type=%d, e=%s", responsePacket.type, e.what());
                    return;
                }
            }
            else
            {
                LOGE("handle_read_header error: err=%s", error.message().c_str());
            }
        }

        void PinpointClient::handle_read_body(const boost::system::error_code &error)
        {

            if (!error)
            {
                try
                {
                    switch (this->responsePacket.type)
                    {
                        case PacketType::APPLICATION_SEND:
                        case PacketType::CONTROL_CLIENT_CLOSE:
                        case PacketType::CONTROL_SERVER_CLOSE:
                            responsePacket.body = readBuffer.readPayLoad();
                            break;
                        case PacketType::CONTROL_HANDSHAKE_RESPONSE:
                        case PacketType::APPLICATION_RESPONSE:
                        case PacketType::CONTROL_HANDSHAKE:
                        case PacketType::APPLICATION_REQUEST:
                            responsePacket.body = readBuffer.readPayLoad();
                            break;
                        case PacketType::APPLICATION_STREAM_CLOSE:
                        case PacketType::APPLICATION_STREAM_CREATE_FAIL:
                            responsePacket.body = readBuffer.readShort();
                            break;
                        case PacketType::APPLICATION_STREAM_CREATE:
                        case PacketType::APPLICATION_STREAM_RESPONSE:
                            responsePacket.body = readBuffer.readPayLoad();
                            break;
                        case PacketType::APPLICATION_STREAM_PING:
                        case PacketType::APPLICATION_STREAM_PONG:
                            responsePacket.body = readBuffer.readInt();
                            break;
                        default:
                            LOGE("get unknown type=%d", this->responsePacket.type);
                            assert(false);
                            return;

                    }

                }
                catch (std::exception &e)
                {
                    LOGE("handle_read_body throw: type=%d, e=%s", responsePacket.type, e.what());
                    return;
                }

                responseHandler();

            }
            else
            {
                LOGE("handle_read_header error: err=%s", error.message().c_str());
            }
        }

        void PinpointClient::responseHandler()
        {
            try
            {
                switch (responsePacket.type)
                {
                    case PacketType::CONTROL_HANDSHAKE_RESPONSE:
                        handShakeSender->responseHandler(responsePacket);
                        if (handShakeSender->getStatus() == HandShakeSender::STATE_FINISHED)
                        {
                            HandshakeResponseCode *handshakeResponseCode =
                                    handShakeSender->getHandshakeResponseCode();
                            if (handshakeResponseCode != NULL)
                            {
                                if (handshakeResponseCode == &HandshakeResponseCode::SUCCESS
                                    || handshakeResponseCode == &HandshakeResponseCode::ALREADY_KNOWN)
                                {
                                    state.toRunSimplex();
                                }
                                else if (handshakeResponseCode == &HandshakeResponseCode::DUPLEX_COMMUNICATION
                                         ||
                                         handshakeResponseCode == &HandshakeResponseCode::ALREADY_DUPLEX_COMMUNICATION)
                                {
                                    //remoteClusterOption = handshaker.getClusterOption();
                                    state.toRunDuplex();
                                }
                                else if (handshakeResponseCode == &HandshakeResponseCode::SIMPLEX_COMMUNICATION
                                         ||
                                         handshakeResponseCode == &HandshakeResponseCode::ALREADY_SIMPLEX_COMMUNICATION)
                                {
                                    state.toRunSimplex();
                                }
                                else
                                {
                                    LOGE("get invalid handshake response code!!!!");
                                }
                            }
                        }
                        break;
                    case PacketType::APPLICATION_RESPONSE:
                        if (requestResponseHandler() != SUCCESS)
                        {
                            LOGE("requestResponseHandler failed");
                        }
                        break;
                    case PacketType::CONTROL_PONG:
                    case PacketType::APPLICATION_STREAM_CREATE:
                    case PacketType::APPLICATION_STREAM_CLOSE:
                    case PacketType::APPLICATION_STREAM_PING:
                    case PacketType::APPLICATION_STREAM_PONG:
                    case PacketType::APPLICATION_STREAM_CREATE_FAIL:
                    case PacketType::APPLICATION_STREAM_CREATE_SUCCESS:
                    case PacketType::APPLICATION_STREAM_RESPONSE:
                    case PacketType::APPLICATION_SEND:
                    case PacketType::CONTROL_HANDSHAKE:
                    case PacketType::CONTROL_SERVER_CLOSE:
                    case PacketType::CONTROL_CLIENT_CLOSE:
                    case PacketType::APPLICATION_REQUEST:
                        // ignore
                        break;
                    case PacketType::CONTROL_PING:
                        // client does not get optional part of ping packet???
                        if (pingPongHandler != NULL)
                        {
                            pingPongHandler->sendPong();
                        }
                        break;

                    default:
                        LOGE("get unknown type=%d", this->responsePacket.type);
                        assert(false);
                }
            }
            catch (std::exception &e)
            {
                LOGE("responseHandler throw: type=%d, %s", responsePacket.type, e.what());
            }

            start_read();

        }

        int32_t PinpointClient::requestResponseHandler()
        {
            PINPOINT_ASSERT_RETURN ((responsePacket.type == PacketType::APPLICATION_RESPONSE), FAILED);

            boost::shared_ptr<TResult> resultPtr;
            int32_t requestId;
            try
            {
                requestId = boost::get<int32_t>(responsePacket.header);

                Pinpoint::Agent::Header header;
                TBasePtr tbasePtr;
                deserializer.reset();
                if (deserializer.deSerializer(header, tbasePtr,
                                              boost::get<std::string>(responsePacket.body)) != Pinpoint::SUCCESS)
                {
                    LOGE("deserializer->deSerializer failed.");
                    return FAILED;
                }

                PINPOINT_ASSERT_RETURN ((header.getType() == DefaultTBaseLocator::RESULT), FAILED);

                resultPtr = boost::dynamic_pointer_cast<TResult>(tbasePtr);

            }
            catch (std::exception& exception)
            {
                LOGE("request response decoder failed. exception=%s", exception.what());
                return FAILED;
            }


            WaitResponseQueue::iterator ip;
            ip = waitResponseQueue.find(requestId);
            if (ip == waitResponseQueue.end())
            {
                LOGW("requestId=%d does not exist", requestId);
                return SUCCESS;
            }

            if (!resultPtr->success)
            {
                PacketPtr packetPtr = ip->second;

                LOGW("requestId=%d failed, message=%s", requestId, resultPtr->message.c_str());
                if (packetPtr->retry())
                {
                    LOGI("retry requestId=%d", requestId);
                    int32_t err = sendQueue.push_front(packetPtr);
                    if (err != SUCCESS)
                    {
                        LOGE("sendQueue.push_front failed! err=%d", err);
                    }
                }
            }

            waitResponseQueue.erase(ip);

            return SUCCESS;
        }

        void PinpointClient::stop()
        {
        	state.toClosedByClient();
        	socket_.cancel();
            socket_.close();
            LOGD("[%s:%d] connection closed",this->DataSender::m_ip.c_str(),this->DataSender::m_port);
            nstate = E_EXIT;
        }

        int32_t PinpointClient::getSocketId()
        {
            return socket_.is_open() ? socket_.native_handle() : -1;
        }

        //</editor-fold>

    }
}
