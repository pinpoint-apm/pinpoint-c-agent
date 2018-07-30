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
#include "pinpoint_client.h"

using namespace Pinpoint::Agent;

#define CHECK_STATE_CHANGE(a, b, can) do {\
    bool can_ex = SocketStateCodeOP::canChangeState(a, b); \
    if (can_ex != can) \
    { \
        LOGE("[%a => %b. error]", a, b); \
        assert (false); \
    } \
} while(0);

#define CHECK_STATE_CHANGE_ENABLE(a, b) CHECK_STATE_CHANGE(a, b, true)


TEST(pinpoint_client, socket_state)
{
    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::NONE, SocketStateCodeOP::BEING_CONNECT);

    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::NONE, SocketStateCodeOP::CONNECTED);
    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::BEING_CONNECT, SocketStateCodeOP::CONNECTED);
    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::CONNECT_FAILED, SocketStateCodeOP::CONNECTED);

    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::CONNECTED, SocketStateCodeOP::IGNORE);

    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::CONNECTED, SocketStateCodeOP::RUN_WITHOUT_HANDSHAKE);

    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::RUN_WITHOUT_HANDSHAKE, SocketStateCodeOP::RUN_SIMPLEX);

    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::RUN_WITHOUT_HANDSHAKE, SocketStateCodeOP::RUN_DUPLEX);

    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::RUN_WITHOUT_HANDSHAKE, SocketStateCodeOP::BEING_CLOSE_BY_CLIENT);
    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::RUN_SIMPLEX, SocketStateCodeOP::BEING_CLOSE_BY_CLIENT);
    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::RUN_DUPLEX, SocketStateCodeOP::BEING_CLOSE_BY_CLIENT);

    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::BEING_CLOSE_BY_CLIENT, SocketStateCodeOP::CLOSED_BY_CLIENT);

    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::CONNECTED, SocketStateCodeOP::UNEXPECTED_CLOSE_BY_CLIENT);
    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::RUN_WITHOUT_HANDSHAKE, SocketStateCodeOP::UNEXPECTED_CLOSE_BY_CLIENT);
    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::RUN_SIMPLEX, SocketStateCodeOP::UNEXPECTED_CLOSE_BY_CLIENT);
    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::RUN_DUPLEX, SocketStateCodeOP::UNEXPECTED_CLOSE_BY_CLIENT);

    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::RUN_WITHOUT_HANDSHAKE, SocketStateCodeOP::BEING_CLOSE_BY_SERVER);
    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::RUN_SIMPLEX, SocketStateCodeOP::BEING_CLOSE_BY_SERVER);
    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::RUN_DUPLEX, SocketStateCodeOP::BEING_CLOSE_BY_SERVER);

    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::BEING_CLOSE_BY_SERVER, SocketStateCodeOP::CLOSED_BY_SERVER);

    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::RUN_WITHOUT_HANDSHAKE, SocketStateCodeOP::UNEXPECTED_CLOSE_BY_SERVER);
    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::RUN_SIMPLEX, SocketStateCodeOP::UNEXPECTED_CLOSE_BY_SERVER);
    CHECK_STATE_CHANGE_ENABLE(SocketStateCodeOP::RUN_DUPLEX, SocketStateCodeOP::UNEXPECTED_CLOSE_BY_SERVER);

    assert (SocketStateCodeOP::isBeforeConnected(SocketStateCodeOP::NONE));
    assert (SocketStateCodeOP::isBeforeConnected(SocketStateCodeOP::BEING_CONNECT));
    assert (!SocketStateCodeOP::isBeforeConnected(SocketStateCodeOP::CONNECTED));

    assert (SocketStateCodeOP::isRunDuplex(SocketStateCodeOP::RUN_DUPLEX));
    assert (!SocketStateCodeOP::isRunDuplex(SocketStateCodeOP::RUN_SIMPLEX));

    assert (SocketStateCodeOP::isClosed(SocketStateCodeOP::CLOSED_BY_CLIENT));
    assert (SocketStateCodeOP::isClosed(SocketStateCodeOP::UNEXPECTED_CLOSE_BY_CLIENT));
    assert (SocketStateCodeOP::isClosed(SocketStateCodeOP::UNEXPECTED_CLOSE_BY_SERVER));
    assert (SocketStateCodeOP::isClosed(SocketStateCodeOP::CLOSED_BY_SERVER));
    assert (SocketStateCodeOP::isClosed(SocketStateCodeOP::ERROR_UNKNOWN));
    assert (SocketStateCodeOP::isClosed(SocketStateCodeOP::ERROR_ILLEGAL_STATE_CHANGE));
    assert (SocketStateCodeOP::isClosed(SocketStateCodeOP::ERROR_SYNC_STATE_SESSION));
    assert (!SocketStateCodeOP::isClosed(SocketStateCodeOP::CONNECTED));

    assert (SocketStateCodeOP::isError(SocketStateCodeOP::ERROR_ILLEGAL_STATE_CHANGE));
    assert (SocketStateCodeOP::isError(SocketStateCodeOP::ERROR_UNKNOWN));
    assert (!SocketStateCodeOP::isError(SocketStateCodeOP::CONNECTED));
}
