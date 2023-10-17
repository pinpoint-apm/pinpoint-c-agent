////////////////////////////////////////////////////////////////////////////////
// Copyright 2020 NAVER Corp
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

const static char* PP_PHP_ARGS = "-1";
const static int PP_PHP_RETURN = 14;
const static int PP_PROXY_HTTP_HEADER = 300;
const static int PP_SQL_ID = 20;
const static int PP_SQL = 21;
const static int PP_SQL_METADATA = 22;
const static int PP_SQL_PARAM = 23;
const static int PP_SQL_BINDVALUE = 24;
const static int PP_STRING_ID = 30;
const static char* PP_HTTP_URL = "40";
const static int PP_HTTP_PARAM = 41;
const static int PP_HTTP_PARAM_ENTITY = 42;
const static int PP_HTTP_COOKIE = 45;
const static char* PP_HTTP_STATUS_CODE = "46";
const static int PP_HTTP_INTERNAL_DISPLAY = 48;
const static int PP_HTTP_IO = 49;
const static int PP_MESSAGE_QUEUE_URI = 100;
const static int PP_KAFKA_TOPIC = 140;

const static char* PP_SQL_FORMAT = "SQL";
const static char* PP_C_CPP = "1300";
const static char* PP_C_CPP_METHOD = "1301";
const static char* PP_MYSQL = "2101";
const static char* PP_KAFKA = "8660";
const static char* PP_REDIS = "8200";
const static char* PP_REDIS_REDISSON = "8203";
const static char* PP_REDIS_REDISSON_INTERNAL = "8204";
const static char* PP_MEMCACHED = "8050";
const static char* PP_C_CPP_REMOTE_METHOD = "9800";

const static char* PP_HEADER_NGINX_PROXY = "Pinpoint-ProxyNginx";
const static char* PP_HEADER_APACHE_PROXY = "HTTP_PINPOINT_PROXYAPACHE";
const static char* PP_HEADER_SAMPLED = "HTTP_PINPOINT_SAMPLED";
const static char* PP_HEADER_PINPOINT_HOST = "HTTP_PINPOINT_HOST";
const static char* PP_HEADER_PAPPTYPE = "HTTP_PINPOINT_PAPPTYPE";
const static char* PP_HEADER_PAPPNAME = "HTTP_PINPOINT_PAPPNAME";
const static char* PP_HEADER_TRACEID = "HTTP_PINPOINT_TRACEID";
const static char* PP_HEADER_SPANID = "HTTP_PINPOINT_SPANID";
const static char* PP_HEADER_PSPANID = "HTTP_PINPOINT_PSPANID";

const static char* PP_SERVER_TYPE = "stp";
const static char* PP_NEXT_SPAN_ID = "nsid";
const static char* PP_DESTINATION = "dst";
const static char* PP_INTERCEPTOR_NAME = "name";
const static char* PP_ADD_EXCEPTION = "EXP";
const static int PP_ROOT_LOC = 1;
const static char* PP_REQ_URI = "uri";
const static char* PP_REQ_CLIENT = "client";
const static char* PP_REQ_SERVER = "server";
const static char* PP_APP_NAME = "appname";
const static char* PP_APP_ID = "appid";
const static char* PP_UT = "UT";
const static char* PP_PARENT_SPAN_ID = "psid";
const static char* PP_PARENT_NAME = "pname";
const static char* PP_PARENT_TYPE = "ptype";
const static char* PP_PARENT_HOST = "Ah";
const static char* PP_NGINX_PROXY = "NP";
const static char* PP_APACHE_PROXY = "AP";
const static char* PP_TRANSCATION_ID = "tid";
const static char* PP_SPAN_ID = "sid";
const static char* PP_NOT_SAMPLED = "s0";
const static char* PP_SAMPLED = "s1";
