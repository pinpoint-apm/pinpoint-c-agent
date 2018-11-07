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
#include "string_data_sender.h"
#include "pinpoint_agent_context.h"
#include "pinpoint_api.h"

using namespace Pinpoint::log;

namespace Pinpoint
{
    namespace Agent
    {
        //<editor-fold desc="StringDataManager">

        StringDataManagerPtr StringDataManager::instance;

        StringDataManager::StringDataManager()
                : stringIdGen(-1)
        {

        }

        int32_t StringDataManager::addString(const std::string &stringValue)
        {
            PinpointAgentContextPtr contextPtr = PinpointAgentContext::getContextPtr();

            assert (contextPtr != NULL);

            try
            {
                TBasePtr tBasePtr;
                TStringMetaData* stringMetaData = new TStringMetaData;
                int32_t stringId = stringIdGen--;
                stringMetaData->__set_stringValue(stringValue);
                stringMetaData->__set_agentId(contextPtr->agentId);
                stringMetaData->__set_agentStartTime((int64_t)contextPtr->startTimestamp);
                stringMetaData->__set_stringId(stringId);
                tBasePtr.reset(stringMetaData);
                strings.push_back(tBasePtr);
                return stringId;
            }
            catch (std::exception& exception)
            {
                LOGE("addString exception=%d", exception.what());
                return INVALID_STRING_ID;
            }



        }

        int32_t StringDataManager::initStringDataManager()
        {
            try
            {
                instance.reset(new StringDataManager());
                instance->strings.reserve(DEFAULT_STRING_COUNT);
            }
            catch (std::bad_alloc&)
            {
                LOGE("new StringDataManager failed.");
                return OUT_OF_MEMORY;
            }

            return SUCCESS;
        }

        StringDataManagerPtr& StringDataManager::getInstance()
        {
            PINPOINT_ASSERT_RETURN((instance != NULL), instance);
            return instance;
        }

        const std::vector<TBasePtr>& StringDataManager::getAllString() const
        {
            return strings;
        }
        //</editor-fold>

        int32_t StringDataSender::init()
        {
            LOGI("StringDataSender::init() start. ");
            this->isInitSuccess = true;
            return SUCCESS;
        }

        int32_t StringDataSender::send(const TBasePtr& tBasePtr)
        {
            if (!isInitSuccess)
            {
                return FAILED;
            }

            try
            {
                PacketPtr packetPtr(new Packet(PacketType::APPLICATION_REQUEST, 3));
                PacketData& packetData = packetPtr->getPacketData();
                packetData = tBasePtr;

                LOGT("TStringMetaData: [%s]", utils::TBaseToString(packetData).c_str());

                dataSender->sendPacket(packetPtr, 100);
            }
            catch (std::exception& exception)
            {
                LOGE("send StringMetaData exception. exception=%s", exception.what());
            }

            return SUCCESS;
        }

        StringDataSender::StringDataSender(const boost::shared_ptr<DataSender> &dataSender)
                : isInitSuccess(false), dataSender(dataSender)
        {

        }

        StringDataSender::~StringDataSender()
        {

        }

        int32_t StringDataSender::start()
        {
            if (!isInitSuccess)
            {
                return SUCCESS;
            }

            StringDataManagerPtr& manager = StringDataManager::getInstance();
            assert (manager != NULL);

            const std::vector<TBasePtr> &strings = manager->getAllString();
            std::vector<TBasePtr>::const_iterator ip;
            for (ip = strings.begin(); ip != strings.end(); ++ip)
            {
                int32_t err = send(*ip);
                if (err != SUCCESS)
                {
                    // todo
                    LOGE("send string meta info error!");
                }
            }

            return SUCCESS;
        }

    }
}
