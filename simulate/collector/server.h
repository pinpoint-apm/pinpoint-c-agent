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
#ifndef PINPOINT_SIMULATE_SERVER_H
#define PINPOINT_SIMULATE_SERVER_H

#include <data_sender.h>
#include "executor.h"
#include "buffer.h"

typedef enum
{
    DECODER_SUCCESS = 1,
    DECODER_INCOMPLETED,
    DECODER_FAILURE
} DECODER_STATUS;

typedef void (*SERVER_HANDLER)(const Pinpoint::Agent::ResponsePacket& requestPacket, Pinpoint::Agent::PacketPtr& snd);

class UdpServerExecutor : public Pinpoint::Agent::ThreadExecutor
{
public:
    UdpServerExecutor(const std::string &executorName,
                      uint32_t port, SERVER_HANDLER handler);

    virtual ~UdpServerExecutor();

private:

    virtual void executeTask();

    virtual void stopTask();

    void start_receive();
    void handle_receive(const boost::system::error_code& error,
                        std::size_t /*bytes_transferred*/);
    void handle_send(Pinpoint::Agent::PacketPtr snd /*message*/,
                     const boost::system::error_code& /*error*/,
                     std::size_t /*bytes_transferred*/);
    uint32_t port;
    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint endpoint_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
    boost::array<char, 4094> recv_buffer_;
    SERVER_HANDLER handler;
};

class TcpConnection;

typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

class TcpConnection : public boost::enable_shared_from_this<TcpConnection>
{
public:
    static const int32_t READ_LEN = 24;
    static TcpConnectionPtr create(boost::asio::io_service& io_service,
                                   SERVER_HANDLER handler);
    void start();
    boost::asio::ip::tcp::socket& socket();
    ~TcpConnection();
private:
    Pinpoint::Agent::ChannelBufferV2 readBuffer;
    Pinpoint::Agent::ResponsePacket responsePacket;
    static const uint32_t READ_BUFFER_LEN = 1024 * 64;
    static const uint32_t TCP_RESPONSE_HEADER_LEN = 2;
    void close();
    void start_read();
    void handle_read_type(const boost::system::error_code& error);
    void handle_read_header(const boost::system::error_code& error);
    void handle_read_body(const boost::system::error_code& error);
    //void handle_read(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void handle_write(const boost::system::error_code&);
    TcpConnection(boost::asio::io_service& io_service,
                  SERVER_HANDLER handler);
    boost::asio::ip::tcp::socket socket_;
//    boost::asio::streambuf input_buffer_;
    bool closed;
    SERVER_HANDLER handler;
    Pinpoint::Agent::ChannelBufferV2 buffer;
};

class TcpServerExecutor : public Pinpoint::Agent::ThreadExecutor
{
public:
    TcpServerExecutor(const std::string &executorName,
                      uint32_t port, SERVER_HANDLER handler);

    virtual ~TcpServerExecutor();

private:

    virtual void executeTask();

    virtual void stopTask();

    void start_accept();

    void handle_accept(TcpConnectionPtr new_connection,
                       const boost::system::error_code& error);

    uint32_t port;
    SERVER_HANDLER handler;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::endpoint endpoint_;
};

#endif
