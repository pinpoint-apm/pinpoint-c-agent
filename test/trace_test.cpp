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
#include <buffer.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "test_env.h"
#include "trace.h"
#include "pinpoint_api.h"

using namespace Pinpoint::Trace;
using namespace Pinpoint::Plugin;

#define HEADER_TEST_INIT_BEGIN() {\
    headerMap.clear(); \
} while(0);

#define HEADER_TEST_INIT_END() {\
    header.updateHeader(headerMap); \
    pinpointHttpHeader = PinpointHttpHeader::createHeader(&header); \
} while(0);

#define HEADER_SET(key, value) headerMap[key] = value

TEST(trace, create_header)
{
    HttpHeader header;
    HeaderMap headerMap;
    PinpointHttpHeader pinpointHttpHeader;

    // sampled
    HEADER_TEST_INIT_BEGIN();
    HEADER_SET(Pinpoint::HTTP_SAMPLED, Pinpoint::Agent::Sampling::SAMPLING_RATE_FALSE);
    HEADER_TEST_INIT_END();
    assert (pinpointHttpHeader.type == VALID_HTTP_HEADER);
    assert (!pinpointHttpHeader.sampled);

    // no traceId
    HEADER_TEST_INIT_BEGIN();
    HEADER_TEST_INIT_END();
    assert (pinpointHttpHeader.type == NULL_HTTP_HEADER);
    assert (pinpointHttpHeader.sampled);

    // no spanId
    HEADER_TEST_INIT_BEGIN();
    HEADER_SET(Pinpoint::HTTP_SAMPLED, Pinpoint::Agent::Sampling::SAMPLING_RATE_TRUE);
    HEADER_SET(Pinpoint::HTTP_TRACE_ID, "php^1234567^2");
    HEADER_TEST_INIT_END();
    assert (pinpointHttpHeader.sampled);
    assert (pinpointHttpHeader.type == INVALID_HTTP_HEADER);

    // error spanId
    HEADER_TEST_INIT_BEGIN();
    HEADER_SET(Pinpoint::HTTP_SAMPLED, Pinpoint::Agent::Sampling::SAMPLING_RATE_TRUE);
    HEADER_SET(Pinpoint::HTTP_TRACE_ID, "php^1234567^2");
    HEADER_SET(Pinpoint::HTTP_SPAN_ID, "abc");
    HEADER_TEST_INIT_END();
    assert (pinpointHttpHeader.sampled);
    assert (pinpointHttpHeader.type == INVALID_HTTP_HEADER);

    // no parentSpanId
    HEADER_TEST_INIT_BEGIN();
    HEADER_SET(Pinpoint::HTTP_SAMPLED, Pinpoint::Agent::Sampling::SAMPLING_RATE_TRUE);
    HEADER_SET(Pinpoint::HTTP_TRACE_ID, "php^1234567^2");
    HEADER_SET(Pinpoint::HTTP_SPAN_ID, "999999");
    HEADER_TEST_INIT_END();
    assert (pinpointHttpHeader.sampled);
    assert (pinpointHttpHeader.type == INVALID_HTTP_HEADER);

    // error parentSpanId
    HEADER_TEST_INIT_BEGIN();
    HEADER_SET(Pinpoint::HTTP_SAMPLED, Pinpoint::Agent::Sampling::SAMPLING_RATE_TRUE);
    HEADER_SET(Pinpoint::HTTP_TRACE_ID, "php^1234567^2");
    HEADER_SET(Pinpoint::HTTP_SPAN_ID, "999999");
    HEADER_SET(Pinpoint::HTTP_PARENT_SPAN_ID, "dadfa");
    HEADER_TEST_INIT_END();
    assert (pinpointHttpHeader.sampled);
    assert (pinpointHttpHeader.type == INVALID_HTTP_HEADER);

    // error flag
    HEADER_TEST_INIT_BEGIN();
    HEADER_SET(Pinpoint::HTTP_SAMPLED, Pinpoint::Agent::Sampling::SAMPLING_RATE_TRUE);
    HEADER_SET(Pinpoint::HTTP_TRACE_ID, "php^1234567^2");
    HEADER_SET(Pinpoint::HTTP_SPAN_ID, "999999");
    HEADER_SET(Pinpoint::HTTP_PARENT_SPAN_ID, "88888");
    HEADER_SET(Pinpoint::HTTP_FLAGS, "adfadf");
    HEADER_TEST_INIT_END();
    assert (pinpointHttpHeader.sampled);
    assert (pinpointHttpHeader.type == INVALID_HTTP_HEADER);

    // error parentApplicationType
    HEADER_TEST_INIT_BEGIN();
    HEADER_SET(Pinpoint::HTTP_SAMPLED, Pinpoint::Agent::Sampling::SAMPLING_RATE_TRUE);
    HEADER_SET(Pinpoint::HTTP_TRACE_ID, "php^1234567^2");
    HEADER_SET(Pinpoint::HTTP_SPAN_ID, "999999");
    HEADER_SET(Pinpoint::HTTP_PARENT_SPAN_ID, "88888");
    HEADER_SET(Pinpoint::HTTP_FLAGS, "1");
    HEADER_SET(Pinpoint::HTTP_PARENT_APPLICATION_TYPE, "abc");
    HEADER_TEST_INIT_END();
    assert (pinpointHttpHeader.sampled);
    assert (pinpointHttpHeader.type == INVALID_HTTP_HEADER);

    // ok
    HEADER_TEST_INIT_BEGIN();
    HEADER_SET(Pinpoint::HTTP_SAMPLED, Pinpoint::Agent::Sampling::SAMPLING_RATE_TRUE);
    HEADER_SET(Pinpoint::HTTP_TRACE_ID, "php^1234567^2");
    HEADER_SET(Pinpoint::HTTP_SPAN_ID, "999999");
    HEADER_SET(Pinpoint::HTTP_PARENT_SPAN_ID, "888888");
    HEADER_TEST_INIT_END();
    assert (pinpointHttpHeader.sampled);
    assert (pinpointHttpHeader.type == VALID_HTTP_HEADER);
    assert (pinpointHttpHeader.traceId == "php^1234567^2");
    assert (pinpointHttpHeader.spanId == 999999);
    assert (pinpointHttpHeader.pSpanId == 888888);
}

TEST(trace, trace_id)
{
    TraceIdPtr traceIdPtr = TraceId::parse("addfadf", 1, 2, 1);
    assert (traceIdPtr == NULL);
}

TEST(trace, transaction_id)
{
    TransactionId a = TransactionId::parseTransactionId("abc^1^2");
    TransactionId b("abc", 1, 2);

    assert (a == b);

    // error
    Pinpoint::Agent::AutoBuffer autoBuffer1;
    autoBuffer1.putByte(123);

    ASSERT_THROW(TransactionId::parseTransactionId(autoBuffer1.getBuffer()), std::invalid_argument);

    // error
    Pinpoint::Agent::AutoBuffer autoBuffer2;
    autoBuffer2.putByte(TransactionId::TVERSION);
    autoBuffer2.putPrefixedString(NULL);
    autoBuffer2.putVar64(1);
    autoBuffer2.putVar64(2);

    TransactionId::parseTransactionId(autoBuffer2.getBuffer());

    // error
    ASSERT_THROW(TransactionId::parseTransactionId("abc"), std::invalid_argument);
    ASSERT_THROW(TransactionId::parseTransactionId("abc^"), std::invalid_argument);
    ASSERT_THROW(TransactionId::parseTransactionId("abc^^"), std::invalid_argument);
    ASSERT_THROW(TransactionId::parseTransactionId("abc^a^"), std::invalid_argument);
    ASSERT_THROW(TransactionId::parseTransactionId("abc^a^b"), std::invalid_argument);

    assert (!TransactionId::isValidTransactionIdStr("abc"));
    assert (TransactionId::isValidTransactionIdStr("abc^1^2"));
}


TEST(trace, annotation)
{
    {
        DefaultAnnotation annotation(1);
        annotation.addBinaryValue("123");
    }

    {
        DefaultAnnotation annotation(1);
        annotation.addStringValue("adb");
    }

    {
        DefaultAnnotation annotation(1);
        annotation.addBoolValue(true);
    }

    {
        DefaultAnnotation annotation(1);
        annotation.addByteValue(1);
    }

    {
        DefaultAnnotation annotation(1);
        annotation.addDoubleValue(1.99);
    }

    {
        DefaultAnnotation annotation(1);
        annotation.addIntStringStringValue(2, "ab", "cd");
    }

    {
        DefaultAnnotation annotation(1);
        annotation.addIntStringValue(2, "abc");
    }

    {
        DefaultAnnotation annotation(1);
        annotation.addIntValue(100);
    }

    {
        DefaultAnnotation annotation(1);
        annotation.addLongValue(11111111L);
    }

    {
        DefaultAnnotation annotation(1);
        annotation.addShortValue(22);
    }


}
