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
#ifndef PHP_PINPOINT_H
#define PHP_PINPOINT_H

#if defined(__cplusplus)||defined(c_plusplus)
extern "C"{
#endif

#include <main/php.h>
#include "zend_modules.h"

extern zend_module_entry pinpoint_module_entry;

#ifdef PHP_WIN32
#   define PHP_PINPOINT_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#   define PHP_PINPOINT_API __attribute__ ((visibility("default")))
#else
#   define PHP_PINPOINT_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define PHP_PINPOINT_VERSION "0.1"
#define PHP_PINPOINT_EXTNAME "pinpoint"

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(pinpoint);
PHP_MSHUTDOWN_FUNCTION(pinpoint);
PHP_RINIT_FUNCTION(pinpoint);
PHP_RSHUTDOWN_FUNCTION(pinpoint);


typedef struct _per_reqeust_state_{
    zend_bool fatal_error_catched;
}PRS;

ZEND_BEGIN_MODULE_GLOBALS(pinpoint)
    zend_bool ignExp;
    char* configFileName;
    PRS prs;// must reset in every request,valid in reqeust
ZEND_END_MODULE_GLOBALS(pinpoint)

extern ZEND_DECLARE_MODULE_GLOBALS(pinpoint);

#ifdef ZTS
#define PINPOINT_G(v) TSRMG(pinpoint_globals_id, zend_pinpoint_globals *, v)
#else
#define PINPOINT_G(v) (pinpoint_globals.v)
#endif

#if defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif /* PHP_XHPROF_H */
