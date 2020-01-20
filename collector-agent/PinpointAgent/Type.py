# ------------------------------------------------------------------------------
#  Copyright  2020. NAVER Corp.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# ------------------------------------------------------------------------------
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# ------------------------------------------------------------------------------

# encoding: utf-8

APPLICATION_SEND = 1
APPLICATION_TRACE_SEND = 2
APPLICATION_TRACE_SEND_ACK = 3
APPLICATION_REQUEST = 5
APPLICATION_RESPONSE = 6
APPLICATION_STREAM_CREATE = 10
APPLICATION_STREAM_CREATE_SUCCESS = 12
APPLICATION_STREAM_CREATE_FAIL = 14
APPLICATION_STREAM_CLOSE = 15
APPLICATION_STREAM_PING = 17
APPLICATION_STREAM_PONG = 18
APPLICATION_STREAM_RESPONSE = 20
CONTROL_CLIENT_CLOSE = 100
CONTROL_SERVER_CLOSE = 110
CONTROL_HANDSHAKE = 150
CONTROL_HANDSHAKE_RESPONSE = 151
CONTROL_PING = 200
CONTROL_PONG = 201
UNKNOWN = 500
HEADLESS = (1 << 16)

API_UNDEFINED = -1
API_DEFAULT = 0
API_EXCEPTION = 1
API_ANNOTATION = 2
API_PARAMETER = 3
API_WEB_REQUEST = 100
API_INVOCATION = 200

SIGNATURE = 0xef
HEADER_VERSION = 0x10

UNKNOWN_SERVICE = -1
PHP = 1500
PHP_METHOD_CALL = 1501
PHP_REMOTE_METHOD = 9700
HTTPD_SERVICE = 1931
HTTPD_PROXY = 2923

UNKNOWN = -1
SPAN = 40
AGENT_INFO = 50
AGENT_STAT = 55
AGENT_STAT_BATCH = 56
API_META_DATA = 310
STRING_META_DATA = 330
RESULT = 320
TVERSION = 0
PROXY_HTTP_HEADER = 300
PHP_RETURN = 921
PHP_ARGS = 922
PHP_DESCRIPTION = 923

#collector type
SUPPORT_GRPC=1
SUPPORT_THRIFT=2



class AgentSocketCode(object):
    NONE = 1
    BEING_CONNECT = 2
    CONNECTED = 3
    CONNECT_FAILED = 6
    IGNORE = 9
    RUN_WITHOUT_HANDSHAKE = 10
    RUN_SIMPLEX = 11
    RUN_DUPLEX = 12
    BEING_CLOSE_BY_CLIENT = 20
    CLOSED_BY_CLIENT = 22
    UNEXPECTED_CLOSE_BY_CLIENT = 26
    BEING_CLOSE_BY_SERVER = 30
    CLOSED_BY_SERVER = 32
    UNEXPECTED_CLOSE_BY_SERVER = 36
    ERROR_UNKNOWN = 40
    ERROR_ILLEGAL_STATE_CHANGE = 41
    ERROR_SYNC_STATE_SESSION = 42

class ControlMessageProtocolConstant:
    TYPE_CHARACTER_NULL = b'N'
    TYPE_CHARACTER_BOOL_TRUE = b'T'
    TYPE_CHARACTER_BOOL_FALSE = b'F'
    TYPE_CHARACTER_INT = b'I'
    TYPE_CHARACTER_LONG = b'L'
    TYPE_CHARACTER_DOUBLE = b'D'
    TYPE_CHARACTER_STRING = b'S'
    CONTROL_CHARACTER_LIST_START = b'V'
    CONTROL_CHARACTER_LIST_END = b'z'
    CONTROL_CHARACTER_MAP_START = b'M'
    CONTROL_CHARACTER_MAP_END = b'z'
    def __init__(self):
        pass

class PacketType:
    APPLICATION_SEND = 1
    APPLICATION_TRACE_SEND = 2
    APPLICATION_TRACE_SEND_ACK = 3
    APPLICATION_REQUEST = 5
    APPLICATION_RESPONSE = 6
    APPLICATION_STREAM_CREATE = 10
    APPLICATION_STREAM_CREATE_SUCCESS = 12
    APPLICATION_STREAM_CREATE_FAIL = 14
    APPLICATION_STREAM_CLOSE = 15
    APPLICATION_STREAM_PING = 17
    APPLICATION_STREAM_PONG = 18
    APPLICATION_STREAM_RESPONSE = 20
    CONTROL_CLIENT_CLOSE = 100
    CONTROL_SERVER_CLOSE = 110
    CONTROL_HANDSHAKE = 150
    CONTROL_HANDSHAKE_RESPONSE = 151
    CONTROL_PING = 200
    CONTROL_PING_PAYLOAD = 211
    CONTROL_PING_SIMPLE = 210
    CONTROL_PONG = 201
    UNKNOWN = 500
    HEADLESS = (1 << 16)

    PTypeBytesLayOut = {
        # header_length |sub_header_length|body_len_offset|
        APPLICATION_SEND: (4,0,4),
        CONTROL_CLIENT_CLOSE: (4,0,4),
        CONTROL_SERVER_CLOSE: (4,0,4),

        APPLICATION_STREAM_CLOSE: (6,4,2),
        APPLICATION_STREAM_CREATE_FAIL: (6, 4, 2),
        APPLICATION_STREAM_CREATE_SUCCESS: (4,4,0),

        APPLICATION_STREAM_PING: (8,4,4),
        APPLICATION_STREAM_PONG: (8,4,4),

        CONTROL_HANDSHAKE_RESPONSE: (8,4,4),
        CONTROL_HANDSHAKE: (8,4,4),
        APPLICATION_RESPONSE: (8,4,4),
        APPLICATION_REQUEST: (8,4,4),
        APPLICATION_STREAM_CREATE: (8,4,4),
        APPLICATION_STREAM_RESPONSE: (8,4,4),
        CONTROL_PING_SIMPLE:(0, 0, 0),
        CONTROL_PONG:(0,0,0)
    }

