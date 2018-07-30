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
#include "test_server.h"
#include "test_env.h"
#include "pinpoint_api.h"

using namespace std;
using namespace Pinpoint::log;


TcpConnection::pointer TcpConnection::create(
        boost::asio::io_service &io_service,
        boost::shared_ptr<TestDataSet>& dataSetPtr,
        boost::shared_ptr<TestTcpServer>& testTcpServer)
{
    return pointer(new TcpConnection(io_service, dataSetPtr, testTcpServer));
}

TcpConnection::TcpConnection(boost::asio::io_service& io_service,
                             boost::shared_ptr<TestDataSet>& dataSetPtr,
                             boost::shared_ptr<TestTcpServer>& testTcpServer)
        : socket_(io_service), closed(false), dataSetPtr(dataSetPtr), testTcpServer(testTcpServer)
{

}

boost::asio::ip::tcp::socket& TcpConnection::socket()
{
    return socket_;
}

void TcpConnection::start(pointer self)
{
    m_self = self; // keep shared_ptr
//    boost::asio::socket_base::linger option(true, 30);
//    socket_.set_option(option);
    start_read();
}

void TcpConnection::close()
{
    socket_.close();
    closed = true;
    m_self.reset(); // free shared_ptr;
}

void TcpConnection::start_read()
{
    if (closed)
    {
        return;
    }

    LOGI("start read.")


    // Start an asynchronous operation to read a newline-delimited message.
    boost::asio::async_read_until(socket_, input_buffer_, '\n',
                                  boost::bind(&TcpConnection::handle_read, this, _1));

}


void TcpConnection::handle_read(const boost::system::error_code &ec)
{
    if (closed)
        return;

    if (!ec)
    {
        // Extract the newline-delimited message from the buffer.
        std::string line;
        std::istream is(&input_buffer_);
        std::getline(is, line);

        LOGI("Received: %s", line.c_str());

        if (line == STOP_MSG)
        {
            socket_.close();
            this->testTcpServer->stopServer();
        }

        if (!line.empty())
        {
            dataSetPtr->receive(line);
            LOGI("Received: %s", line.c_str());
        }

        start_read();

    }
    else
    {

        if ((boost::asio::error::eof == ec) ||
            (boost::asio::error::connection_reset == ec))
        {
            LOGE("Read failed, error=%d, msg=%s", ec.value(), ec.message().c_str());
            LOGI("Connection broken.");
            this->close();
        }
    }

}


TestTcpServer::TestTcpServer(const std::string& name, uint32_t port,
                       boost::shared_ptr<TestDataSet> &dataSetPtr)
        : ThreadExecutor(name), m_port(port), findAvailablePortCompleted(false), io_service(), dataSetPtr(dataSetPtr),
          acceptor_(io_service)

{

}

volatile uint32_t TestTcpServer::getPort()
{
    return findAvailablePortCompleted ? this->m_port : 0;
}

void TestTcpServer::stopServer()
{
    stopTask();
}

void TestTcpServer::executeTask()
{
    // find available port
    for (int i = 1; i <= 100; i++)
    {
        try
        {
            endpoint_ = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), short(m_port));
            acceptor_.open(endpoint_.protocol());
            acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            acceptor_.bind(endpoint_);
            acceptor_.listen();
            break;
        }
        catch (std::exception& e)
        {
            LOGW("TestTcpServer::executeTask: e=%s", e.what());

            try
            {
                acceptor_.close();
            }
            catch (...)
            {

            }

            if (i == 100)
            {
                throw e;
            }

            m_port++;
        }
    }

    findAvailablePortCompleted = true;

    // acceptor_ = boost::asio::ip::tcp::acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), short(m_port)))

    start_accept();
    io_service.run();
}

void TestTcpServer::start_accept()
{
    boost::shared_ptr<TestTcpServer> testTcpServerPtr = shared_from_this();
    TcpConnection::pointer new_connection =
            TcpConnection::create(acceptor_.get_io_service(), dataSetPtr, testTcpServerPtr);

    acceptor_.async_accept(new_connection->socket(),
                           boost::bind(&TestTcpServer::handle_accept, this, new_connection,
                                       boost::asio::placeholders::error));

}

void TestTcpServer::handle_accept(TcpConnection::pointer new_connection,
                               const boost::system::error_code &error)
{
    if (!error)
    {
        LOGI("accept success!");
        new_connection->start(new_connection);
        start_accept();
    }
    else
    {
        LOGE("accept failed, error=%d, msg=%s", error.value(), error.message().c_str());
    }
}

void TestTcpServer::stopTask()
{
    io_service.stop();
    acceptor_.close();
    dataSetPtr->check();
}


TestUdpServer::TestUdpServer(const std::string &name, uint32_t port,
                             boost::shared_ptr<TestDataSet> &dataSetPtr)
        : ThreadExecutor(name), m_port(port), findAvailablePortCompleted(false), io_service(),
          dataSetPtr(dataSetPtr), socket_(io_service)
{

}

void TestUdpServer::executeTask()
{
    // find available port
    for (int i = 1; i <= 100; i++)
    {
        try
        {
            endpoint_ = boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), short(m_port));
            socket_.open(endpoint_.protocol());
            socket_.bind(endpoint_);
            break;
        }
        catch (std::exception& e)
        {

            try
            {
                socket_.close();
            }
            catch (...)
            {

            }

            if (i == 100)
            {
                throw e;
            }

            m_port++;
        }
    }

    findAvailablePortCompleted = true;

    start_receive();
    io_service.run();
}

volatile uint32_t TestUdpServer::getPort()
{
    return findAvailablePortCompleted ? this->m_port : 0;
}

void TestUdpServer::stopTask()
{
    io_service.stop();
    socket_.close();
    dataSetPtr->check();
}

void TestUdpServer::start_receive()
{
    socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_), remote_endpoint_,
            boost::bind(&TestUdpServer::handle_receive, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
}

void TestUdpServer::handle_receive(const boost::system::error_code &error, std::size_t len)
{
    if (!error || error == boost::asio::error::message_size)
    {
        // Extract the newline-delimited message from the buffer.
        std::string line(recv_buffer_.data(), len);

        LOGI("Received: %s", line.c_str());

        if (line == STOP_MSG)
        {
            this->stopTask();
        }

        if (!line.empty())
        {
            dataSetPtr->receive(line);
            LOGI("Received: %s, len=%d", line.c_str(), line.size());
        }


        start_receive();
    }
}


TestDataSetImp::TestDataSetImp(vector<int32_t> &testData)
        : origin(testData)

{
    dist.reserve(testData.size());
}

void TestDataSetImp::receive(const std::string &line)
{
    int32_t d = atoi(line.c_str());
    dist.push_back(d);
}

void TestDataSetImp::check()
{
    // if udp, order maybe changed
    check_vector_ignore_order(origin, dist);
}
