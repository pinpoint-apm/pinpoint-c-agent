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
#include "hand_shake_sender.h"

using namespace Pinpoint::Agent;

#define CHECK_HANDSHAKE_RESPONSE_CODE(pa, pb) do { \
    LOGI("test [%s]", pa->toString().c_str()); \
    if ((pa) != (pb)) \
    { \
        LOGE("[%s] ! = [%s]", pa->toString().c_str(), (pb)->toString().c_str()); \
        assert (false); \
    } \
} while(0);

TEST(hand_shake, code_test)
{
    HandshakeResponseCode* target = NULL;

    target = HandshakeResponseCode::get(0, 0);
    CHECK_HANDSHAKE_RESPONSE_CODE(target, &HandshakeResponseCode::SUCCESS);

    target = HandshakeResponseCode::get(0, 1);
    CHECK_HANDSHAKE_RESPONSE_CODE(target, &HandshakeResponseCode::SIMPLEX_COMMUNICATION);

    target = HandshakeResponseCode::get(0, 2);
    CHECK_HANDSHAKE_RESPONSE_CODE(target, &HandshakeResponseCode::DUPLEX_COMMUNICATION);

    target = HandshakeResponseCode::get(1, 0);
    CHECK_HANDSHAKE_RESPONSE_CODE(target, &HandshakeResponseCode::ALREADY_KNOWN);

    target = HandshakeResponseCode::get(1, 1);
    CHECK_HANDSHAKE_RESPONSE_CODE(target, &HandshakeResponseCode::ALREADY_SIMPLEX_COMMUNICATION);

    target = HandshakeResponseCode::get(1, 2);
    CHECK_HANDSHAKE_RESPONSE_CODE(target, &HandshakeResponseCode::ALREADY_DUPLEX_COMMUNICATION);

    target = HandshakeResponseCode::get(2, 0);
    CHECK_HANDSHAKE_RESPONSE_CODE(target, &HandshakeResponseCode::PROPERTY_ERROR);

    target = HandshakeResponseCode::get(3, 0);
    CHECK_HANDSHAKE_RESPONSE_CODE(target, &HandshakeResponseCode::PROTOCOL_ERROR);

    target = HandshakeResponseCode::get(4, 0);
    CHECK_HANDSHAKE_RESPONSE_CODE(target, &HandshakeResponseCode::UNKNOWN_ERROR);

    target = HandshakeResponseCode::get(100, 100);
    CHECK_HANDSHAKE_RESPONSE_CODE(target, &HandshakeResponseCode::UNKNOWN_CODE);
}
