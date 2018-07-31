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
#ifndef PINPOINT_API_DATA_SENDER_H
#define PINPOINT_API_DATA_SENDER_H

#include "thrift/protocol/TCompactProtocol.h"
#include "thrift/transport/TBufferTransports.h"
#include <boost/enable_shared_from_this.hpp>
#include "thrift/gen-cpp/Trace_types.h"
#include "data_sender.h"
#include "serializer.h"
#include "pinpoint_api.h"

namespace Pinpoint
{
    namespace Agent
    {

        class ApiDataManager;
        typedef boost::shared_ptr<ApiDataManager> ApiDataManagerPtr;

        class ApiIdGenerator
        {
        public:
            virtual int32_t generate(const std::string &apiInfo) = 0;
            virtual int32_t init() = 0;
            virtual ~ApiIdGenerator() {}
        };
        typedef boost::shared_ptr<ApiIdGenerator> ApiIdGeneratorPtr;

        class ApiDataManager
        {
        public:
            static ApiDataManagerPtr& getInstance();

            static int32_t initApiDataManager();

            int32_t addApi(const std::string &apiInfo, int32_t line = -1, ApiType type = API_UNDEFINED);

            const std::vector<TBasePtr> &getAllApi();

        private:
            const static uint16_t DEFAULT_API_COUNT = 100;

            ApiDataManager();

            std::vector<TBasePtr> apis;
            ApiIdGeneratorPtr apiIdGenPtr;
            static ApiDataManagerPtr instance;
        };

        class ApiDataSender : public boost::enable_shared_from_this<ApiDataSender>
        {
        public:
            explicit   ApiDataSender(const boost::shared_ptr<DataSender> &dataSender);

            ~ApiDataSender();

            int32_t init();

            int32_t start();

        private:
            int32_t send(const TBasePtr& tBasePtr);

            bool isInitSuccess;
            boost::shared_ptr<DataSender> dataSender;

        };
    }
}

#endif
