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
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "test_env.h"
#include "executor.h"
#include "data_sender.h"
#include "test_server.h"
#include <string>
#include <boost/lexical_cast.hpp>
#include "pinpoint_api.h"

using namespace std;
using namespace Pinpoint::log;
using namespace Pinpoint::Agent;

#if 0
TEST(tcp_sender_test, basic_test)
{
    const uint32_t testServerPort = 12345;
    const uint32_t testSetLen = 10;
    const uint32_t wait_sec = testSetLen * 2;

    vector<int32_t> testSet;
    random_vector(testSet, testSetLen);

    boost::shared_ptr<TestDataSet> testDataSetPtr(new TestDataSetImp(testSet));

    TestTcpServer server("testTcpServer", testServerPort, testDataSetPtr);
    server.start();

    TcpDataSender sender("tcpDataSender", "127.0.0.1", testServerPort);
    sender.start();

    boost::shared_ptr<Packet> packetPtr;
    for (uint32_t i = 0; i < testSetLen; ++i)
    {
        string data = boost::lexical_cast<string>(testSet[i]);
        data += '\n';
        packetPtr.reset(new Packet(data, 100));
        if (probability(50))
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(2*TcpDataSender::MAX_REFRESH_MSEC));
        }

        // simulate to disconnect
        if (probability(30))
        {
            LOGI("simulate to disconnect.");
            sender.getSocket().close();
        }

        sender.sendPacket(packetPtr, -1);
    }

    boost::this_thread::sleep(boost::posix_time::seconds(wait_sec));
    sender.stop();
    server.stop();
}
#endif

TEST(udp_sender_test, basic_test)
{
    const uint32_t testServerPort = 12346;
    const uint32_t testSetLen = 10;
//    const uint32_t wait_sec = testSetLen * 1;

    vector<int32_t> testSet;
    random_vector(testSet, testSetLen);

    boost::shared_ptr<TestDataSet> testDataSetPtr(new TestDataSetImp(testSet));

    TestUdpServer server("testUdpServer", testServerPort, testDataSetPtr);
    server.start();

    uint32_t availablePort = 0;
    while (availablePort == 0)
    {
        availablePort = server.getPort();
    }

    LOGI("availablePort=%d", availablePort);

    UdpDataSender sender("udpDataSender", "127.0.0.1", availablePort);
    sender.start();

    boost::shared_ptr<Packet> packetPtr;
    for (uint32_t i = 0; i < testSetLen; ++i)
    {
        string data = boost::lexical_cast<string>(testSet[i]);
        packetPtr.reset(new Packet(PacketType::HEADLESS, 100));
        PacketData& packetData = packetPtr->getPacketData();
        packetData = data;
        packetPtr->setCodedData(boost::get<std::string>(packetPtr->getPacketData()));
        if (probability(50))
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(2*UdpDataSender::MAX_REFRESH_MSEC));
        }

        sender.sendPacket(packetPtr, -1);
    }

    // stop msg
    string stopData;
    stopData += STOP_MSG;
    packetPtr.reset(new Packet(PacketType::HEADLESS, 100));
    PacketData& packetData = packetPtr->getPacketData();
    packetData = stopData;
    sender.sendPacket(packetPtr, -1);

//    boost::this_thread::sleep(boost::posix_time::seconds(wait_sec));
    server.join();
    sender.stop();
}
