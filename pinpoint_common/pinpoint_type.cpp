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
/*
 * pinpoint_type.cpp
 *
 *  Created on: Mar 29, 2017
 *      Author: bluse
 */

#include "pinpoint_type.h"

#include <cstddef>
namespace Pinpoint
{

namespace Naming
{
const char* gNameTable[] = {
        "USER" ,
        "HOME" ,
        "FCGI_ROLE" ,
        "SCRIPT_FILENAME" ,
        "QUERY_STRING" ,
        "REQUEST_METHOD" ,
        "CONTENT_TYPE" ,
        "CONTENT_LENGTH" ,
        "SCRIPT_NAME" ,
        "REQUEST_URI" ,
        "DOCUMENT_URI" ,
        "DOCUMENT_ROOT" ,
        "SERVER_PROTOCOL" ,
        "REQUEST_SCHEME" ,
        "GATEWAY_INTERFACE" ,
        "SERVER_SOFTWARE" ,
        "REMOTE_ADDR" ,
        "REMOTE_PORT" ,
        "SERVER_ADDR" ,
        "SERVER_PORT" ,
        "SERVER_NAME" ,
        "REDIRECT_STATUS" ,
        "HTTP_HOST" ,
        "HTTP_CONNECTION" ,
        "HTTP_XIP" ,
        "HTTP_CACHE_CONTROL" ,
        "HTTP_DF" ,
        "HTTP_USER_AGENT" ,
        "HTTP_ASFDAS" ,
        "HTTP_ASDFASDF" ,
        "HTTP_ASDFSA" ,
        "HTTP_POSTMAN_TOKEN" ,
        "HTTP_ACCEPT" ,
        "HTTP_ACCEPT_ENCODING" ,
        "HTTP_ACCEPT_LANGUAGE" ,
        "PHP_SELF" ,
        "REQUEST_TIME_FLOAT" ,
        "REQUEST_TIME" ,
        NULL};
}}
