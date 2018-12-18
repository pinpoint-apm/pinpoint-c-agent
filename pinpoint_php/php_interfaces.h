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
#ifndef PINPOINT_PHP_INTERFACES_H
#define PINPOINT_PHP_INTERFACES_H

#include <pinpoint_api.h>
#include "php.h"

PHP_METHOD(SpanEventRecorder, __construct);
PHP_METHOD(SpanEventRecorder, markBeforeTime);
PHP_METHOD(SpanEventRecorder, markAfterTime);
PHP_METHOD(SpanEventRecorder, setApiId);
PHP_METHOD(SpanEventRecorder, setServiceType);
PHP_METHOD(SpanEventRecorder, addAnnotation);
PHP_METHOD(SpanEventRecorder, setEndPoint);
PHP_METHOD(SpanEventRecorder, setDestinationId);
PHP_METHOD(SpanEventRecorder, setRpc);
PHP_METHOD(SpanEventRecorder, setNextSpanId);
PHP_METHOD(SpanEventRecorder, setExceptionInfo);

PHP_METHOD(Trace, __construct);
PHP_METHOD(Trace, traceBlockBegin);
PHP_METHOD(Trace, traceBlockEnd);
PHP_METHOD(Trace, getEvent);
PHP_METHOD(Trace, setApiId);
PHP_METHOD(Trace, addAnnotation);
PHP_METHOD(Trace, setEndPoint);
PHP_METHOD(Trace, setRemoteAddr);
PHP_METHOD(Trace, setRpc);
PHP_METHOD(Trace, getNextSpanInfo);

PHP_METHOD(Plugin, __construct);
PHP_METHOD(Plugin, init);
PHP_METHOD(Plugin, addInterceptor);
PHP_METHOD(Plugin, addSimpleInterceptor);

PHP_METHOD(Interceptor, init);
PHP_METHOD(Interceptor, onBefore);
PHP_METHOD(Interceptor, onEnd);
PHP_METHOD(Interceptor, onException);
PHP_METHOD(Interceptor, getSelf);


//PHP_FUNCTION(pinpoint_start_trace);
PHP_FUNCTION(pinpoint_get_current_trace);
PHP_FUNCTION(pinpoint_end_trace);
PHP_FUNCTION(pinpoint_add_plugin);
PHP_FUNCTION(pinpoint_add_api);
PHP_FUNCTION(pinpoint_log);


PHP_FUNCTION(pinpint_aop_reload);

PHP_FUNCTION(pinpoint_start_calltree);
PHP_FUNCTION(pinpoint_end_calltree);



typedef int32_t PhpInterfacePluginHandler;
static const PhpInterfacePluginHandler InvalidPhpPluginHandler = -1;

class PhpInterfacePluginManager
{
public:
    static PhpInterfacePluginManager* getManager();
    Pinpoint::Plugin::PluginPtr getPhpPluginPtr(PhpInterfacePluginHandler handler);
    PhpInterfacePluginHandler registerPlugin(const Pinpoint::Plugin::PluginPtr& phpPluginPtr);
    Pinpoint::Plugin::PluginPtrVector& getAllPlugins();
private:
    static PhpInterfacePluginManager* instance;
    Pinpoint::Plugin::PluginPtrVector plugins;
};

extern int32_t register_pinpoint_class();

#endif
