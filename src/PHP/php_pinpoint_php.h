/*******************************************************************************
 * Copyright 2019 NAVER Corp
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
#ifndef PHP_PINPOINT_PHP_H
#define PHP_PINPOINT_PHP_H

#include "common.h"
#include "config.h"

extern zend_module_entry pinpoint_php_module_entry;
#define phpext_pinpoint_php_ptr &pinpoint_php_module_entry

#define PHP_PINPOINT_PHP_VERSION "0.2.0"

#ifdef PHP_WIN32
#	define PHP_PINPOINT_PHP_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_PINPOINT_PHP_API __attribute__ ((visibility("default")))
#else
#	define PHP_PINPOINT_PHP_API
#endif


#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(pinpoint_php)
//    uint  w_timeout_ms;
    char*  co_host; // tcp:ip:port should support dns
    zend_bool   utest_flag;
//    zend_bool   limit;
    zend_bool    debug_report;
    int   tracelimit;
ZEND_END_MODULE_GLOBALS(pinpoint_php)

extern ZEND_DECLARE_MODULE_GLOBALS(pinpoint_php);

#ifdef ZTS
#define PPG(v) TSRMG(pinpoint_php_globals_id, zend_pinpoint_php_globals *, v)
#else
#define PPG(v) (pinpoint_php_globals.v)
#endif

#if defined(ZTS) && defined(COMPILE_DL_PINPOINT_PHP)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

PHP_MINIT_FUNCTION(pinpoint_php);
PHP_MSHUTDOWN_FUNCTION(pinpoint_php);
PHP_RINIT_FUNCTION(pinpoint_php);
PHP_RSHUTDOWN_FUNCTION(pinpoint_php);
PHP_MINFO_FUNCTION(pinpoint_php);
#endif	/* PHP_PINPOINT_PHP_H */
