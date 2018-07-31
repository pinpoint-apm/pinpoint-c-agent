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
#ifndef PINPOINT_TRACE_DATA_SENDER_H
#define PINPOINT_TRACE_DATA_SENDER_H

#include "thrift/protocol/TCompactProtocol.h"
#include "thrift/transport/TBufferTransports.h"
#include <boost/enable_shared_from_this.hpp>
#include "data_sender.h"
#include "trace.h"
#include "serializer.h"

namespace Pinpoint
{
    namespace Agent
    {

        class TraceDataSender : public boost::enable_shared_from_this<TraceDataSender>
        {
        public:
            explicit TraceDataSender(const boost::shared_ptr<DataSender> &dataSender);

            ~TraceDataSender();

            int32_t init();

            int32_t start();

            int32_t send(const Trace::TracePtr &tracePtr);

        private:
            boost::shared_ptr<DataSender> dataSender;
            boost::shared_ptr<TMemoryBuffer> transportOut;
            boost::shared_ptr<TCompactProtocol> protocolOut;
            HeaderTBaseSerializer serializer;
        };

        typedef boost::shared_ptr<TraceDataSender> TraceDataSenderPtr;

    }
}

#endif
