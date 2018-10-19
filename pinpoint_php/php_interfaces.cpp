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
#include <Zend/zend_interfaces.h>
#include <Zend/zend_objects_API.h>
#include <Zend/zend_constants.h>
#include "php.h"
#include "php_interfaces.h"
#include "pinpoint_api.h"
#include "php_common.h"
#include "php_plugin.h"
#include "aop_hook.h"
#include "php_define_wapper.h"


using namespace Pinpoint::log;


static zend_class_entry* ce_trace = NULL;
static zend_class_entry* ce_span_event_recorder = NULL;
static zend_class_entry* ce_plugin = NULL;
static zend_class_entry* ce_interceptor = NULL;


#define CHECK_AGENT_STARTED do {\
    Pinpoint::Agent::AgentPtr agentPtr = Pinpoint::Agent::Agent::getAgentPtr(); \
    if (agentPtr == NULL || agentPtr->getAgentStatus() != Pinpoint::Agent::AGENT_STARTED) \
    { \
        LOGE("agent has not be started!"); \
        return; \
    } \
} while(0);

#define CHECK_AGENT_BEFORE_START do {\
    Pinpoint::Agent::AgentPtr agentPtr = Pinpoint::Agent::Agent::getAgentPtr(); \
    if (agentPtr == NULL || (agentPtr->getAgentStatus() != Pinpoint::Agent::AGENT_CREATED && agentPtr->getAgentStatus() != Pinpoint::Agent::AGENT_PRE_INITED)) \
    { \
        LOGE("agent has not be started!"); \
        return; \
    } \
} while(0);

#define CHECK_AGENT_PRE_INITED do {\
    Pinpoint::Agent::AgentPtr agentPtr = Pinpoint::Agent::Agent::getAgentPtr(); \
    if (agentPtr == NULL && (agentPtr->getAgentStatus() == Pinpoint::Agent::AGENT_PRE_INITED \
                            || agentPtr->getAgentStatus() == Pinpoint::Agent::AGENT_INITED \
                            || agentPtr->getAgentStatus() == Pinpoint::Agent::AGENT_STARTED )) \
    { \
        LOGE("agent has not be pre-init!"); \
        return; \
    } \
} while(0);



//<editor-fold desc="SpanEventRecorder">

const static char SPAN_EVENT_INDEX_PROPERTY_NAME[] = "__pinpoint_span_event_index";
const static int32_t SPAN_EVENT_INDEX_PROPERTY_NAME_LEN = sizeof(SPAN_EVENT_INDEX_PROPERTY_NAME) / sizeof(char) - 1;

ZEND_BEGIN_ARG_INFO_EX(arginfo_span_set_api_id, 0, 0, 1)
     ZEND_ARG_INFO(0, api_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_span_set_service_type, 0, 0, 1)
    ZEND_ARG_INFO(0, service_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_span_add_annotation, 0, 0, 4)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value1)
    ZEND_ARG_INFO(0, value2)
    ZEND_ARG_INFO(0, value3)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_span_set_end_point, 0, 0, 1)
    ZEND_ARG_INFO(0, endpoint)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_span_set_destination_id, 0, 0, 1)
    ZEND_ARG_INFO(0, destination)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_span_rpc, 0, 0, 1)
    ZEND_ARG_INFO(0, rpc)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_span_set_next_span_id, 0, 0, 1)
    ZEND_ARG_INFO(0, nextSpanId)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_span_set_exception_info, 0, 0, 1)
    ZEND_ARG_INFO(0, exceptionInfo)
ZEND_END_ARG_INFO()

static const zend_function_entry span_event_recorder_methods[] = {
    PHP_ME(SpanEventRecorder, __construct, NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
    PHP_ME(SpanEventRecorder, markBeforeTime, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SpanEventRecorder, markAfterTime, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SpanEventRecorder, setServiceType, arginfo_span_set_service_type, ZEND_ACC_PUBLIC)
    PHP_ME(SpanEventRecorder, setApiId, arginfo_span_set_api_id, ZEND_ACC_PUBLIC)
    PHP_ME(SpanEventRecorder, addAnnotation, arginfo_span_add_annotation, ZEND_ACC_PUBLIC)
    PHP_ME(SpanEventRecorder, setEndPoint, arginfo_span_set_end_point, ZEND_ACC_PUBLIC)
    PHP_ME(SpanEventRecorder, setDestinationId, arginfo_span_set_destination_id, ZEND_ACC_PUBLIC)
    PHP_ME(SpanEventRecorder, setRpc, arginfo_span_rpc, ZEND_ACC_PUBLIC)
    PHP_ME(SpanEventRecorder, setNextSpanId, arginfo_span_set_next_span_id, ZEND_ACC_PUBLIC)
    PHP_ME(SpanEventRecorder, setExceptionInfo, arginfo_span_set_exception_info, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

Pinpoint::Trace::SpanEventRecorderPtr getSpanEventRecorderPtrByZval(zval* obj)
{
    TSRMLS_FETCH();

    if (!Pinpoint::Trace::Trace::isStarted())
    {
      return Pinpoint::Trace::SpanEventRecorder::NULL_SPAN_EVENT_RECORDER_PTR;
    }

    if (!is_class_impl(obj, "Pinpoint\\SpanEventRecorder"))
    {
        LOGE("getSpanEventRecorderPtrByZval obj is not a SpanEventRecorder!");
        return Pinpoint::Trace::SpanEventRecorder::NULL_SPAN_EVENT_RECORDER_PTR;
    }

    zval* z_index = NULL;
    w_zend_read_property(z_index, zend_get_class_entry(obj TSRMLS_CC), obj,
                         SPAN_EVENT_INDEX_PROPERTY_NAME,
                         SPAN_EVENT_INDEX_PROPERTY_NAME_LEN, 0);

    if (z_index == NULL || Z_TYPE_P(z_index) != IS_LONG)
    {
        LOGE("getSpanEventRecorderPtrByZval get z_index error!");
        return Pinpoint::Trace::SpanEventRecorder::NULL_SPAN_EVENT_RECORDER_PTR;
    }

    uint64_t call_id = (uint64_t)Z_LVAL_P(z_index);

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    PINPOINT_ASSERT_RETURN ((tracePtr != NULL), Pinpoint::Trace::SpanEventRecorder::NULL_SPAN_EVENT_RECORDER_PTR);

    return tracePtr->getSpanEventRecorderPtr(call_id);
}

PHP_METHOD(SpanEventRecorder, __construct)
{
    CHECK_AGENT_STARTED;
}

PHP_METHOD(SpanEventRecorder, setExceptionInfo)
{
    CHECK_AGENT_STARTED;

    std::string exceptionInfo;

#if PHP_VERSION_ID < 70000
    char* texceptionInfo;
    int texceptionInfoLen;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &texceptionInfo, &texceptionInfoLen) == FAILURE)
    {
        zend_error(E_ERROR, "setExceptionInfo() expects (string).");
        return;
    }
    exceptionInfo = std::string(texceptionInfo, (size_t)texceptionInfoLen);
#else
    zend_string *texceptionInfo;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &texceptionInfo) == FAILURE)
    {
      zend_error(E_ERROR, "setExceptionInfo() expects (string).");
      return;
    }
    exceptionInfo = std::string(_PSTR(texceptionInfo), _PSTR_LEN(texceptionInfo));
#endif

    zval* obj = getThis();
    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = getSpanEventRecorderPtrByZval(obj);

    PINPOINT_ASSERT (spanEventRecorderPtr != NULL);

    if (spanEventRecorderPtr != NULL)
    {
        spanEventRecorderPtr->setExceptionInfo(Pinpoint::Plugin::EXCEPTION_STRING_ID, exceptionInfo);
    }
}

PHP_METHOD(SpanEventRecorder, markBeforeTime)
{
    CHECK_AGENT_STARTED;

    zval* obj = getThis();
    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = getSpanEventRecorderPtrByZval(obj);

    PINPOINT_ASSERT (spanEventRecorderPtr != NULL);

    if (spanEventRecorderPtr != NULL)
    {
        spanEventRecorderPtr->markBeforeTime();
    }

}


PHP_METHOD(SpanEventRecorder, markAfterTime)
{
    CHECK_AGENT_STARTED;

    zval* obj = getThis();
    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = getSpanEventRecorderPtrByZval(obj);

    PINPOINT_ASSERT (spanEventRecorderPtr != NULL);
    if (spanEventRecorderPtr != NULL)
    {
        spanEventRecorderPtr->markAfterTime();
    }

}

PHP_METHOD(SpanEventRecorder, setApiId)
{
    CHECK_AGENT_STARTED;

    zend_long apiId;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &apiId) == FAILURE)
    {
        zend_error(E_ERROR, "setApiId() expects an int.");
        return;
    }

    zval* obj = getThis();
    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = getSpanEventRecorderPtrByZval(obj);

    PINPOINT_ASSERT (spanEventRecorderPtr != NULL);
    if (spanEventRecorderPtr != NULL)
    {
        spanEventRecorderPtr->setApiId(apiId);
    }

}

PHP_METHOD(SpanEventRecorder, setServiceType)
{
    CHECK_AGENT_STARTED;

    zend_long serviceType;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &serviceType) == FAILURE)
    {
        zend_error(E_ERROR, "setServiceType() expects an int.");
        return;
    }

    zval* obj = getThis();
    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = getSpanEventRecorderPtrByZval(obj);

    PINPOINT_ASSERT (spanEventRecorderPtr != NULL);
    if (spanEventRecorderPtr != NULL)
    {
        spanEventRecorderPtr->setServiceType(serviceType);
    }
}


PHP_METHOD(SpanEventRecorder, addAnnotation)
{
    CHECK_AGENT_STARTED;
    zend_long key;

    std::string ts1;
    std::string ts2;

#if PHP_VERSION_ID < 70000

    char* s1 = NULL;
    int s1_len;
    if (!Pinpoint::Trace::Trace::isStarted())
    {
        RETURN_NULL();
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &key, &s1, &s1_len) == FAILURE)
    {
        zend_error(E_ERROR, "addAnnotation() expects (int, string).");
        return;
    }
    ts1 = std::string(s1, (size_t)s1_len);

#else

    zend_string* s1;
    if (!Pinpoint::Trace::Trace::isStarted())
    {
       RETURN_NULL();
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lS", &key ,&s1) == FAILURE)
    {
       zend_error(E_ERROR, "addAnnotation() expects (int, string).");
       return;
    }

    ts1 = std::string(_PSTR(s1), _PSTR_LEN(s1));

#endif

    zval* obj = getThis();
    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = getSpanEventRecorderPtrByZval(obj);

    if (spanEventRecorderPtr == NULL)
    {
        assert (false);
        RETURN_NULL();
    }

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    PINPOINT_ASSERT (tracePtr != NULL);

    Pinpoint::Trace::AnnotationPtr annotationPtr = Pinpoint::Trace::Annotation::createAnnotation(key);
    if (annotationPtr == NULL)
    {
        LOGE("createAnnotation failed!!!");
        RETURN_NULL();
    }

    annotationPtr->addStringValue(ts1);
    spanEventRecorderPtr->addAnnotationPtr(annotationPtr);
}

PHP_METHOD(SpanEventRecorder, setEndPoint)
{
    CHECK_AGENT_STARTED;

    zval* obj = getThis();
    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = getSpanEventRecorderPtrByZval(obj);

    if (spanEventRecorderPtr == NULL)
    {
        assert (false);
        RETURN_NULL();
    }

    char* endpoint = NULL;
    int endpoint_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &endpoint, &endpoint_len) == FAILURE)
    {
        zend_error(E_ERROR, "setEndPoint() expects (string).");
        RETURN_NULL();
    }

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    PINPOINT_ASSERT (tracePtr != NULL);

    spanEventRecorderPtr->setEndPoint(endpoint);
}

PHP_METHOD(SpanEventRecorder, setDestinationId)
{
    CHECK_AGENT_STARTED;

    zval* obj = getThis();
    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = getSpanEventRecorderPtrByZval(obj);

    if (spanEventRecorderPtr == NULL)
    {
        assert (false);
        RETURN_NULL();
    }

    char* p_destination = NULL;
    int destination_len;
    std::string destination = "";
    
#if PHP_VERSION_ID < 70000

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &p_destination, &destination_len) == FAILURE)
    {
        zend_error(E_ERROR, "setDestinationId() expects (string).");
        RETURN_NULL();
    }
    destination = std::string(p_destination,destination_len);
    
#else

    zend_string* s1;
    if (!Pinpoint::Trace::Trace::isStarted())
    {
       RETURN_NULL();
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S",&s1) == FAILURE)
    {
       zend_error(E_ERROR, "setDestinationId() expects (int, string).");
       return;
    }

    destination = std::string(_PSTR(s1), _PSTR_LEN(s1));

#endif

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    PINPOINT_ASSERT (tracePtr != NULL);
    spanEventRecorderPtr->setDestinationId(destination);
}


PHP_METHOD(SpanEventRecorder, setRpc)
{
    CHECK_AGENT_STARTED;

    zval* obj = getThis();
    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = getSpanEventRecorderPtrByZval(obj);

    if (spanEventRecorderPtr == NULL)
    {
        assert (false);
        RETURN_NULL();
    }

    char* rpc = NULL;
    int rpc_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &rpc, &rpc_len) == FAILURE)
    {
        zend_error(E_ERROR, "setDestinationId() expects (string).");
        RETURN_NULL();
    }

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    PINPOINT_ASSERT (tracePtr != NULL);

    spanEventRecorderPtr->setRpc(rpc);
}


PHP_METHOD(SpanEventRecorder, setNextSpanId)
{
    CHECK_AGENT_STARTED;

    zval* obj = getThis();
    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = getSpanEventRecorderPtrByZval(obj);

    if (spanEventRecorderPtr == NULL)
    {
        assert (false);
        RETURN_NULL();
    }

    zend_long nextSpanId;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &nextSpanId) == FAILURE)
    {
        zend_error(E_ERROR, "setNextSpanId() expects (long).");
        RETURN_NULL();
    }

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    PINPOINT_ASSERT (tracePtr != NULL);

    spanEventRecorderPtr->setNextSpanId(nextSpanId);
}
//</editor-fold>


//<editor-fold desc="Trace">

ZEND_BEGIN_ARG_INFO_EX(arginfo_trace_block_begin, 0, 0, 1)
    ZEND_ARG_INFO(0, call_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_trace_get_event, 0, 0, 1)
    ZEND_ARG_INFO(0, call_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_trace_set_api_id, 0, 0, 1)
    ZEND_ARG_INFO(0, api_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_trace_add_annotation, 0, 0, 4)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value1)
    ZEND_ARG_INFO(0, value2)
    ZEND_ARG_INFO(0, value3)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_trace_set_end_point, 0, 0, 1)
    ZEND_ARG_INFO(0, endpoint)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_trace_set_remote_addr, 0, 0, 1)
    ZEND_ARG_INFO(0, remote)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_trace_set_rpc, 0, 0, 1)
    ZEND_ARG_INFO(0, rpc)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_trace_get_next_span_info, 0, 0, 2)
    ZEND_ARG_INFO(1, headers)
    ZEND_ARG_INFO(1, nextSpanId)
ZEND_END_ARG_INFO()

static const zend_function_entry trace_methods[] = {
    PHP_ME(Trace, __construct, NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
    PHP_ME(Trace, traceBlockBegin, arginfo_trace_block_begin, ZEND_ACC_PUBLIC)
    PHP_ME(Trace, traceBlockEnd, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Trace, getEvent, arginfo_trace_get_event, ZEND_ACC_PUBLIC)
    PHP_ME(Trace, setApiId, arginfo_trace_set_api_id, ZEND_ACC_PUBLIC)
    PHP_ME(Trace, addAnnotation, arginfo_trace_add_annotation, ZEND_ACC_PUBLIC)
    PHP_ME(Trace, setEndPoint, arginfo_trace_set_end_point, ZEND_ACC_PUBLIC)
    PHP_ME(Trace, setRemoteAddr, arginfo_trace_set_remote_addr, ZEND_ACC_PUBLIC)
    PHP_ME(Trace, setRpc, arginfo_trace_set_rpc, ZEND_ACC_PUBLIC)
    PHP_ME(Trace, getNextSpanInfo, arginfo_trace_get_next_span_info, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

PHP_METHOD(Trace, __construct)
{
    CHECK_AGENT_STARTED;
}

PHP_METHOD(Trace, traceBlockBegin)
{
    CHECK_AGENT_STARTED;

    zend_long call_id;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &call_id) == FAILURE)
    {
        zend_error(E_ERROR, "traceBlockBegin() expects an int.");
        return;
    }

    if (!Pinpoint::Trace::Trace::isStarted())
    {
        RETURN_NULL();
    }

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    if (tracePtr == NULL)
    {
        assert (false);
        RETURN_NULL();
    }

    if(object_init_ex(return_value, ce_span_event_recorder) != SUCCESS)
    {
        LOGE("create php span_event_recorder object failed.");
        RETURN_NULL();
    }

    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = tracePtr->traceBlockBegin((uint64_t)call_id);
    if (spanEventRecorderPtr == NULL)
    {
        LOGE("tracePtr->traceBlockBegin() failed!");
        assert (false);
        RETURN_NULL();
    }

    add_property_long(return_value, SPAN_EVENT_INDEX_PROPERTY_NAME, call_id);
}

PHP_METHOD(Trace, traceBlockEnd)
{
    CHECK_AGENT_STARTED;

    zval* object;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &object) == FAILURE)
    {
        zend_error(E_ERROR, "trace_block_end() expects an spanEventRecorder object.");
        return;
    }

    if(!is_class_impl(object, "Pinpoint\\SpanEventRecorder"))
    {
        zend_error(E_ERROR, "trace_block_end() expects an spanEventRecorder object.");
        return;
    }

    if (!Pinpoint::Trace::Trace::isStarted())
    {
        RETURN_NULL();
    }

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    if (tracePtr == NULL)
    {
        assert (false);
        RETURN_NULL();
    }

    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = getSpanEventRecorderPtrByZval(object);
    if (spanEventRecorderPtr == NULL)
    {
        assert (false);
        RETURN_NULL();
    }

    tracePtr->traceBlockEnd(spanEventRecorderPtr);
}

PHP_METHOD(Trace, getEvent)
{
    CHECK_AGENT_STARTED;

    if (!Pinpoint::Trace::Trace::isStarted())
    {
        RETURN_NULL();
    }

    zend_long call_id;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &call_id) == FAILURE)
    {
        zend_error(E_ERROR, "getEvent() expects an int.");
        return;
    }

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    if (tracePtr == NULL)
    {
        assert (false);
        RETURN_NULL();
    }

    Pinpoint::Trace::SpanEventRecorderPtr spanEventRecorderPtr = tracePtr->getSpanEventRecorderPtr((uint64_t)call_id);
    if (spanEventRecorderPtr == NULL)
    {
        RETURN_NULL();
    }

    if(object_init_ex(return_value, ce_span_event_recorder) != SUCCESS)
    {
        LOGE("create PHP span_event_recorder object failed.");
        RETURN_NULL();
    }

    add_property_long(return_value, SPAN_EVENT_INDEX_PROPERTY_NAME, call_id);
}

PHP_METHOD(Trace, setApiId)
{
    CHECK_AGENT_STARTED;

    zend_long apiId;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &apiId) == FAILURE)
    {
        zend_error(E_ERROR, "setApiId() expects an int.");
        return;
    }

    if (!Pinpoint::Trace::Trace::isStarted())
    {
        RETURN_NULL();
    }

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    PINPOINT_ASSERT (tracePtr != NULL);

    tracePtr->setApiId(apiId);
}

PHP_METHOD(Trace, addAnnotation)
{
    CHECK_AGENT_STARTED;

    zend_long key;
    std::string ts1;

#if PHP_VERSION_ID < 70000

    char* s1 = NULL;
    int s1_len;

    if (!Pinpoint::Trace::Trace::isStarted())
    {
        RETURN_NULL();
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &key, &s1, &s1_len) == FAILURE)
    {
        zend_error(E_ERROR, "addAnnotation() expects (int, string).");
        return;
    }
    ts1 = std::string(s1, (size_t)s1_len);

#else

    zend_string* s1;
    if (!Pinpoint::Trace::Trace::isStarted())
    {
       RETURN_NULL();
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lS", &key, &s1) == FAILURE)
    {
       zend_error(E_ERROR, "addAnnotation() expects (int, (int, string)).");
       return;
    }

    ts1 = std::string(_PSTR(s1), _PSTR_LEN(s1));

#endif

    if (!Pinpoint::Trace::Trace::isStarted())
    {
        RETURN_NULL();
    }

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    PINPOINT_ASSERT (tracePtr != NULL);

    Pinpoint::Trace::AnnotationPtr annotationPtr = Pinpoint::Trace::Annotation::createAnnotation(key);
    if (annotationPtr == NULL)
    {
        LOGE("createAnnotation failed!!!");
        RETURN_NULL();
    }

    annotationPtr->addStringValue(ts1);

    tracePtr->addAnnotationPtr(annotationPtr);

}

PHP_METHOD(Trace, setEndPoint)
{
    CHECK_AGENT_STARTED;

    char* endpoint = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &endpoint) == FAILURE)
    {
        zend_error(E_ERROR, "setEndPoint() expects (string).");
        RETURN_NULL();
    }

    if (!Pinpoint::Trace::Trace::isStarted())
    {
        RETURN_NULL();
    }

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    PINPOINT_ASSERT (tracePtr != NULL);

    tracePtr->setEndPoint(endpoint);
}


PHP_METHOD(Trace, setRemoteAddr)
{
    CHECK_AGENT_STARTED;

    char* remote = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &remote) == FAILURE)
    {
        zend_error(E_ERROR, "setRemoteAddr() expects (string).");
        RETURN_NULL();
    }

    if (!Pinpoint::Trace::Trace::isStarted())
    {
        RETURN_NULL();
    }

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    PINPOINT_ASSERT (tracePtr != NULL);

    tracePtr->setRemoteAddr(remote);
}

PHP_METHOD(Trace, setRpc)
{
    CHECK_AGENT_STARTED;

    char* rpc = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &rpc) == FAILURE)
    {
        zend_error(E_ERROR, "setRpc() expects (string).");
        RETURN_NULL();
    }

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    PINPOINT_ASSERT (tracePtr != NULL);

    if (!Pinpoint::Trace::Trace::isStarted())
    {
        RETURN_NULL();
    }

    tracePtr->setRpc(rpc);
}


PHP_METHOD(Trace, getNextSpanInfo)
{
    CHECK_AGENT_STARTED;

    zval* headers = NULL;
    zval* next_span_id = NULL;

#if PHP_VERSION_ID < 70000
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &headers, &next_span_id) == FAILURE)
    {
        zend_error(E_ERROR, "getNextTraceInfo() expects (headers, nextSpanId).");
        RETURN_FALSE
    }

#else
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z/z/", &headers, &next_span_id) == FAILURE)
    {
        zend_error(E_ERROR, "getNextTraceInfo() expects (headers, nextSpanId).");
        RETURN_FALSE
    }
#endif

    if (!Pinpoint::Trace::Trace::isStarted())
    {
        RETURN_FALSE;
    }

    Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
    if (tracePtr == NULL)
    {
        LOGE("trace == null!!!");
        RETURN_FALSE;
    }

    Pinpoint::Plugin::HeaderMap nextHeader;
    int64_t nextSpanId;
    if (tracePtr->getNextSpanInfo(nextHeader, nextSpanId) != SUCCESS)
    {
        LOGE("tracePtr->getNextSpanInfo failed!!!");
        RETURN_FALSE

    }
    std::string itemstr;

    for(Pinpoint::Plugin::HeaderMap::iterator ip = nextHeader.begin(); ip != nextHeader.end(); ++ip)
    {
        itemstr = ip->first+":"+ip->second;
        call_php_kernel_array_push(headers, itemstr.data(), (uint)itemstr.length());
    }

#ifdef TEST_SIMULATE
    Pinpoint::Plugin::HttpHeader* pHeader = Pinpoint::Plugin::RequestHeader::getCurrentRequestHeader();
    std::string testCasePath;
    if (pHeader->getHeader(Pinpoint::HTTP_TESTCASE_PATH, testCasePath))
    {
        itemstr.clear();
        itemstr = itemstr + Pinpoint::HTTP_TESTCASE_PATH +  ":" + testCasePath;
        call_php_kernel_array_push(headers, itemstr.data(), (uint)itemstr.length());
        LOGI("testCasePath: %s", testCasePath.c_str());
    }else
    {
        LOGE("testCasePath not find ......");
    }
#endif

    ZVAL_LONG(next_span_id, nextSpanId);

    RETURN_TRUE;
}

//</editor-fold>


//<editor-fold desc="PhpInterceptor">

const static char INTERCEPTOR_HANDLER_PROPERTY_NAME[] = "__pinpoint_internal_interceptor_handler";
const static int32_t INTERCEPTOR_HANDLER_PROPERTY_NAME_LEN = sizeof(INTERCEPTOR_HANDLER_PROPERTY_NAME) / sizeof(char) - 1;

typedef enum
{
    PHP_INTERCEPTOR_CREATED = 1,
    PHP_INTERCEPTOR_INITED,
    PHP_INTERCEPTOR_BEFORE,
    PHP_INTERCEPTOR_END,
    PHP_INTERCEPTOR_EXCEPTION
} PhpInterceptorStatus;

class PhpInterceptor : public Pinpoint::Plugin::Interceptor
{
public:
    PhpInterceptor(const std::string& interceptedFuncName);
    int32_t setPhpImplObject(zval* impl, const char* definitionFilePath);
    virtual int32_t init();
    PhpInterceptorStatus getStatus();
    zval* getSelf();
protected:
    virtual void onBefore(uint64_t callId, Pinpoint::Plugin::FuncArgFetcher& argFetcher);
    virtual void onEnd(uint64_t callId, Pinpoint::Plugin::FuncArgFetcher& argFetcher, Pinpoint::Plugin::FuncResultFetcher& resultFetcher);
    virtual void onException(uint64_t callId, const Pinpoint::Plugin::ExceptionInfo& exceptionInfo);

private:
    PObjectCache objectCache;
    PhpInterceptorStatus status;
    zval* self;
};

typedef boost::shared_ptr<PhpInterceptor> PhpInterceptorPtr;
static PhpInterceptorPtr NULL_PHP_INTERCEPTOR_PTR;

PhpInterceptor::PhpInterceptor(const std::string &interceptedFuncName)
        : Pinpoint::Plugin::Interceptor(interceptedFuncName)
{
    status = PHP_INTERCEPTOR_CREATED;
    self = NULL;
}

int32_t PhpInterceptor::setPhpImplObject(zval *impl, const char *definitionFilePath)
{
    return objectCache.store(impl, definitionFilePath);
}

int32_t PhpInterceptor::init()
{
    status = PHP_INTERCEPTOR_INITED;
    return SUCCESS;
}

void PhpInterceptor::onBefore(uint64_t callId, Pinpoint::Plugin::FuncArgFetcher &argFetcher)
{
    const static char call_name[] = "onbefore";
    const static int32_t name_len = sizeof(call_name) / sizeof(char) - 1;

    CHECK_AGENT_STARTED;

    status = PHP_INTERCEPTOR_BEFORE;

    // clean up
    this->self = NULL;

    zval *args = NULL;

    try
    {
        AbstractPHPFuncArgFetcher &phpFuncArgFetcher = dynamic_cast<AbstractPHPFuncArgFetcher &>(argFetcher);
        args = phpFuncArgFetcher.getArgs();
        this->self = phpFuncArgFetcher.getSelf();

    }
    catch (std::exception& e)
    {
        LOGE("get php args throw: e=%s", e.what());
    }

    zval *id = NULL;
    MAKE_STD_ZVAL(id);
    ZVAL_LONG(id, callId);

    // turn off aop
    RunOriginExecute tDpF;

    if (objectCache.callMethod(call_name, name_len, NULL,args ? (2):(1) , id, args) != SUCCESS)
    {
        LOGE("Interceptor name=[%s] onbefore failed!!! please check your code!!!", this->getInterceptedFuncName().c_str());
        Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
        if (tracePtr)
        {
            tracePtr->markBadTrace();
        }
    }

    w_zval_ptr_dtor(id);
}

void PhpInterceptor::onEnd(uint64_t callId, Pinpoint::Plugin::FuncArgFetcher &argFetcher,
                           Pinpoint::Plugin::FuncResultFetcher &resultFetcher)
{
    const static char call_name[] = "onend";
    const static int32_t name_len = sizeof(call_name) / sizeof(char) - 1;

    const static char* args_key_str = "args";
    const static char* result_key_str = "result";

    CHECK_AGENT_STARTED;

    status = PHP_INTERCEPTOR_END;


    zval *id = NULL;
    MAKE_STD_ZVAL(id);
    ZVAL_LONG(id, callId);

    zval *array = NULL;
    MAKE_STD_ZVAL(array);
    array_init_size(array, 2);

    zval *args = NULL;
    zval *result = NULL;
    MAKE_STD_ZVAL(args);
    MAKE_STD_ZVAL(result);
    ZVAL_NULL(args);
    ZVAL_NULL(result);

    try
    {
        AbstractPHPFuncArgFetcher &phpFuncArgFetcher = dynamic_cast<AbstractPHPFuncArgFetcher &>(argFetcher);
        zval* tval = phpFuncArgFetcher.getArgs();
        if(tval){
            ZVAL_ZVAL(args , tval,1,0);
        }
    }
    catch (std::exception& e)
    {
        LOGE("get php args throw: e=%s", e.what());
    }

    add_assoc_zval(array,args_key_str, args);

    try
    {
        AbstractPHPResultFetcher &phpResultFetcher = dynamic_cast<AbstractPHPResultFetcher &>(resultFetcher);
        zval* tval = phpResultFetcher.getResult();
       if(tval){// if exception catched , tval is 0x0
           ZVAL_ZVAL(result , tval,1,0);
       }
    }
    catch (std::exception& e)
    {
        LOGE("get php result throw: e=%s", e.what());
    }

    add_assoc_zval(array, result_key_str, result);

    RunOriginExecute watch;

    if (objectCache.callMethod(call_name, name_len, NULL, 2, id, array) != SUCCESS)
    {
        LOGE("Interceptor name=[%s] onend failed!!! please check your code!!!", this->getInterceptedFuncName().c_str());
        Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
        if (tracePtr)
        {
            tracePtr->markBadTrace();
        }
        //note when exception throw, array refcount is 2
        zval_set_refcount_p(array,1);
    }
    w_zval_ptr_dtor(array);
    w_zval_ptr_dtor(id);

#if PHP_MAJOR_VERSION < 7
    w_zval_ptr_dtor(args);
    w_zval_ptr_dtor(result);
#else
    efree(args);
    efree(result);
#endif

}

void PhpInterceptor::onException(uint64_t callId, const Pinpoint::Plugin::ExceptionInfo &exceptionInfo)
{
    TSRMLS_FETCH();

    const static char call_name[] = "onexception";
    const static int32_t name_len = sizeof(call_name) / sizeof(char) - 1;

    CHECK_AGENT_STARTED;

    status = PHP_INTERCEPTOR_EXCEPTION;

    std::string errorMsg;

    try
    {
        errorMsg = Pinpoint::utils::FormatConverter::getErrorMsg(exceptionInfo.file.c_str(),
                                                       exceptionInfo.line,
                                                       exceptionInfo.message.c_str());
    }
    catch (...)
    {
        errorMsg = exceptionInfo.message;
    }

    zval *id = NULL;
    MAKE_STD_ZVAL(id);
    ZVAL_LONG(id, callId);

#if PHP_VERSION_ID < 70000
    zval* error = NULL;
    ALLOC_INIT_ZVAL(error);
    ZVAL_STRING(error, errorMsg.c_str(), 1);

    RunOriginExecute watch;
    if (objectCache.callMethod(call_name, name_len, NULL, 2, id, error) != SUCCESS)
    {
        LOGE("Interceptor name=[%s] onexception failed!!! please check your code!!!",
             this->getInterceptedFuncName().c_str());
        Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
        if (tracePtr)
        {
            tracePtr->markBadTrace();
        }
    }
    w_zval_ptr_dtor(error);
   
#else
    zval error;
    ZVAL_STRING(&error,errorMsg.c_str());
    // turn off aop
    RunOriginExecute tDpF;
    if (objectCache.callMethod(call_name, name_len, NULL, 2, id, &error) != SUCCESS)
    {
        LOGE("Interceptor name=[%s] onexception failed!!! please check your code!!!",
              this->getInterceptedFuncName().c_str());
        Pinpoint::Trace::TracePtr tracePtr = Pinpoint::Trace::Trace::getCurrentTrace();
        if (tracePtr)
        {
            tracePtr->markBadTrace();
        }
    }
    zval_dtor(&error);
#endif

    w_zval_ptr_dtor(id);

}
PhpInterceptorStatus PhpInterceptor::getStatus()
{
    return this->status;
}

zval* PhpInterceptor::getSelf()
{
    if (status != PHP_INTERCEPTOR_END
        && status != PHP_INTERCEPTOR_BEFORE)
    {
        return NULL;
    }
    return this->self;
}


class PhpInterceptorManager
{
public:
    static PhpInterceptorManager* getManager();
    int32_t addInterceptor(const PhpInterceptorPtr& phpInterceptorPtr);
    PhpInterceptorPtr getInterceptor(int32_t handler);
private:
    static PhpInterceptorManager* instance;
    std::vector<PhpInterceptorPtr> interceptors;
};

const int32_t InvalidPhpInterceptorHandler = -1;

PhpInterceptorManager* PhpInterceptorManager::instance(NULL);

PhpInterceptorManager* PhpInterceptorManager::getManager()
{
    if (instance == NULL)
    {
        try
        {
            instance = new PhpInterceptorManager();
            instance->interceptors.reserve(20);
        }
        catch (...)
        {
            LOGE("new PhpInterfacePluginManager failed.")
        }
    }

    return instance;
}

int32_t PhpInterceptorManager::addInterceptor(const PhpInterceptorPtr &phpInterceptorPtr)
{
    interceptors.push_back(phpInterceptorPtr);
    return (int32_t)interceptors.size() - 1;
}

PhpInterceptorPtr PhpInterceptorManager::getInterceptor(int32_t handler)
{
    if (handler < 0 || (size_t)handler >= interceptors.size())
    {
        return NULL_PHP_INTERCEPTOR_PTR;
    }

    return interceptors[handler];
}

PhpInterceptorPtr getPhpInterceptorPtrByZval(zval* obj)
{
    TSRMLS_FETCH();

    if (!is_class_impl(obj, "Pinpoint\\Interceptor"))
    {
        return NULL_PHP_INTERCEPTOR_PTR;
    }

    zval* z_index = NULL;
    w_zend_read_property(z_index, zend_get_class_entry(obj TSRMLS_CC), obj,
                         INTERCEPTOR_HANDLER_PROPERTY_NAME,
                         INTERCEPTOR_HANDLER_PROPERTY_NAME_LEN, 0);

    if (z_index == NULL || Z_TYPE_P(z_index) != IS_LONG)
    {
        LOGW("getPhpInterceptorPtrByZval get z_index failed!!!");
        return NULL_PHP_INTERCEPTOR_PTR;
    }

    int32_t index = (int32_t)Z_LVAL_P(z_index);

    LOGD("index = %d", index);

    PhpInterceptorManager* manager = PhpInterceptorManager::getManager();
    if (manager != NULL)
    {
        return manager->getInterceptor(index);
    }

    return NULL_PHP_INTERCEPTOR_PTR;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_interceptor_on_before, 0, 0, 2)
    ZEND_ARG_INFO(0, callId)
    ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_interceptor_on_end, 0, 0, 2)
    ZEND_ARG_INFO(0, callId)
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_interceptor_on_exception, 0, 0, 2)
    ZEND_ARG_INFO(0, callId)
    ZEND_ARG_INFO(0, exceptionStr)
ZEND_END_ARG_INFO()


static const zend_function_entry interceptor_methods[] = {
    PHP_ME(Interceptor, onBefore, arginfo_interceptor_on_before, ZEND_ACC_PUBLIC | ZEND_ACC_ABSTRACT)
    PHP_ME(Interceptor, onEnd, arginfo_interceptor_on_end, ZEND_ACC_PUBLIC | ZEND_ACC_ABSTRACT)
    PHP_ME(Interceptor, onException, arginfo_interceptor_on_exception, ZEND_ACC_PUBLIC | ZEND_ACC_ABSTRACT)
    PHP_ME(Interceptor, getSelf, NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


PHP_METHOD(Interceptor, onBefore)
{

}

PHP_METHOD(Interceptor, onEnd)
{

}

PHP_METHOD(Interceptor, onException)
{

}


PHP_METHOD(Interceptor, getSelf)
{
    zval* obj = getThis();

    PhpInterceptorPtr phpInterceptorPtr = getPhpInterceptorPtrByZval(obj);
    if (phpInterceptorPtr == NULL)
    {
        assert (false);
        LOGE("can not find phpInterceptorPtr");
        RETURN_NULL();
    }

    if (phpInterceptorPtr->getStatus() != PHP_INTERCEPTOR_END
        && phpInterceptorPtr->getStatus() != PHP_INTERCEPTOR_BEFORE)
    {
        LOGE("call getself not in before/end method.") ;
        RETURN_NULL();
    }

    zval* self = phpInterceptorPtr->getSelf();

    if (self != NULL)
    {
        RETURN_ZVAL(self, 1, 0);
    }

    RETURN_NULL();
}

//</editor-fold>


//<editor-fold desc="Plugin">

class PhpInterfacePlugin : public Pinpoint::Plugin::Plugin
{
public:
    PhpInterfacePlugin();
    int32_t setObject(zval* impl, const char* definition_path);
    virtual int32_t init();
    virtual std::string getName();
    virtual Pinpoint::Plugin::InterceptorPtrVector& getAllInterceptors();
    virtual ~PhpInterfacePlugin();
    int32_t addInterceptor(const Pinpoint::Plugin::InterceptorPtr& interceptorPtr);
private:
    Pinpoint::Plugin::InterceptorPtrVector interceptors;
    std::string pluginName;
    PObjectCache objectCache;
};

typedef boost::shared_ptr<PhpInterfacePlugin> PhpInterfacePluginPtr;


PhpInterfacePlugin::PhpInterfacePlugin()
{

}

int32_t PhpInterfacePlugin::setObject(zval *impl, const char *definition_path)
{
    TSRMLS_FETCH();

    if (impl != NULL && Z_TYPE_P(impl) == IS_OBJECT)
    {
        zend_class_entry* ce = zend_get_class_entry(impl TSRMLS_CC);

        if (ce != NULL && ce->name != NULL)
        {
            this->pluginName = std::string(_PSTR(ce->name));
        }
    }
    return objectCache.store(impl, definition_path);
}

PhpInterfacePlugin::~PhpInterfacePlugin()
{

}


int32_t PhpInterfacePlugin::init()
{
    return SUCCESS;
}

std::string PhpInterfacePlugin::getName()
{
    return this->pluginName;
}


Pinpoint::Plugin::InterceptorPtrVector& PhpInterfacePlugin::getAllInterceptors()
{
    return interceptors;
}

int32_t PhpInterfacePlugin::addInterceptor(const Pinpoint::Plugin::InterceptorPtr& interceptorPtr)
{
    interceptors.push_back(interceptorPtr);
    return SUCCESS;
}

PhpInterfacePluginManager* PhpInterfacePluginManager::instance(NULL);

PhpInterfacePluginManager* PhpInterfacePluginManager::getManager()
{
    if (instance == NULL)
    {
        try
        {
            instance = new PhpInterfacePluginManager();
            instance->plugins.reserve(20);
        }
        catch (...)
        {
            LOGE("PhpInterfacePluginManager failed.");
        }
    }

    return instance;
}

PhpInterfacePluginHandler PhpInterfacePluginManager::registerPlugin(const Pinpoint::Plugin::PluginPtr &phpPluginPtr)
{
    plugins.push_back(phpPluginPtr);
    return (PhpInterfacePluginHandler)(plugins.size() - 1);
}

Pinpoint::Plugin::PluginPtr PhpInterfacePluginManager::getPhpPluginPtr(PhpInterfacePluginHandler handler)
{
    if (plugins.size() - 1 < (size_t)handler || handler < 0)
    {
        return Pinpoint::Plugin::PluginPtr();
    }

    return plugins[handler];
}


Pinpoint::Plugin::PluginPtrVector& PhpInterfacePluginManager::getAllPlugins()
{
    return plugins;
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_pulgin_add_interceptor, 0, 0, 1)
    ZEND_ARG_INFO(0, interceptor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pulgin_add_simple_interceptor, 0, 0, 2)
    ZEND_ARG_INFO(0, interceptorFuncName)
    ZEND_ARG_INFO(0, line)
ZEND_END_ARG_INFO()

static const zend_function_entry plugin_methods[] = {
    PHP_ME(Plugin, __construct, NULL, ZEND_ACC_PROTECTED)
    PHP_ME(Plugin, addInterceptor, arginfo_pulgin_add_interceptor, ZEND_ACC_PUBLIC)
    PHP_ME(Plugin, addSimpleInterceptor, arginfo_pulgin_add_simple_interceptor, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


const static char PLUGIN_HANDLER_PROPERTY_NAME[] = "__pinpoint_internal_handler";
const static int32_t PLUGIN_HANDLER_PROPERTY_NAME_LEN = sizeof(PLUGIN_HANDLER_PROPERTY_NAME) / sizeof(char) - 1;

PHP_METHOD(Plugin, __construct)
{
    CHECK_AGENT_PRE_INITED;

    zval* object = getThis();
    PINPOINT_ASSERT(object != NULL);

    PhpInterfacePluginPtr phpInterfacePluginPtr;
    PhpInterfacePluginHandler handler = InvalidPhpPluginHandler;

    try
    {
        phpInterfacePluginPtr.reset(new PhpInterfacePlugin());
    }
    catch (...)
    {
        handler = InvalidPhpPluginHandler;
        LOGE("create phpInterfacePluginPtr failed!!!");
        return;
    }

    PhpInterfacePluginManager* manager = PhpInterfacePluginManager::getManager();
    if (manager != NULL && phpInterfacePluginPtr != NULL)
    {
        handler = manager->registerPlugin(boost::dynamic_pointer_cast<Pinpoint::Plugin::Plugin>(phpInterfacePluginPtr));
    }

    // add_property_long_ex append 1 to key len
    // avoiding to confuse in future , add_property_long_ex replace with add_property_long
    add_property_long(object, PLUGIN_HANDLER_PROPERTY_NAME, handler);

}


PHP_METHOD(Plugin, addInterceptor)
{
    CHECK_AGENT_PRE_INITED;
    std::string TinterceptorFuncName;
    std::string Tdefinition_path;


#if PHP_VERSION_ID < 70000

    zval* obj;
    char *interceptorFuncName;
    int interceptorFuncNameLen;
    char *definition_path;
    int definition_path_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,  "o"STR_SPEC""STR_SPEC,
                              &obj,
                              &interceptorFuncName, &interceptorFuncNameLen,
                              &definition_path, &definition_path_len) == FAILURE)
    {
        zend_error(E_ERROR, "pinpoint_add_plugin() expects (interceptor object, string, string).");
        return;
    }
    TinterceptorFuncName = std::string(interceptorFuncName, (size_t)interceptorFuncNameLen);
    Tdefinition_path =  std::string(definition_path, (size_t)definition_path_len);
#else

    zval* obj;
    zend_string* interceptorFuncName;
    zend_string* definition_path;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o"STR_SPEC""STR_SPEC,
                              &obj,
                              &interceptorFuncName, &definition_path) == FAILURE)
    {
        zend_error(E_ERROR, "pinpoint_add_plugin() expects (interceptor object, string, string).");
        return;
    }

    TinterceptorFuncName = std::string(interceptorFuncName->val, (size_t)interceptorFuncName->len);
    Tdefinition_path =  std::string(definition_path->val, (size_t)definition_path->len);
#endif


    if (!is_class_impl(obj, "Pinpoint\\Interceptor"))
    {
        zend_error(E_ERROR, "addInterceptor() expects (interceptor object, string, string).");
        return;
    }

    zval* object = getThis();
    PINPOINT_ASSERT (object != NULL);

    zval* handler = NULL;
    w_zend_read_property(handler,zend_get_class_entry(object TSRMLS_CC), object,
                                       PLUGIN_HANDLER_PROPERTY_NAME, PLUGIN_HANDLER_PROPERTY_NAME_LEN, 0);
    if (handler == NULL || Z_TYPE_P(handler) != IS_LONG)
    {
        zend_error(E_ERROR, "Please call Plugin's constructor at derived class' constructor.");
        return;
    }

    PhpInterfacePluginHandler phpInterfacePluginHandler = (PhpInterfacePluginHandler)Z_LVAL_P(handler);
    PhpInterfacePluginManager* manager = PhpInterfacePluginManager::getManager();
    if (manager == NULL)
    {
        return;
    }

    PhpInterfacePluginPtr phpInterfacePluginPtr =
            boost::dynamic_pointer_cast<PhpInterfacePlugin>(manager->getPhpPluginPtr(phpInterfacePluginHandler));
    if (phpInterfacePluginPtr == NULL)
    {
        LOGE("get phpInterfacePluginPtr failed. phpInterfacePluginHandler=%d.", phpInterfacePluginHandler);
        return;
    }

    PhpInterceptorPtr interceptorPtr;
    try
    {
        interceptorPtr.reset(new PhpInterceptor(TinterceptorFuncName));
    }
    catch (...)
    {
        LOGE("new PhpInterceptor failed. interceptorFuncName=%s", TinterceptorFuncName.c_str());
        return;
    }

    int32_t interceptorHandler = InvalidPhpInterceptorHandler;
    PhpInterceptorManager* interceptorManager = PhpInterceptorManager::getManager();
    if (interceptorManager != NULL)
    {
        interceptorHandler = interceptorManager->addInterceptor(interceptorPtr);
    }

    add_property_long(obj, INTERCEPTOR_HANDLER_PROPERTY_NAME, interceptorHandler);

    PINPOINT_ASSERT(Pinpoint::Agent::PinpointAgentContext::getContextPtr() != NULL);
    Pinpoint::Agent::AgentConfigArgsPtr contextPtr =
               Pinpoint::Agent::PinpointAgentContext::getContextPtr()->agentConfigArgsPtr;

    PINPOINT_ASSERT(contextPtr != NULL);
    interceptorPtr->setPhpImplObject(obj, path_join(contextPtr->pluginDir, Tdefinition_path).c_str());

    phpInterfacePluginPtr->addInterceptor(interceptorPtr);
}


PHP_METHOD(Plugin, addSimpleInterceptor)
{
    CHECK_AGENT_PRE_INITED;

    std::string funcName;
    zend_long lineno;

#if PHP_VERSION_ID < 70000
    char *interceptorFuncName;
    int interceptorFuncNameLen;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl",
                              &interceptorFuncName, &interceptorFuncNameLen, &lineno) == FAILURE)
    {
        zend_error(E_ERROR, "pinpoint_add_api() expects (string (api info), int(line number)).");
        return;
    }
    funcName = std::string(interceptorFuncName, (size_t)interceptorFuncNameLen);
#else
    zend_string* interceptorFuncName;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Sl", &interceptorFuncName, &lineno) == FAILURE)
    {
        zend_error(E_ERROR, "pinpoint_add_api() expects (string (api info), int(line number)).");
        return;
    }
    funcName = std::string(interceptorFuncName->val, (size_t)interceptorFuncName->len);
#endif

    Pinpoint::Plugin::InterceptorPtr interceptorPtr;
    try
    {
        interceptorPtr.reset(new SimpleInterceptor(funcName, lineno));
    }
    catch (...)
    {
        LOGE("create %s interceptor failed.", funcName.c_str());
        return;
    }

    zval* object = getThis();
    PINPOINT_ASSERT (object != NULL);

    zval* handler = NULL;

    w_zend_read_property(handler,zend_get_class_entry(object TSRMLS_CC), object,
         PLUGIN_HANDLER_PROPERTY_NAME, PLUGIN_HANDLER_PROPERTY_NAME_LEN, 0);

    if (handler == NULL || Z_TYPE_P(handler) != IS_LONG)
    {
        zend_error(E_ERROR, "Please call Plugin's constructor at derived class' constructor.");
        return;
    }

    PhpInterfacePluginHandler phpInterfacePluginHandler = (PhpInterfacePluginHandler)Z_LVAL_P(handler);
    PhpInterfacePluginManager* manager = PhpInterfacePluginManager::getManager();
    if (manager == NULL)
    {
        return;
    }

    PhpInterfacePluginPtr phpInterfacePluginPtr =
            boost::dynamic_pointer_cast<PhpInterfacePlugin>(manager->getPhpPluginPtr(phpInterfacePluginHandler));
    if (phpInterfacePluginPtr == NULL)
    {
        LOGE("get phpInterfacePluginPtr failed. phpInterfacePluginHandler=%d.", phpInterfacePluginHandler);
        assert (false);
        return;
    }

    phpInterfacePluginPtr->addInterceptor(interceptorPtr);
}


//<editor-fold desc="Global">
PHP_FUNCTION(pinpoint_start_trace)
{
    CHECK_AGENT_STARTED;

    if (Pinpoint::Trace::Trace::isStarted())
    {
        LOGE("trace has been started!!!!");
        RETURN_NULL();
    }

    if(object_init_ex(return_value, ce_trace) != SUCCESS)
    {
        LOGE("create PHP trace object failed.");
        RETURN_NULL();
    }

    int32_t err = Pinpoint::Trace::Trace::startTrace(NULL);
    if (err != SUCCESS)
    {
        LOGE("start trace failed.")
        RETURN_NULL();
    }

}

PHP_FUNCTION(pinpoint_get_current_trace)
{
    CHECK_AGENT_STARTED;

    if (!Pinpoint::Trace::Trace::isStarted())
    {
        RETURN_NULL();
    }


    if(object_init_ex(return_value, ce_trace) != SUCCESS)
    {
        LOGE("create php trace object failed");
        RETURN_NULL();
    }
}

PHP_FUNCTION(pinpoint_end_trace)
{
    CHECK_AGENT_STARTED;

    if (!Pinpoint::Trace::Trace::isStarted())
    {
        LOGE("trace does not start!!!")
    }

    int32_t err = Pinpoint::Trace::Trace::endTrace();
    if (err != SUCCESS)
    {
        LOGE("end trace failed.")
    }
}

PHP_FUNCTION(pinpoint_add_api)
{
    CHECK_AGENT_PRE_INITED;

#if PHP_VERSION_ID < 70000
    char *api_info;
    int api_info_len;
    int lineno;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &api_info, &api_info_len, &lineno) == FAILURE)
    {
        zend_error(E_ERROR, "pinpoint_add_api() expects api info(string) and line number(int).");
        return;
    }

#else
    zend_string* api_info;
    zend_long lineno;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Sl", &api_info, &lineno) == FAILURE)
    {
        zend_error(E_ERROR, "pinpoint_add_api() expects api info(string) and line no(int).");
        return;
    }

#endif

    Pinpoint::Agent::AgentPtr agentPtr = Pinpoint::Agent::Agent::getAgentPtr();
    if (agentPtr == NULL)
    {
        assert (false);
        LOGE("get agentPtr == NULL!!!");
        RETVAL_LONG(Pinpoint::Agent::INVALID_API_ID);
    }

    int32_t apiid = agentPtr->addApi(_PSTR(api_info), lineno);

    RETURN_LONG(apiid);
}

PHP_FUNCTION(pinpoint_add_string)
{
    CHECK_AGENT_PRE_INITED;
}

PHP_FUNCTION(pinpoint_add_plugin)
{
    CHECK_AGENT_PRE_INITED;

#if PHP_VERSION_ID < 70000

    zval* object;
    char *definition_path;
    int definition_path_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o"STR_SPEC, &object, &definition_path, &definition_path_len) == FAILURE)
    {
        zend_error(E_ERROR, "pinpoint_add_plugin() expects an plugin object and the file path of its definition.");
        return;
    }

    std::string Tdefinition_path =std::string(definition_path, (size_t)definition_path_len);
    PINPOINT_ASSERT (object != NULL);

#else
    zval* object;
    zend_string* definition_path;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o"STR_SPEC, &object, &definition_path) == FAILURE)
    {
        zend_error(E_ERROR, "pinpoint_add_plugin() expects an plugin object and the file path of its definition.");
        return;
    }

    std::string Tdefinition_path =std::string(definition_path->val, (size_t)definition_path->len);
    PINPOINT_ASSERT (object != NULL);

#endif

    zval* handler = NULL;
    w_zend_read_property(handler,zend_get_class_entry(object TSRMLS_CC), object,
                                       PLUGIN_HANDLER_PROPERTY_NAME, PLUGIN_HANDLER_PROPERTY_NAME_LEN, 0);
    if (handler == NULL || Z_TYPE_P(handler) != IS_LONG)
    {
        zend_error(E_ERROR, "Please call Plugin's constructor at derived class' constructor.");
        return;
    }

    PhpInterfacePluginHandler phpInterfacePluginHandler = (PhpInterfacePluginHandler)Z_LVAL_P(handler);
    PhpInterfacePluginManager* manager = PhpInterfacePluginManager::getManager();
    if (manager == NULL)
    {
        return;
    }

    PhpInterfacePluginPtr phpInterfacePluginPtr =
            boost::dynamic_pointer_cast<PhpInterfacePlugin>(manager->getPhpPluginPtr(phpInterfacePluginHandler));
    if (phpInterfacePluginPtr == NULL)
    {
        LOGE("get phpInterfacePluginPtr failed. phpInterfacePluginHandler=%d.", phpInterfacePluginHandler);
        assert (false);
        return;
    }

    PINPOINT_ASSERT(Pinpoint::Agent::PinpointAgentContext::getContextPtr() != NULL);
    Pinpoint::Agent::AgentConfigArgsPtr contextPtr =
             Pinpoint::Agent::PinpointAgentContext::getContextPtr()->agentConfigArgsPtr;

    PINPOINT_ASSERT(contextPtr != NULL);
    phpInterfacePluginPtr->setObject(object, path_join(contextPtr->pluginDir, Tdefinition_path).c_str());
}

PHP_FUNCTION(pinpoint_log)
{
    CHECK_AGENT_PRE_INITED;

    zend_long level;
    std::string tinfo;
#if PHP_VERSION_ID < 70000
    char *info;
    int info_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &level, &info, &info_len) == FAILURE)
    {
        zend_error(E_ERROR, "pinpoint_log(level, information)");
        return;
    }
    tinfo = std::string(info, (size_t)info_len);
#else
    zend_string* info;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lS", &level, &info) == FAILURE)
    {
        zend_error(E_ERROR, "pinpoint_log(level, information)");
        return;
    }
    tinfo = std::string(info->val, (size_t)info->len);
#endif

    const char *error_filename;
    uint error_lineno;

    if (zend_is_executing(TSRMLS_C))
    {
        error_filename = zend_get_executed_filename(TSRMLS_C);
        error_lineno = zend_get_executed_lineno(TSRMLS_C);
    }
    else
    {
        error_filename = "";
        error_lineno = 0;
    }

    printf_to_stderr_ex(level, error_filename, error_lineno, tinfo.c_str());
}

int32_t register_pinpoint_class()
{
    TSRMLS_FETCH();

    int32_t err;

    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Pinpoint\\Trace", trace_methods);
    ce_trace = zend_register_internal_class(&ce TSRMLS_CC);

    INIT_CLASS_ENTRY(ce, "Pinpoint\\SpanEventRecorder", span_event_recorder_methods);
    ce_span_event_recorder = zend_register_internal_class(&ce TSRMLS_CC);

    err =  w_zend_declare_property_long(ce_span_event_recorder,
                                        SPAN_EVENT_INDEX_PROPERTY_NAME,
                                        SPAN_EVENT_INDEX_PROPERTY_NAME_LEN,
                                        -1, ZEND_ACC_PUBLIC TSRMLS_CC);
    PINPOINT_ASSERT_RETURN((SUCCESS == err), Pinpoint::FAILED );

    INIT_CLASS_ENTRY(ce, "Pinpoint\\Plugin", plugin_methods);
    ce_plugin = zend_register_internal_class(&ce TSRMLS_CC);

    err =  w_zend_declare_property_long(ce_plugin,
                                        PLUGIN_HANDLER_PROPERTY_NAME,
                                        PLUGIN_HANDLER_PROPERTY_NAME_LEN,
                                        InvalidPhpPluginHandler, ZEND_ACC_PUBLIC TSRMLS_CC);
    PINPOINT_ASSERT_RETURN((SUCCESS == err), Pinpoint::FAILED );

    INIT_CLASS_ENTRY(ce, "Pinpoint\\Interceptor", interceptor_methods);
    ce_interceptor = zend_register_internal_class(&ce TSRMLS_CC);

    err =  w_zend_declare_property_long(ce_interceptor,INTERCEPTOR_HANDLER_PROPERTY_NAME,INTERCEPTOR_HANDLER_PROPERTY_NAME_LEN,
            InvalidPhpInterceptorHandler, ZEND_ACC_PUBLIC TSRMLS_CC);
    PINPOINT_ASSERT_RETURN((SUCCESS == err), Pinpoint::FAILED );

    return SUCCESS;
}

//</editor-fold>

