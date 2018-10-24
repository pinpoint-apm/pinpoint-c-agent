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
#include "api_data_sender.h"
#include "pinpoint_agent_context.h"

using namespace Pinpoint::log;

namespace Pinpoint
{
    namespace Agent
    {

        //<editor-fold desc="DefaultApiIdGenerator">

        typedef std::map<std::string, int32_t> ApiAssignTable;
        typedef std::set<int32_t> AssignedApiSet;

        class DefaultApiIdGenerator : public ApiIdGenerator
        {
        public:
            DefaultApiIdGenerator() : counter(-1) {}
            virtual int32_t generate(const std::string &apiInfo);
            virtual int32_t init();
        private:
            ApiAssignTable apiAssignTable;
            AssignedApiSet assignedApiSet;
            int32_t counter;
        };

        int32_t DefaultApiIdGenerator::init()
        {
            // read
            PinpointAgentContextPtr& contextPtr = PinpointAgentContext::getContextPtr();
            PINPOINT_ASSERT_RETURN((contextPtr != NULL), FAILED);
            PINPOINT_ASSERT_RETURN((contextPtr->agentConfigArgsPtr != NULL), FAILED);
            // note: read api from file
            if (contextPtr->agentConfigArgsPtr->apiAssignFileNameIsSet)
            {
                std::ifstream ifstream(contextPtr->agentConfigArgsPtr->apiAssignFileName.c_str());
                if (ifstream.is_open())
                {
                    std::string line;
                    std::stringstream ss;

                    int32_t id;

                    while(getline(ifstream, line))
                    {
                        if (!line.empty())
                        {
                            std::size_t found = line.find(';');
                            if (std::string::npos == found)
                            {
                                LOGE("get error format line=[%s]", line.c_str());
                                continue;
                            }

                            std::string api = Pinpoint::utils::trim(line.substr(0, found));
                            if (api.empty())
                            {
                                LOGE("get error format line=[%s]", line.c_str());
                                continue;
                            }

                            ss.clear();
                            ss.str(line.substr(found+1));

                            ss >> id;
                            if (!ss.fail() && id < INVALID_API_ID)
                            {
                                LOGT("read api: apiInfo=[%s], apiId=[%d]", api.c_str(), id);

                                // the api has been assigned
                                ApiAssignTable::iterator ip = apiAssignTable.find(api);
                                if (ip != apiAssignTable.end())
                                {
                                    LOGW("api=[%s] has been assigned. Id=[%d]", ip->first.c_str(), ip->second);
                                    continue;
                                }

                                // assign
                                assignedApiSet.insert(id);
                                apiAssignTable[api] = id;
                            }
                            else
                            {
                                LOGE("get error format line=[%s]", line.c_str());
                            }
                        }
                    }
                }
                else
                {
                    LOGE("open [%s] failed!", contextPtr->agentConfigArgsPtr->apiAssignFileName.c_str());
                }
            }


            return SUCCESS;
        }

        int32_t DefaultApiIdGenerator::generate(const std::string &apiInfo)
        {
            // the api has been assigned
            ApiAssignTable::iterator ip = apiAssignTable.find(apiInfo);
            if (ip != apiAssignTable.end())
            {
                LOGW("api=[%s] has been assigned. Id=[%d]", ip->first.c_str(), ip->second);
                return ip->second;
            }

            while (true)
            {
                if (assignedApiSet.find(counter) == assignedApiSet.end())
                {
                    break;
                }
                counter--;
            }

            assignedApiSet.insert(counter);
            apiAssignTable[apiInfo] = counter;
            return counter;
        }

        //</editor-fold>

        //<editor-fold desc="ApiDataManager">

        ApiDataManagerPtr ApiDataManager::instance;

        ApiDataManagerPtr& ApiDataManager::getInstance()
        {
            PINPOINT_ASSERT_RETURN((instance != NULL), instance);
            return instance;
        }

        int32_t ApiDataManager::initApiDataManager()
        {
            try
            {
                instance.reset(new ApiDataManager());
                instance->apis.reserve(ApiDataManager::DEFAULT_API_COUNT);
                instance->apiIdGenPtr.reset(new DefaultApiIdGenerator);
                return instance->apiIdGenPtr->init();
            }
            catch (std::bad_alloc&)
            {
                LOGE("new ApiDataManager failed.");
                return OUT_OF_MEMORY;
            }
        }


        ApiDataManager::ApiDataManager()
        {

        }

        int32_t ApiDataManager::addApi(const std::string &apiInfo, int32_t line, ApiType type)
        {
            try
            {
                PINPOINT_ASSERT_RETURN((apiIdGenPtr != NULL), INVALID_API_ID);

                int32_t apiId = apiIdGenPtr->generate(apiInfo);
                TBasePtr tBasePtr;

                TApiMetaData* tApiMetaData = new TApiMetaData;
                tApiMetaData->__set_apiId(apiId);
                if (line != -1)
                {
                    tApiMetaData->__set_line(line);
                }

                tApiMetaData->__set_apiInfo(apiInfo);
                if (type != API_UNDEFINED)
                {
                    tApiMetaData->__set_type(type);
                }

                tBasePtr.reset(tApiMetaData);
                apis.push_back(tBasePtr);
                return apiId;
            }
            catch (std::exception& exception)
            {
                LOGE("addApi exception=%d", exception.what());
                return INVALID_API_ID;
            }

        }

        const std::vector<TBasePtr> &ApiDataManager::getAllApi()
        {
            PinpointAgentContextPtr contextPtr = PinpointAgentContext::getContextPtr();

            PINPOINT_ASSERT_RETURN ((contextPtr != NULL), this->apis);

            for (std::vector<TBasePtr>::iterator ip = apis.begin(); ip != apis.end(); ++ip)
            {
                TApiMetaDataPtr tApiMetaDataPtr = boost::dynamic_pointer_cast<TApiMetaData>(*ip);
                tApiMetaDataPtr->__set_agentId(contextPtr->agentId);
                tApiMetaDataPtr->__set_agentStartTime((int64_t)contextPtr->startTimestamp);
            }
            return this->apis;
        }

        //</editor-fold>


        //<editor-fold desc="ApiDataSender">

        ApiDataSender::ApiDataSender(const boost::shared_ptr<DataSender> &dataSender)
                : isInitSuccess(false), dataSender(dataSender)
        {

        }

        ApiDataSender::~ApiDataSender()
        {

        }

        int32_t ApiDataSender::init()
        {
            LOGI("ApiDataSender::init() start. ");
            this->isInitSuccess = true;
            return SUCCESS;
        }

        // update metaData
        int32_t ApiDataSender::start()
        {
            if (!isInitSuccess)
            {
                return SUCCESS;
            }

            ApiDataManagerPtr& manager = ApiDataManager::getInstance();
            PINPOINT_ASSERT_RETURN ((manager != NULL), FAILED);

            const std::vector<TBasePtr> &apis = manager->getAllApi();
            std::vector<TBasePtr>::const_iterator ip;
            for (ip = apis.begin(); ip != apis.end(); ++ip)
            {
                int32_t err = send(*ip);
                if (err != SUCCESS)
                {
                    TApiMetaDataPtr tApiMetaDataPtr = boost::dynamic_pointer_cast<TApiMetaData>(*ip);
                    LOGE("send api meta info failed! Api=%s, id=%d",
                         tApiMetaDataPtr->apiInfo.c_str(), tApiMetaDataPtr->apiId);
                }
            }

            return SUCCESS;
        }

        int32_t ApiDataSender::send(const TBasePtr &tBasePtr)
        {

            try
            {
                PacketPtr packetPtr(new Packet(PacketType::APPLICATION_REQUEST, 3));
                PacketData& packetData = packetPtr->getPacketData();
                packetData = tBasePtr;

                LOGT("TApiMetaData: [%s]", utils::TBaseToString(packetData).c_str());

                dataSender->sendPacket(packetPtr, 100);
            }
            catch (std::exception& exception)
            {
                LOGE("send ApiMetaData exception. exception=%s", exception.what());
            }

            return SUCCESS;
        }
        //</editor-fold>

    }
}
