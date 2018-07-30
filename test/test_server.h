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
#ifndef PINPOINT_TEST_SERVER_H
#define PINPOINT_TEST_SERVER_H

#include <string>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include "executor.h"


const char STOP_MSG[] = "STOP";

class TestDataSet
{
public:
    virtual void receive(const std::string& line) = 0;
    virtual void check() = 0;
    virtual ~TestDataSet(){};
};

class TestTcpServer;
class TcpConnection
{
public:
    typedef boost::shared_ptr<TcpConnection> pointer;
    static pointer create(boost::asio::io_service& io_service,
                          boost::shared_ptr<TestDataSet>& dataSetPtr,
                          boost::shared_ptr<TestTcpServer>& testTcpServer);
    void start(pointer self);
    boost::asio::ip::tcp::socket& socket();
private:
    void close();
    void start_read();
    void handle_read(const boost::system::error_code& ec);
    TcpConnection(boost::asio::io_service& io_service,
                  boost::shared_ptr<TestDataSet>& dataSetPtr,
                  boost::shared_ptr<TestTcpServer>& testTcpServer);
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf input_buffer_;
    bool closed;
    boost::shared_ptr<TestDataSet> dataSetPtr;
    boost::shared_ptr<TestTcpServer> testTcpServer;
    pointer m_self;
};


class TestTcpServer : public Pinpoint::Agent::ThreadExecutor, public boost::enable_shared_from_this<TestTcpServer>
{
public:
    TestTcpServer(const std::string& name, uint32_t port,
               boost::shared_ptr<TestDataSet>& dataSetPtr);
    volatile uint32_t getPort();
    void stopServer();
private:
    volatile uint32_t m_port;
    volatile bool findAvailablePortCompleted;
    boost::asio::io_service io_service;
    boost::shared_ptr<TestDataSet> dataSetPtr;
    boost::asio::ip::tcp::acceptor acceptor_;
    // boost::shared_ptr<boost::asio::ip::tcp::endpoint> endpointPtr;
    boost::asio::ip::tcp::endpoint endpoint_;
    void executeTask();
    void stopTask();
    void start_accept();
    void handle_accept(TcpConnection::pointer new_connection,
                       const boost::system::error_code& error);
};


class TestUdpServer : public Pinpoint::Agent::ThreadExecutor
{
public:
    TestUdpServer(const std::string& name, uint32_t port,
                  boost::shared_ptr<TestDataSet>& dataSetPtr);
    volatile uint32_t getPort();
    static const uint32_t READ_BUFFER_SIZE = 1024;
private:
    volatile uint32_t m_port;
    volatile bool findAvailablePortCompleted;
    boost::asio::io_service io_service;
    boost::shared_ptr<TestDataSet> dataSetPtr;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint endpoint_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
    boost::array<char, READ_BUFFER_SIZE> recv_buffer_;

    void executeTask();
    void stopTask();

    void start_receive();
    void handle_receive(const boost::system::error_code& error,
                        std::size_t /*bytes_transferred*/);
};

class TestDataSetImp : public TestDataSet
{
public:
    TestDataSetImp(std::vector<int32_t>& testData);
    void receive(const std::string& line);
    void check();
private:
    std::vector<int32_t> origin;
    std::vector<int32_t> dist;
};


#endif
