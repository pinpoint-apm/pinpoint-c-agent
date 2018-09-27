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
#ifndef PINPOINT_DEF_H
#define PINPOINT_DEF_H

#include "stdint.h"

#define PINPOINT_ASSERT_RETURN(cond, err) do { \
    if (!(cond)) \
    { \
        LOGE("%s assert false!!! return", #cond); \
        assert(false); \
        return err; \
    } \
} while(0);

#define PINPOINT_ASSERT(cond) do { \
    if (!(cond)) \
    { \
        LOGE("%s assert false!!!", #cond); \
        assert(false); \
        return; \
    } \
} while(0);

namespace Pinpoint
{

    const char PINPOINT_AGENT_VERSION[] = "1.6.0-SNAPSHOT";

    const char HTTP_TRACE_ID[] = "Pinpoint-TraceID";
    const char HTTP_SPAN_ID[] = "Pinpoint-SpanID";
    const char HTTP_PARENT_SPAN_ID[] = "Pinpoint-pSpanID";
    const char HTTP_SAMPLED[] = "Pinpoint-Sampled";
    const char HTTP_FLAGS[] = "Pinpoint-Flags";
    const char HTTP_PARENT_APPLICATION_NAME[] = "Pinpoint-pAppName";
    const char HTTP_PARENT_APPLICATION_TYPE[] = "Pinpoint-pAppType";
    const char HTTP_HOST[] = "Pinpoint-Host";

#ifdef TEST_SIMULATE
    const char HTTP_TESTCASE_PATH[] = "Pinpoint-Testcase-PATH";
#endif


/* api info */
// copy from java MethodType
    typedef enum
    {
        API_UNDEFINED = -1,
        // method
        API_DEFAULT = 0,
        // exception message
        API_EXCEPTION = 1,
        // information
        API_ANNOTATION = 2,
        // method parameter
        API_PARAMETER = 3,
        // tomcat, jetty, bloc ...
        API_WEB_REQUEST = 100,
        // sync/async
        API_INVOCATION = 200,

        // database, javascript

        // corrupted when : 1. slow network, 2. too much node ...
        API_CORRUPTED = 900
    } ApiType;

    typedef enum{
        TYPE_APP = 1,
        TYPE_NGINX = 2,
        TYPE_APACHE = 3
    }ProxyHeaderType;

    namespace Trace
    {
        /* copy from AnnotationKey.java */
        class AnnotationKey
        {
        public:
            static const int32_t API = 12;
            static const int32_t API_METADATA = 13;
            static const int32_t RETURN_DATA = 14;
            static const int32_t API_TAG = 10015;

            static const int32_t ERROR_API_METADATA_ERROR = 10000010;
            static const int32_t ERROR_API_METADATA_AGENT_INFO_NOT_FOUND = 10000011;
            static const int32_t ERROR_API_METADATA_IDENTIFIER_CHECK_ERROR = 10000012;
            static const int32_t ERROR_API_METADATA_NOT_FOUND = 10000013;
            static const int32_t ERROR_API_METADATA_DID_COLLSION = 10000014;

            static const int32_t SQL_ID = 20;
            static const int32_t SQL = 21;
            static const int32_t SQL_METADATA = 22;
            static const int32_t SQL_PARAM = 23;
            static const int32_t SQL_BINDVALUE = 24;

            static const int32_t STRING_ID = 30;

            static const int32_t HTTP_URL = 40;
            static const int32_t HTTP_PARAM = 41;
            static const int32_t HTTP_PARAM_ENTITY = 42;
            static const int32_t HTTP_COOKIE = 45;
            static const int32_t HTTP_STATUS_CODE = 46;
            static const int32_t HTTP_INTERNAL_DISPLAY = 48;
            static const int32_t HTTP_IO = 49;

            static const int32_t MESSAGE_QUEUE_URI = 100;

            static const int32_t ARGS0 = -1;
            static const int32_t ARGS1 = -2;
            static const int32_t ARGS2 = -3;
            static const int32_t ARGS3 = -4;
            static const int32_t ARGS4 = -5;
            static const int32_t ARGS5 = -6;
            static const int32_t ARGS6 = -7;
            static const int32_t ARGS7 = -8;
            static const int32_t ARGS8 = -9;
            static const int32_t ARGS9 = -10;
            static const int32_t ARGSN = -11;

            static const int32_t CACHE_ARGS0 = -30;
            static const int32_t CACHE_ARGS1 = -31;
            static const int32_t CACHE_ARGS2 = -32;
            static const int32_t CACHE_ARGS3 = -33;
            static const int32_t CACHE_ARGS4 = -34;
            static const int32_t CACHE_ARGS5 = -35;
            static const int32_t CACHE_ARGS6 = -36;
            static const int32_t CACHE_ARGS7 = -37;
            static const int32_t CACHE_ARGS8 = -38;
            static const int32_t CACHE_ARGS9 = -39;
            static const int32_t CACHE_ARGSN = -40;

            /* Deprecated */
            static const int32_t EXCEPTION = -50;
            /* Deprecated */
            static const int32_t EXCEPTION_CLASS = -51;

            static const int32_t UNKNOWN = -9999;
            static const int32_t ASYNC = -100;

            /* PHP */
            static const int32_t PROXY_HTTP_HEADER = 300;
            static const int32_t PHP_RETURN = 921;
            static const int32_t PHP_ARGS = 922;
            static const int32_t PHP_DESCRIPTION = 923;

#ifdef TEST_SIMULATE
            static const int32_t TESTCASE_PATH = 999;
#endif
        };
    }

/* service type */
//#define HTTPD_SERVICE_TYPE 1911
//#define HTTPD_PROXY_SERVICE_TYPE 1912

//#define PHP_SERVICE 1921
//#define PHP_SERVICE_METHOD 1922
    static const int32_t UNKNOWN_SERVICE = -1;
    static const int32_t PHP = 1500;
    static const int32_t PHP_METHOD_CALL = 1501;
    static const int32_t PHP_REMOTE_METHOD = 9700;


    static const int32_t HTTPD_SERVICE = 1931;
    static const int32_t HTTPD_PROXY = 2923;

}

#endif
