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
#include "pinpoint_client.h"

using namespace std;
using namespace Pinpoint::log;
using namespace Pinpoint::Agent;

TEST(tcp_sender2_test, basic_test)
{
    const uint32_t testServerPort = 12345;
    const uint32_t testSetLen = 10;
//    const uint32_t wait_sec = testSetLen * 5;

    vector<int32_t> testSet;
    random_vector(testSet, testSetLen);

    boost::shared_ptr<TestDataSet> testDataSetPtr(new TestDataSetImp(testSet));

    boost::shared_ptr<TestTcpServer> server(new TestTcpServer("testTcpServer", testServerPort, testDataSetPtr));
    server->start();

    uint32_t availablePort = 0;
    while (availablePort == 0)
    {
        availablePort = server->getPort();
    }

    LOGI("availablePort=%d", availablePort);

    boost::shared_ptr<ScheduledExecutor> scheduledExecutorPtr(new ScheduledExecutor("schedule"));

    PinpointClientPtr pinpointClientPtr(new PinpointClient(
            "pinpoint client", "127.0.0.1", availablePort, scheduledExecutorPtr,10));

    pinpointClientPtr->start();

    while(!SocketStateCodeOP::isRun(pinpointClientPtr->getState()->getCurrentState()));

    // simulate to disconnect
    LOGI("simulate to disconnect.");
    pinpointClientPtr->getSocket().close();


    boost::shared_ptr<Packet> packetPtr;
    for (uint32_t i = 0; i < testSetLen; ++i)
    {
        string data = boost::lexical_cast<string>(testSet[i]);
        data += '\n';
        packetPtr.reset(new Packet(PacketType::HEADLESS, 100));
        PacketData& packetData = packetPtr->getPacketData();
        packetData = data;
        packetPtr->setCodedData(boost::get<std::string>(packetPtr->getPacketData()));
        if (probability(50))
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(2* 1000));
        }

        pinpointClientPtr->sendPacket(packetPtr, -1);
        LOGI("send:%s", data.c_str());
    }

    // stop msg
    string stopData;
    stopData += STOP_MSG;
    stopData += "\n";
    packetPtr.reset(new Packet(PacketType::HEADLESS, 100));
    PacketData& packetData = packetPtr->getPacketData();
    packetData = stopData;
    pinpointClientPtr->sendPacket(packetPtr, -1);
    LOGI("send:%s", stopData.c_str());

//    boost::this_thread::sleep(boost::posix_time::seconds(wait_sec));

    server->join();
    pinpointClientPtr->stop();
}

//TEST(udp_sender_test, basic_test)
//{
//    const uint32_t testServerPort = 12346;
//    const uint32_t testSetLen = 10;
//    const uint32_t wait_sec = testSetLen * 1;
//
//    vector<int32_t> testSet;
//    random_vector(testSet, testSetLen);
//
//    boost::shared_ptr<TestDataSet> testDataSetPtr(new TestDataSetImp(testSet));
//
//    TestUdpServer server("testUdpServer", testServerPort, testDataSetPtr);
//    server.start();
//
//    UdpDataSender sender("udpDataSender", "127.0.0.1", testServerPort);
//    sender.start();
//
//    boost::shared_ptr<Packet> packetPtr;
//    for (uint32_t i = 0; i < testSetLen; ++i)
//    {
//        string data = boost::lexical_cast<string>(testSet[i]);
//        data += '\n';
//        packetPtr.reset(new Packet(data, 100));
//        if (probability(50))
//        {
//            boost::this_thread::sleep(boost::posix_time::milliseconds(2*TcpDataSender::MAX_REFRESH_MSEC));
//        }
//
//        sender.sendPacket(packetPtr, -1);
//    }
//
//    boost::this_thread::sleep(boost::posix_time::seconds(wait_sec));
//    sender.stop();
//    server.stop();
//}
