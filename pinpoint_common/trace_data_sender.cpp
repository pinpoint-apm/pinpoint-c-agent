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
#include "trace_data_sender.h"
#include "pinpoint_agent_context.h"
#include "serializer.h"

using namespace Pinpoint::log;
using namespace Pinpoint::utils;
using namespace Pinpoint::Trace;

namespace Pinpoint
{
    namespace Agent
    {

        TraceDataSender::TraceDataSender(const boost::shared_ptr<DataSender> &dataSender)
                : dataSender(dataSender)
        {

        }

        TraceDataSender::~TraceDataSender()
        {

        }

        int32_t TraceDataSender::init()
        {
            LOGI("TraceDataSender::init() start. ");

            TMemoryBuffer *tb = new(std::nothrow) TMemoryBuffer();
            if (tb == NULL)
            {
                LOGE("new TMemoryBuffer fail.");
                return FAILED;
            }

            this->transportOut.reset(tb);

            TCompactProtocol *tp = new(std::nothrow) TCompactProtocol(this->transportOut);
            if (tp == NULL)
            {
                LOGE("new TCompactProtocol fail.");
                return FAILED;
            }
            this->protocolOut.reset(tp);

            return SUCCESS;
        }

        int32_t TraceDataSender::start()
        {
            return SUCCESS;
        }

        int32_t TraceDataSender::send(const TracePtr &tracePtr)
        {
            if (protocolOut == NULL || transportOut == NULL)
            {
                return FAILED;
            }

            try
            {
                std::string data;
                int32_t err;

                PINPOINT_ASSERT_RETURN ((tracePtr != NULL), FAILED);
                DefaultTracePtr defaultTracePtr = boost::dynamic_pointer_cast<DefaultTrace>(tracePtr);

                SpanPtr& spanPtr = defaultTracePtr->getSpanPtr();

                PINPOINT_ASSERT_RETURN ((spanPtr != NULL), FAILED);

                LOGT("Span: [%s]", utils::TBaseToString(spanPtr->getTSpan()).c_str());

                err = serializer.serializer(spanPtr->getTSpan(), data);
                if (err != SUCCESS)
                {
                    LOGE("serializer trace failed");
                    return err;
                }

                PacketPtr packetPtr(new Packet(PacketType::HEADLESS, 1));
                PacketData& packetData = packetPtr->getPacketData();
                packetData = data;

                return this->dataSender->sendPacket(packetPtr, 100);
            }
            catch (std::bad_alloc& e)
            {
                LOGE("TraceDataSender::send throw: e=%s", e.what());
                return FAILED;
            }
            catch (std::exception& e)
            {
                LOGE("TraceDataSender::send throw: e=%s", e.what());
                assert (false);
                return FAILED;
            }
        }

    }
}
