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
#include "server.h"

using Pinpoint::Agent::PacketType;

UdpServerExecutor::UdpServerExecutor(const std::string &executorName,
                                     uint32_t port,
                                     SERVER_HANDLER handler)
        : Pinpoint::Agent::ThreadExecutor(executorName),
          port(port), socket_(io_service),
          handler(handler)
{

}

UdpServerExecutor::~UdpServerExecutor()
{

}

void UdpServerExecutor::executeTask()
{
    endpoint_ = boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), short(port));
    socket_.open(endpoint_.protocol());
    socket_.bind(endpoint_);
    start_receive();
    io_service.run();
}

void UdpServerExecutor::start_receive()
{
    socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_), remote_endpoint_,
            boost::bind(&UdpServerExecutor::handle_receive, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
}

void UdpServerExecutor::handle_receive(const boost::system::error_code& error,
                    std::size_t /*bytes_transferred*/)
{
    if (!error || error == boost::asio::error::message_size)
    {
        std::string recv(recv_buffer_.begin(), recv_buffer_.end());
        Pinpoint::Agent::PacketPtr snd;
        Pinpoint::Agent::ResponsePacket requestPacket;
        requestPacket.type = Pinpoint::Agent::PacketType::HEADLESS;
        requestPacket.body = recv;

        if (handler)
        {
            handler(requestPacket, snd);
        }

        if (snd != NULL)
        {
            socket_.async_send_to(boost::asio::buffer(snd->getCodedData()), remote_endpoint_,
                                  boost::bind(&UdpServerExecutor::handle_send, this, snd,
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));
        }


    }
    else
    {
        LOGE("UdpServerExecutor::handle_receive error=%d", error.message().c_str())
    }

    start_receive();
}

void UdpServerExecutor::handle_send(Pinpoint::Agent::PacketPtr snd, const boost::system::error_code &error, std::size_t size)
{
    // todo: if erorr, log it.
}

void UdpServerExecutor::stopTask()
{
    if (!io_service.stopped())
    {
        io_service.stop();
    }
}


TcpConnectionPtr TcpConnection::create(
        boost::asio::io_service& io_service,
        SERVER_HANDLER handler)
{
    return TcpConnectionPtr(new TcpConnection(io_service, handler));
}

TcpConnection::TcpConnection(boost::asio::io_service& io_service,
                             SERVER_HANDLER handler)
        : socket_(io_service), closed(false), handler(handler)
{

}

boost::asio::ip::tcp::socket& TcpConnection::socket()
{
    return socket_;
}

TcpConnection::~TcpConnection()
{
    LOGI("call ~TcpConnection");
}

void TcpConnection::start()
{
    try
    {
        boost::asio::socket_base::keep_alive keep_alive(true);
        socket_.set_option(keep_alive);

        boost::asio::socket_base::receive_buffer_size receive_buffer_size(READ_BUFFER_LEN);
        socket_.set_option(receive_buffer_size);

        boost::asio::socket_base::send_buffer_size send_buffer_size(READ_BUFFER_LEN);
        socket_.set_option(send_buffer_size);

        boost::asio::ip::tcp::no_delay no_delay(true);
        socket_.set_option(no_delay);

    }
    catch (std::exception& exception)
    {
        LOGE("get exception = %s", exception.what());
        assert(false);
    }

    start_read();
}

void TcpConnection::close()
{
    socket_.close();
    closed = true;
}

void TcpConnection::start_read()
{
    if (closed)
    {
        return;
    }

#if 0
//    LOG_INFO("start read.")

    readBuffer.reset();
    if (!readBuffer.ensureWriteCapacity(READ_BUFFER_LEN))
    {
        LOGE("ensureWriteCapacity failed.");
        return;
    }

    try
    {
        socket_.async_read_some(boost::asio::buffer(readBuffer.getWriteAddr(), READ_BUFFER_LEN),
                                boost::bind(&TcpConnection::handle_read, shared_from_this(),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred)
        );
    }
    catch (...)
    {
        // todo
    }
#endif
    readBuffer.reset();
    if (!readBuffer.ensureWriteCapacity(READ_BUFFER_LEN))
    {
        LOGE("ensureWriteCapacity failed.");
        return;
    }

    try
    {
        boost::asio::async_read(socket_,
                                boost::asio::buffer(readBuffer.getWriteAddr(TCP_RESPONSE_HEADER_LEN), TCP_RESPONSE_HEADER_LEN),
                                boost::bind(&TcpConnection::handle_read_type, shared_from_this(),
                                            boost::asio::placeholders::error));
    }
    catch (...)
    {
        // todo
    }
}


//void TcpConnection::handle_read(const boost::system::error_code &ec, std::size_t bytes_transferred)
//{
//    if (closed)
//    {
//        return;
//    }
//
//    if (!ec)
//    {
//        readBuffer.updateWritePos(bytes_transferred);
//
//        if (readBuffer.getReadCapacity() >= TCP_RESPONSE_HEADER_LEN)
//        {
//            int16_t type = readBuffer.readShort();
//            responsePacket.type = type;
//            LOG_INFO("tcp get packet, type=%d", type);
//            try
//            {
//                switch (type)
//                {
//                    case Pinpoint::Agent::PacketType::CONTROL_HANDSHAKE:
//                        LOG_INFO("get handshake");
//                        break;
//                    case Pinpoint::Agent::PacketType::APPLICATION_REQUEST:
//                        responsePacket.header.messageId = readBuffer.readInt();
//                        responsePacket.payLoad = readBuffer.readPayLoad();
//                        break;
//                    case Pinpoint::Agent::PacketType::CONTROL_PING:
//                        LOG_INFO("get ping");
//                        break;
//                    case Pinpoint::Agent::PacketType::CONTROL_PONG:
//                        LOG_INFO("get Pong");
//                        goto read_next_packet;
//                        break;
//                    case Pinpoint::Agent::PacketType::APPLICATION_STREAM_CLOSE:
//                    case Pinpoint::Agent::PacketType::APPLICATION_STREAM_CREATE:
//                    case Pinpoint::Agent::PacketType::APPLICATION_STREAM_CREATE_FAIL:
//                    case Pinpoint::Agent::PacketType::APPLICATION_STREAM_CREATE_SUCCESS:
//                    case Pinpoint::Agent::PacketType::APPLICATION_STREAM_PING:
//                    case Pinpoint::Agent::PacketType::APPLICATION_STREAM_PONG:
//                    case Pinpoint::Agent::PacketType::APPLICATION_STREAM_RESPONSE:
//                    case Pinpoint::Agent::PacketType::APPLICATION_SEND:
//                    case Pinpoint::Agent::PacketType::APPLICATION_TRACE_SEND:
//                    case Pinpoint::Agent::PacketType::APPLICATION_TRACE_SEND_ACK:
//                        // no support, just ignore
//                        goto read_next_packet;
//                        break;
//                    default:
//                        LOG_ERROR("get error type=%d", type);
//                        assert (false);
//                        goto read_next_packet;
//                }
//
//            }
//            catch (std::exception& exception)
//            {
//                LOG_ERROR("decode error: err=%s", exception.what());
//                goto read_next_packet;
//            }
//
//            Pinpoint::Agent::PacketPtr snd;
//            try
//            {
//                handler(responsePacket, snd);
//            }
//            catch (std::exception& exception)
//            {
//                LOG_ERROR("responseHandler throw: exception=%s", exception.what());
//                goto read_next_packet;
//            }
//
//            if (snd != NULL)
//            {
//
//                boost::asio::async_write(socket_,
//                                         boost::asio::buffer(snd->getCodedData()),
//                                         boost::bind(&TcpConnection::handle_write, shared_from_this(),
//                                                     boost::asio::placeholders::error));
//            }
//        }
//        else
//        {
//            LOG_ERROR("packet len < type len");
//        }
//    }
//    else
//    {
//        LOG_ERROR("read error, err=%s", ec.message().c_str());
//    }
//
//read_next_packet:
//    start_read();
//}

void TcpConnection::handle_read_type(const boost::system::error_code &error)
{
    if (closed)
    {
        return;
    }

    if (!error)
    {
        int16_t type = readBuffer.readShort();
//                memset(&this->responsePacket, 0, sizeof(ResponsePacket));
        responsePacket.type = type;
        uint32_t header_size = 0;
        switch (type)
        {
            case PacketType::CONTROL_HANDSHAKE:
            case PacketType::APPLICATION_RESPONSE:
            case PacketType::APPLICATION_REQUEST:
                // header: requestId + payLoad
                header_size= 4 + 4;
                break;
            case PacketType::APPLICATION_STREAM_CLOSE:
                // header: streamChannelId
                header_size = 4;
                break;
            case PacketType::CONTROL_PING:
                // client does not get optional part of ping packet???
                // todo: send pong
                header_size = 6;
                break;
            case PacketType::CONTROL_PONG:
                LOGI("get Pong");
                start_read();
                return;
            default:
                // todo: tcp protocol between agent and collector has bug(decode ping packet)
                // so if we get unknown type, just ignore it and stop to read
                LOGE("get unknown type=%d", type);
//                        assert(false);
                return;
        }

        boost::asio::async_read(socket_,
                                boost::asio::buffer(readBuffer.getWriteAddr(header_size), header_size),
                                boost::bind(&TcpConnection::handle_read_header, shared_from_this(),
                                            boost::asio::placeholders::error));
    }
    else
    {
        LOGE("handle_read_type error: err=%s", error.message().c_str());
    }
}

void TcpConnection::handle_read_header(const boost::system::error_code &error)
{
    if (closed)
    {
        return;
    }

    if (!error)
    {
        int32_t bodyLength;
        Pinpoint::Agent::PingPacket pingPacket;
        switch (this->responsePacket.type)
        {
            case PacketType::CONTROL_HANDSHAKE:
            case PacketType::APPLICATION_RESPONSE:
            case PacketType::APPLICATION_REQUEST:
                // body: requestId + payLoad
                responsePacket.header = readBuffer.readInt();
                bodyLength = readBuffer.peekPayLoadLength();

                break;
            case PacketType::APPLICATION_STREAM_CLOSE:
                responsePacket.header = readBuffer.readInt();
                // body: short code
                bodyLength = 2;
                break;
            case PacketType::CONTROL_PING:
                pingPacket.pingId = readBuffer.readInt();
                pingPacket.stateVersion = readBuffer.readByte();
                pingPacket.stateCode = readBuffer.readByte();
                LOGD("get ping: pingId=%d, stateVersion=%d, stateCode=%d",
                          pingPacket.pingId, pingPacket.stateVersion, pingPacket.stateCode);
                start_read();
                return;
            default:
                LOGE("get unknown type=%d", this->responsePacket.type);
                assert(false);
                return;
        }

        boost::asio::async_read(socket_,
                                boost::asio::buffer(readBuffer.getWriteAddr(bodyLength), bodyLength),
                                boost::bind(&TcpConnection::handle_read_body, shared_from_this(),
                                            boost::asio::placeholders::error));
    }
    else
    {
        LOGE("handle_read_header error: err=%s", error.message().c_str());
    }
}

void TcpConnection::handle_read_body(const boost::system::error_code &error)
{
    if (closed)
    {
        return;
    }

    if (!error)
    {
        switch (this->responsePacket.type)
        {
            case PacketType::CONTROL_HANDSHAKE:
            case PacketType::APPLICATION_RESPONSE:
            case PacketType::APPLICATION_REQUEST:
                try
                {
                    this->responsePacket.body = readBuffer.readPayLoad();
                }
                catch (Pinpoint::Agent::ChannelBufferException &e)
                {
                    LOGE("ChannelBufferException throw!!!");
                    return;
                }
                break;
            case PacketType::APPLICATION_STREAM_CLOSE:
                responsePacket.body = readBuffer.readShort();
                break;
            default:
                LOGE("get unknown type=%d", this->responsePacket.type);
                assert(false);
                return;

        }

        Pinpoint::Agent::PacketPtr snd;
        try
        {
            handler(responsePacket, snd);
        }
        catch (std::exception& exception)
        {
            LOGE("responseHandler throw: exception=%s", exception.what());
        }

        if (snd != NULL)
        {

            boost::asio::async_write(socket_,
                                     boost::asio::buffer(snd->getCodedData()),
                                     boost::bind(&TcpConnection::handle_write, shared_from_this(),
                                                 boost::asio::placeholders::error));
        }

        start_read();
    }
    else
    {
        LOGE("handle_read_header error: err=%s", error.message().c_str());
    }
}

void TcpConnection::handle_write(const boost::system::error_code& /*error*/)
{

}

TcpServerExecutor::TcpServerExecutor(const std::string &executorName, uint32_t port, SERVER_HANDLER handler)
        : Pinpoint::Agent::ThreadExecutor(executorName), port(port), handler(handler),
          acceptor_(io_service)
{

}

TcpServerExecutor::~TcpServerExecutor()
{

}

void TcpServerExecutor::executeTask()
{
    endpoint_ = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), short(port));
    acceptor_.open(endpoint_.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint_);
    acceptor_.listen();
    start_accept();
    io_service.run();
}

void TcpServerExecutor::stopTask()
{
    if (!io_service.stopped())
    {
        io_service.stop();
    }
}

void TcpServerExecutor::start_accept()
{
    TcpConnectionPtr new_connection =
            TcpConnection::create(io_service, handler);

    acceptor_.async_accept(new_connection->socket(),
                           boost::bind(&TcpServerExecutor::handle_accept, this, new_connection,
                                       boost::asio::placeholders::error));
}

void TcpServerExecutor::handle_accept(TcpConnectionPtr new_connection,
                   const boost::system::error_code& error)
{
    if (!error)
    {
        new_connection->start();
    }
    else
    {
        LOGE("handle_accept failed: err=%s", error.message().c_str());
    }

    start_accept();
}
