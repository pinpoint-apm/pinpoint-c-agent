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

#include "zend_types.h"
#ifdef __cplusplus

extern "C" {
#endif // __cplusplus

extern zend_module_entry pinpoint_php_module_entry;
#define phpext_pinpoint_php_ptr &pinpoint_php_module_entry

#define PHP_PINPOINT_PHP_VERSION "0.5.2"

#ifdef PHP_WIN32
#define PHP_PINPOINT_PHP_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#define PHP_PINPOINT_PHP_API __attribute__((visibility("default")))
#else
#define PHP_PINPOINT_PHP_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(pinpoint_php)
char *co_host; // tcp:ip:port should support dns
zend_bool utest_flag;
zend_bool debug_report;
#if PHP_VERSION_ID >= 70000
zend_long tracelimit;
zend_long w_timeout_ms;
#else
int tracelimit;
int w_timeout_ms;
#endif
HashTable *interceptors;
ZEND_END_MODULE_GLOBALS(pinpoint_php)

extern ZEND_DECLARE_MODULE_GLOBALS(pinpoint_php);

#if PHP_VERSION_ID < 70200
typedef void (*zif_handler)(INTERNAL_FUNCTION_PARAMETERS);
#endif

typedef struct {
  zval before;
  zval end;
  zval exception;
  zif_handler origin;
  zend_function *origin_func;
  zend_string *name;
} pp_interceptor_v_t;

#ifdef ZTS
#define PPG(v) TSRMG(pinpoint_php_globals_id, zend_pinpoint_php_globals *, v)
#else
#define PPG(v) (pinpoint_php_globals.v)
#endif

#if PHP_VERSION_ID > 70000
#if defined(ZTS) && defined(COMPILE_DL_PINPOINT_PHP)
ZEND_TSRMLS_CACHE_EXTERN()
#endif
#endif

PHP_FUNCTION(_pinpoint_start_trace);
PHP_FUNCTION(_pinpoint_end_trace);
PHP_FUNCTION(_pinpoint_add_clue);
PHP_FUNCTION(_pinpoint_add_clues);
PHP_FUNCTION(_pinpoint_unique_id);
PHP_FUNCTION(pinpoint_get_this);
PHP_FUNCTION(pinpoint_get_caller_arg);
PHP_FUNCTION(_pinpoint_trace_limit);
PHP_FUNCTION(_pinpoint_drop_trace);
PHP_FUNCTION(_pinpoint_start_time);
PHP_FUNCTION(_pinpoint_set_context);
PHP_FUNCTION(_pinpoint_get_context);
PHP_FUNCTION(_pinpoint_mark_as_error);
// DEPRECATED("pinpoint_get_func_ref_args only used for compatiblity")
// PHP_FUNCTION(pinpoint_get_func_ref_args);
PHP_FUNCTION(pinpoint_status);
PHP_FUNCTION(_pinpoint_join_cut);

PHP_MINIT_FUNCTION(pinpoint_php);
PHP_MSHUTDOWN_FUNCTION(pinpoint_php);
PHP_RINIT_FUNCTION(pinpoint_php);
PHP_RSHUTDOWN_FUNCTION(pinpoint_php);
PHP_MINFO_FUNCTION(pinpoint_php);

#ifdef __cplusplus
} // extern C
#endif // __cplusplus

#endif /* PHP_PINPOINT_PHP_H */
