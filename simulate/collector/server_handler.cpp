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
#include <data_sender.h>
#include "server_handler.h"
#include "serializer.h"
#include "utility.h"
#include "collector_context.h"
#include "tbase_op.h"
#include "test_case.h"
#include "simulate_def.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <thrift/protocol/TJSONProtocol.h>

static void pinpoint_request_handler(const Pinpoint::Agent::ResponsePacket& requestPacket,
                                     Pinpoint::Agent::PacketPtr& snd);

static void tbase_handler(const Pinpoint::Agent::Header& header,
                          const boost::shared_ptr<apache::thrift::TBase>& tbasePtr);

static void output_trace(const boost::shared_ptr<apache::thrift::TBase>& tbasePtr);
static void output_api(const boost::shared_ptr<apache::thrift::TBase>& tbasePtr);
static void output_string(const boost::shared_ptr<apache::thrift::TBase>& tbasePtr);
static void output_once(const Pinpoint::Agent::Header& header,
                        const boost::shared_ptr<apache::thrift::TBase>& tbasePtr);


void tbase_handler(const Pinpoint::Agent::Header& header,
                   const boost::shared_ptr<apache::thrift::TBase>& tbasePtr)
{
    switch(header.getType())
    {
        case Pinpoint::Agent::DefaultTBaseLocator::SPAN:
            output_trace(tbasePtr);
            return;
        case Pinpoint::Agent::DefaultTBaseLocator::API_META_DATA:
            output_api(tbasePtr);
            return;
        case Pinpoint::Agent::DefaultTBaseLocator::STRING_META_DATA:
            output_string(tbasePtr);
            return;
        default:
            output_once(header, tbasePtr);
            return;
    }
}


void pinpoint_udp_handler(const Pinpoint::Agent::ResponsePacket& requestPacket, Pinpoint::Agent::PacketPtr& snd)
{
    static boost::thread_specific_ptr<Pinpoint::Agent::HeaderTBaseDeSerializer> deserializer;

    if (deserializer.get() == NULL)
    {
        try
        {
            deserializer.reset(new Pinpoint::Agent::HeaderTBaseDeSerializer);
        }
        catch (std::bad_alloc&)
        {
            LOGE("new Pinpoint::Agent::HeaderTBaseDeSerializer failed");
            return;
        }
    }

    Pinpoint::Agent::Header header;
    boost::shared_ptr<apache::thrift::TBase> tbasePtr;
    if (deserializer->deSerializer(header, tbasePtr,
                                   boost::get<std::string>(requestPacket.body)) != Pinpoint::SUCCESS)
    {
        LOGE("deserializer->deSerializer failed.");
        return;
    }

    tbase_handler(header, tbasePtr);

}

void pinpoint_tcp_handler(const Pinpoint::Agent::ResponsePacket& requestPacket, Pinpoint::Agent::PacketPtr& snd)
{
    int16_t packetType = requestPacket.type;

    switch(packetType)
    {
        case Pinpoint::Agent::PacketType::APPLICATION_REQUEST:
            pinpoint_request_handler(requestPacket, snd);
            break;
        default:
            return;
    }
}

void pinpoint_request_handler(const Pinpoint::Agent::ResponsePacket& requestPacket, Pinpoint::Agent::PacketPtr&  snd)
{
    static boost::thread_specific_ptr<Pinpoint::Agent::HeaderTBaseDeSerializer> deserializer;

    if (deserializer.get() == NULL)
    {
        try
        {
            deserializer.reset(new Pinpoint::Agent::HeaderTBaseDeSerializer);
        }
        catch (std::bad_alloc&)
        {
            LOGE("new Pinpoint::Agent::HeaderTBaseDeSerializer failed");
            return;
        }
    }

    Pinpoint::Agent::Header header;
    boost::shared_ptr<apache::thrift::TBase> tbasePtr;
    if (deserializer->deSerializer(header, tbasePtr,
                                   boost::get<std::string>(requestPacket.body)) != Pinpoint::SUCCESS)
    {
        LOGE("deserializer->deSerializer failed.");
        return;
    }

    tbase_handler(header, tbasePtr);

    try
    {
        snd.reset(new Pinpoint::Agent::Packet(Pinpoint::Agent::PacketType::APPLICATION_RESPONSE, 1));
        Pinpoint::Agent::PacketData& packetData = snd->getPacketData();

        TResult* rp = new TResult;
        rp->__set_success(true);
        rp->__set_message("success!");
        TBasePtr tBasePtr;
        tBasePtr.reset(rp);
        packetData = tbasePtr;

        // encode

        int32_t requestId = boost::get<int32_t>(requestPacket.header);

        std::string data;
        int32_t err;

        Pinpoint::Agent::HeaderTBaseSerializer serializer;

        err = serializer.serializer(*tBasePtr, data);
        if (err != Pinpoint::SUCCESS)
        {
            LOGE("serializer tBase failed");
            return;
        }
        // add request header
        Pinpoint::Agent::ChannelBufferV2 buffer;
        buffer.writeShort(Pinpoint::Agent::PacketType::APPLICATION_RESPONSE);
        buffer.writeInt(requestId);
        buffer.appendPayload(data);

        std::string codedData = buffer.getBuffer();
        snd->setCodedData(codedData);

    }
    catch (std::exception& exception)
    {
        LOGE("pinpoint_request_handler create response failed: exception=%s", exception.what());
    }

}


void output_trace(const boost::shared_ptr<apache::thrift::TBase>& tbasePtr)
{
    std::string testCasePathStr;
    std::string rawFileName;

    TSpanPtr tSpanPtr = boost::dynamic_pointer_cast<TSpan>(tbasePtr);

    int32_t err = TBaseOP::getTSpanTestcasePath(tSpanPtr, testCasePathStr);
    if (err != Pinpoint::SUCCESS)
    {
        LOGE("getTSpanTestcasePath failed");
        return;
    }

    rawFileName = TBaseOP::getTSpanFileRawName(tbasePtr);

    TBaseOP::writeTbaseToJsonFile(boost::filesystem::path(testCasePathStr) /= boost::filesystem::path(rawFileName + RESULT_EXT_NAME),
                                  *tbasePtr);
}

void output_once(const Pinpoint::Agent::Header& header,
                 const boost::shared_ptr<apache::thrift::TBase>& tbasePtr)
{
    static boost::atomic_int agentInfoOutputCount(0);
    static const boost::filesystem::path agentInfoPath = TestCase::getResultFilePath(AGENT_INFO_PATH, AGENT_INFO_NAME);

    static boost::atomic_int agentStatBatchOutputCount(0);
    static const boost::filesystem::path agentStatBatchPath = TestCase::getResultFilePath(AGENT_STAT_BATCH_PATH, AGENT_STAT_BATCH_NAME);

    boost::filesystem::path testCasePath(CollectorContext::getContext().getTestCasePath());

    int count = 0;

    switch(header.getType())
    {
        case Pinpoint::Agent::DefaultTBaseLocator::AGENT_INFO:
            count = agentInfoOutputCount.fetch_add(1);
            if (count > 0)
            {
                return;
            }
            TBaseOP::writeTbaseToJsonFile(testCasePath /= agentInfoPath, *tbasePtr);
            break;
        case Pinpoint::Agent::DefaultTBaseLocator::AGENT_STAT_BATCH:
            count = agentStatBatchOutputCount.fetch_add(1);
            if (count > 0)
            {
                return;
            }
            TBaseOP::writeTbaseToJsonFile(testCasePath /= agentStatBatchPath, *tbasePtr);
            break;
        default:
            return;

    }
}

void output_api(const TBasePtr& tbasePtr)
{
    boost::filesystem::path testCasePath(CollectorContext::getContext().getTestCasePath());
    boost::filesystem::path apiInfoPath =
            TestCase::getResultFilePath(API_META_DATA_PATH, TBaseOP::getTApiMetaDataFileName(tbasePtr));
    TBaseOP::writeTbaseToJsonFile(testCasePath /= apiInfoPath, *tbasePtr);
}

void output_string(const boost::shared_ptr<apache::thrift::TBase>& tbasePtr)
{
    boost::filesystem::path testCasePath(CollectorContext::getContext().getTestCasePath());
    boost::filesystem::path apiInfoPath =
            TestCase::getResultFilePath(STRING_META_DATA_PATH, TBaseOP::getTStringMetaDataFileName(tbasePtr));
    TBaseOP::writeTbaseToJsonFile(testCasePath /= apiInfoPath, *tbasePtr);
}
