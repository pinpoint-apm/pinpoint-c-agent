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
#ifndef PINPOINT_PHP_AOP_H
#define PINPOINT_PHP_AOP_H

#include "php_function_helper.h"
#include "interceptor.h"


extern int32_t init_aop();
extern int32_t turn_on_aop();
extern int32_t turn_off_aop();
extern int32_t add_interceptor(const Pinpoint::Plugin::InterceptorPtr& interceptorPtr);

void apm_throw_exception_hook(EG_EXP_TPYE exception TSRMLS_DC);

class AbstractPHPFuncArgFetcher : public Pinpoint::Plugin::FuncArgFetcher
{
public:
    virtual int32_t setInArgs(pt_frame_t &pt) = 0;
    virtual zval* getArgs() = 0;
    virtual zval* getSelf() = 0;
};

class AbstractPHPResultFetcher : public Pinpoint::Plugin::FuncResultFetcher
{
public:
    virtual zval* getResult() = 0;
};



#endif
