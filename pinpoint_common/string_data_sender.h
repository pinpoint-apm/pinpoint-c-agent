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
#ifndef PINPOINT_STRING_DATA_SENDER_H
#define PINPOINT_STRING_DATA_SENDER_H

#include "thrift/gen-cpp/Trace_types.h"
#include "data_sender.h"
#include "serializer.h"

namespace Pinpoint
{
    namespace Agent
    {
        class StringDataManager;
        typedef boost::shared_ptr<StringDataManager> StringDataManagerPtr;

        class StringDataManager
        {
        public:
            static StringDataManagerPtr& getInstance();

            static int32_t initStringDataManager();

            int32_t addString(const std::string &stringValue);

            const std::vector<TBasePtr> &getAllString() const;

        private:
            const static uint16_t DEFAULT_STRING_COUNT = 100;

            StringDataManager();
            std::vector<TBasePtr> strings;
            int32_t stringIdGen;
            static StringDataManagerPtr instance;
        };

        class StringDataSender : public boost::enable_shared_from_this<StringDataSender>
        {
        public:
            explicit StringDataSender(const boost::shared_ptr<DataSender> &dataSender);

            ~StringDataSender();

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
