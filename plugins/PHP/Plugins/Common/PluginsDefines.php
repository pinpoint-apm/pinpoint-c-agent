<?php
/******************************************************************************
 * Copyright 2020 NAVER Corp.                                                 *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License");            *
 * you may not use this file except in compliance with the License.           *
 * You may obtain a copy of the License at                                    *
 *                                                                            *
 *     http://www.apache.org/licenses/LICENSE-2.0                             *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 ******************************************************************************/

define('PP_PHP_ARGS',-1);
define('PP_PHP_RETURN',14);
define('PP_PROXY_HTTP_HEADER',300);
define('PP_SQL_ID' ,20);
define("PP_SQL",  21);
define("PP_SQL_METADATA",  22);
define("PP_SQL_PARAM",  23);
define("PP_SQL_BINDVALUE",  24);
define("PP_STRING_ID", 30);
define("PP_SQL_FORMAT","SQL");
define("PP_HTTP_URL", 40);
define("PP_HTTP_PARAM",  41);
define("PP_HTTP_PARAM_ENTITY",  42);
define("PP_HTTP_COOKIE",  45);
define("PP_HTTP_STATUS_CODE",  46);
define("PP_HTTP_INTERNAL_DISPLAY",  48);
define("PP_HTTP_IO",  49);
define("PP_MESSAGE_QUEUE_URI ",100);
define("PP_KAFKA_TOPIC",140);
define("PP_PHP_METHOD","1501");
define("PP_PHP","1500");
define("PP_MYSQL","2101");
define("PP_KAFKA","8660");
define("PP_REDIS","8200");
define("PP_REDIS_REDISSON","8203");
define("PP_REDIS_REDISSON_INTERNAL","8204");
define("PP_MEMCACHED","8050");
define("PP_PHP_REMOTE",9700);

define("PP_HEADER_NGINX_PROXY","Pinpoint-ProxyNginx");
define("PP_HEADER_APACHE_PROXY","HTTP_PINPOINT_PROXYAPACHE");
define("PP_HEADER_SAMPLED","HTTP_PINPOINT_SAMPLED");
define("PP_HEADER_PINPOINT_HOST","HTTP_PINPOINT_HOST");
define("PP_HEADER_PAPPTYPE","HTTP_PINPOINT_PAPPTYPE");
define("PP_HEADER_PAPPNAME","HTTP_PINPOINT_PAPPNAME");
define("PP_HEADER_TRACEID","HTTP_PINPOINT_TRACEID");
define("PP_HEADER_SPANID","HTTP_PINPOINT_SPANID");
define("PP_HEADER_PSPANID","HTTP_PINPOINT_PSPANID");

define("PP_SERVER_TYPE","stp");
define("PP_NEXT_SPAN_ID",'nsid');
define("PP_DESTINATION","dst");
define("PP_INTERCEPTOR_NAME","name");
define("PP_ADD_EXCEPTION","EXP");
define("PP_ROOT_LOC",1);
define("PP_REQ_URI","uri");
define("PP_REQ_CLIENT","client");
define("PP_REQ_SERVER","server");
define("PP_APP_NAME","appname");
define("PP_APP_ID","appid");
define("PP_PARENT_SPAN_ID","psid");
define("PP_PARENT_NAME","pname");
define("PP_PARENT_TYPE","ptype");
define("PP_PARENT_HOST","Ah");
define("PP_NGINX_PROXY","NP");
define("PP_APACHE_PROXY","AP");
define("PP_TRANSCATION_ID","tid");
define("PP_SPAN_ID","sid");
define("PP_NOT_SAMPLED","s0");
define("PP_SAMPLED","s1");
