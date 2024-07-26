////////////////////////////////////////////////////////////////////////////////
// Copyright 2019 NAVER Corp
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
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */
// clang-format off
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <iostream>
#include <string>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdio.h>
#include <cinttypes>

#include "zend_API.h"
#include "zend_string.h"
#include "zend_types.h"
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "common.h"
#include "php_pinpoint_php.h"

// clang-format on
#ifdef COMPILE_DL_PINPOINT_PHP
#ifdef ZTS
#if PHP_VERSION_ID >= 70000 && PHP_VERSION_ID < 80000
ZEND_TSRMLS_CACHE_DEFINE()
#else
#include "TSRM.h"
#endif
#endif
ZEND_GET_MODULE(pinpoint_php)
#endif

ZEND_DECLARE_MODULE_GLOBALS(pinpoint_php);
static void pinpoint_log(char *msg);

// clang-format off
/* {{{ PHP_INI
 */

PHP_INI_BEGIN()

STD_PHP_INI_ENTRY("pinpoint_php.SendSpanTimeOutMs", "0", PHP_INI_ALL,
                  OnUpdateLong, w_timeout_ms, zend_pinpoint_php_globals,
                  pinpoint_php_globals)

STD_PHP_INI_ENTRY("pinpoint_php.CollectorHost", "unix:/tmp/collector.sock",
                  PHP_INI_ALL, OnUpdateString, co_host,
                  zend_pinpoint_php_globals, pinpoint_php_globals)

STD_PHP_INI_ENTRY("pinpoint_php.UnitTest", "no", PHP_INI_ALL, OnUpdateBool,
                  utest_flag, zend_pinpoint_php_globals, pinpoint_php_globals)

STD_PHP_INI_ENTRY("pinpoint_php.TraceLimit", "-1", PHP_INI_ALL, OnUpdateLong,
                  tracelimit, zend_pinpoint_php_globals, pinpoint_php_globals)

STD_PHP_INI_ENTRY("pinpoint_php.DebugReport", "no", PHP_INI_ALL, OnUpdateBool,
                  debug_report, zend_pinpoint_php_globals, pinpoint_php_globals)
PHP_INI_END()

/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_add_id_key_value, 0, 0, 2)
ZEND_ARG_INFO(0, key)
ZEND_ARG_INFO(0, value)
ZEND_ARG_INFO(0, nodeid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_add_id_key_value_flag, 0, 0, 2)
ZEND_ARG_INFO(0, key)
ZEND_ARG_INFO(0, value)
ZEND_ARG_INFO(0, nodeid)
ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_add_join_cb_cb_cb, 0, 0, 4)
ZEND_ARG_INFO(0, joinable)
ZEND_ARG_INFO(0, onBefore)
ZEND_ARG_INFO(0, onEnd)
ZEND_ARG_INFO(0, onException)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_add_msg_filename_lineno_id, 0, 0, 2)
ZEND_ARG_INFO(0, msg)
ZEND_ARG_INFO(0, filename)
ZEND_ARG_INFO(0, lineno)
ZEND_ARG_INFO(0, nodeid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_add_timestamp, 0, 0, 0)
ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_add_arg_index, 0, 0, 0)
ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_add_id, 0, 0, 0)
ZEND_ARG_INFO(0, nodeid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_add_id_value, 0, 0, 1)
ZEND_ARG_INFO(0, key)
ZEND_ARG_INFO(0, nodeid)
ZEND_END_ARG_INFO()
// php5 needs (0,0,0)
ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, 0, 0)
ZEND_END_ARG_INFO()

/* {{{ pinpioint_php_functions[]
 *
 * Every user visible function must have an entry in pinpioint_php_functions[].
 */
const zend_function_entry pinpoint_php_functions[] = {
  PHP_FE(_pinpoint_start_trace, arginfo_add_id) 
  PHP_FE(_pinpoint_end_trace, arginfo_add_id)
  PHP_FE(_pinpoint_unique_id, arginfo_none) 
  PHP_FE(pinpoint_get_this, arginfo_none) 
  PHP_FE(pinpoint_get_caller_arg,arginfo_add_arg_index)
  PHP_FE(pinpoint_status, arginfo_none) 
  // PHP__FE(pinpoint_get_func_ref_args, arginfo_none)
  PHP_FE(_pinpoint_drop_trace, arginfo_add_id) 
  PHP_FE(_pinpoint_start_time, arginfo_none)
  PHP_FE(_pinpoint_set_context, arginfo_add_id_key_value)
  PHP_FE(_pinpoint_get_context, arginfo_add_id_value)
  PHP_FE(_pinpoint_trace_limit, arginfo_add_timestamp)
  PHP_FE(_pinpoint_mark_as_error, arginfo_add_msg_filename_lineno_id)
  PHP_FE(_pinpoint_add_clue, arginfo_add_id_key_value_flag)
  PHP_FE(_pinpoint_add_clues, arginfo_add_id_key_value_flag)
  PHP_FE(_pinpoint_join_cut,arginfo_add_join_cb_cb_cb)
  PHP_FE_END /* Must be the last line in pinpioint_php_functions[] */
};
/* }}} */
// clang-format on
/* {{{ pinpioint_php_module_entry
 */
zend_module_entry pinpoint_php_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "pinpoint_php",
    pinpoint_php_functions,
    PHP_MINIT(pinpoint_php),
    PHP_MSHUTDOWN(pinpoint_php),
    PHP_RINIT(pinpoint_php), /* Replace with NULL if there's nothing to do at
                                request start */
    PHP_RSHUTDOWN(pinpoint_php), /* Replace with NULL if there's nothing to do
                                    at request end */
    PHP_MINFO(pinpoint_php),
#if ZEND_MODULE_API_NO >= 20010901
    PHP_PINPOINT_PHP_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES};
/* }}} */
#if PHP_VERSION_ID >= 80100
void (*old_error_cb)(int type, zend_string *error_filename,
                     const uint32_t error_lineno, zend_string *message);
#elif PHP_VERSION_ID >= 80000
void (*old_error_cb)(int type, const char *error_filename,
                     const uint32_t error_lineno, zend_string *message);
#else
void (*old_error_cb)(int type, const char *error_filename,
                     const uint32_t error_lineno, const char *format,
                     va_list args);
#endif

#define safe_free(x)                                                           \
  if ((x)) {                                                                   \
    free((x));                                                                 \
    (x) = NULL;                                                                \
  }

PHP_FUNCTION(_pinpoint_drop_trace) {
  NodeID id = E_ROOT_NODE, cur_id = E_ROOT_NODE;
#if PHP_VERSION_ID < 70000
  size_t _id = -1;
  zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &_id);
#else
  zend_long _id = -1;
  zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &_id);
#endif
  if (_id == -1) {
    id = pinpoint_get_per_thread_id();
  } else {
    id = (NodeID)_id;
  }
  change_trace_status(id, E_TRACE_BLOCK);
  RETURN_TRUE;
}

PHP_FUNCTION(pinpoint_get_this) {
  if (EX(prev_execute_data) && EX(prev_execute_data)->prev_execute_data) {
    zval *self = &EX(prev_execute_data)->prev_execute_data->This;
    if (zval_get_type(self) != IS_UNDEF) {
      RETURN_ZVAL(self, 1, 0);
    }
  }
  pp_trace("pinpoint_get_this: not support, no this");
  RETURN_FALSE;
}

PHP_FUNCTION(_pinpoint_set_context) {

  std::string key;
  zval *zvalue;
#if PHP_VERSION_ID < 70000
  char *zkey = NULL;
  int zkey_len;
  size_t _id = -1;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|l", &zkey, &zkey_len,
                            &zvalue, &_id) == FAILURE) {
    zend_error(E_ERROR, "key/value required");
    return;
  }
  key = std::string(zkey, zkey_len);

#else
  zend_long _id = -1;
  zend_string *zkey;
  if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz|l", &zkey, &zvalue, &_id) ==
      FAILURE) {
    zend_error(E_ERROR, "key/value required");
    return;
  }
  key = std::string(zkey->val, zkey->len);
#endif

  if (_id == -1) {
    _id = pinpoint_get_per_thread_id();
  }

  switch (Z_TYPE_P(zvalue)) {
    // case IS_LONG:
    //     pinpoint_set_context_long(_id,key.c_str(),Z_LVAL_P(zvalue));
    //             break;
  case IS_STRING: {
    std::string value(Z_STRVAL_P(zvalue), Z_STRLEN_P(zvalue));
    pinpoint_set_context_key((NodeID)_id, key.c_str(), value.c_str());
  } break;
  default:
    zend_error(E_WARNING, "value only support string");
    return;
  }
  RETURN_TRUE;
}

// ref from ZEND_FUNCTION(func_get_arg)
PHP_FUNCTION(pinpoint_get_caller_arg) {
  uint32_t arg_count, first_extra_arg;
  zval *arg;
  zend_long requested_offset;
  zend_execute_data *ex;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &requested_offset) ==
      FAILURE) {
    return;
  }

  if (requested_offset < 0) {
    zend_error(
        E_WARNING,
        "pinpoint_get_caller_arg():  The argument number should be >= 0");
    RETURN_FALSE;
  }

  // changes
  // ex = EX(prev_execute_data)
  ex = EX(prev_execute_data)->prev_execute_data;
  if (ZEND_CALL_INFO(ex) & ZEND_CALL_CODE) {
    zend_error(E_WARNING, "pinpoint_get_caller_arg():  Called from the global "
                          "scope - no function context");
    RETURN_FALSE;
  }
#if PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION >= 2
  if (zend_forbid_dynamic_call() == FAILURE) {
    RETURN_THROWS();
  }
#else
  if (zend_forbid_dynamic_call("pinpoint_get_caller_arg()") == FAILURE) {
    RETURN_FALSE;
  }
#endif
  arg_count = ZEND_CALL_NUM_ARGS(ex);

  if ((zend_ulong)requested_offset >= arg_count) {
    zend_error(E_WARNING,
               "pinpoint_get_caller_arg():  Argument "
               "%" PRId64 " not passed to function",
               requested_offset);
    RETURN_FALSE;
  }

  // ZEND_LONG_FMT

  first_extra_arg = ex->func->op_array.num_args;
  if ((zend_ulong)requested_offset >= first_extra_arg &&
      (ZEND_CALL_NUM_ARGS(ex) > first_extra_arg)) {
    arg = ZEND_CALL_VAR_NUM(ex, ex->func->op_array.last_var +
                                    ex->func->op_array.T) +
          (requested_offset - first_extra_arg);
  } else {
    arg = ZEND_CALL_ARG(ex, requested_offset + 1);
  }
  if (EXPECTED(!Z_ISUNDEF_P(arg))) {

#if PHP_MAJOR_VERSION == 7 && PHP_MINOR_VERSION <= 2
    ZVAL_DEREF(arg);
    ZVAL_COPY(return_value, arg);
#else
    ZVAL_COPY_DEREF(return_value, arg);
#endif
  }
}

PHP_FUNCTION(_pinpoint_get_context) {

  std::string key;
#if PHP_VERSION_ID < 70000
  long _id = -1;
  char *zkey = NULL;
  size_t zkey_len;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &zkey, &zkey_len,
                            &_id) == FAILURE) {
    zend_error(E_ERROR, "key/value required");
    return;
  }
  key = std::string(zkey, zkey_len);

#else
  zend_string *zkey;
  zend_long _id = -1;
  if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|l", &zkey, &_id) == FAILURE) {
    zend_error(E_ERROR, "key/value required");
    return;
  }
  key = std::string(zkey->val, zkey->len);
#endif

  if (_id == -1) {
    _id = pinpoint_get_per_thread_id();
  }
  char value[1024] = {0};
  int len = pinpoint_get_context_key((NodeID)_id, key.c_str(), value, 1024);
  if (len > 0) {
#if PHP_VERSION_ID < 70000
    RETURN_STRINGL(value, len, 1);
#else
    RETURN_STRINGL(value, len);
#endif
  } else {
    RETURN_FALSE;
  }
}

PHP_FUNCTION(_pinpoint_start_time) {
  RETURN_DOUBLE(static_cast<double>(pinpoint_start_time()));
}

PHP_FUNCTION(_pinpoint_start_trace) {
  NodeID id = E_ROOT_NODE, cur_id = E_ROOT_NODE;
#if PHP_VERSION_ID < 70000
  long _id = -1;
  zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &_id);
#else
  zend_long _id = -1;
  zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &_id);
#endif
  if (_id == -1) {
    id = pinpoint_get_per_thread_id();
    cur_id = pinpoint_start_trace(id);
    pinpoint_update_per_thread_id(cur_id);
    RETURN_LONG((long)cur_id);
  } else {
    id = (NodeID)_id;
    cur_id = pinpoint_start_trace(id);
    RETURN_LONG((long)cur_id);
  }
}

#if PHP_VERSION_ID >= 80100
void apm_error_cb(int type, zend_string *_error_filename,
                  const uint32_t error_lineno, zend_string *_message) {
  char *error_filename = _error_filename->val;
  char *msg = _message->val;

#elif PHP_VERSION_ID >= 80000
void apm_error_cb(int type, const char *_error_filename,
                  const uint32_t error_lineno, zend_string *_message) {
  char *msg = _message->val;
  const char *error_filename = _error_filename;
#else

void apm_error_cb(int type, const char *error_filename,
                  const uint32_t error_lineno, const char *format,
                  va_list args) {
  char *msg;
  va_list args_copy;
#if PHP_VERSION_ID < 80000
  TSRMLS_FETCH();
#endif
  va_copy(args_copy, args);
  vspprintf(&msg, 0, format, args_copy);
  va_end(args_copy);

#endif

  if (!(EG(error_reporting) & type)) {
#if PHP_VERSION_ID < 80000
    efree(msg);
#endif
    return;
  }

  catch_error(pinpoint_get_per_thread_id(), msg, error_filename, error_lineno);

  pp_trace("apm_error_cb called");

#if PHP_VERSION_ID < 80000

  efree(msg);
  /// call origin cb
  old_error_cb(type, error_filename, error_lineno, format, args);
#else

  old_error_cb(type, _error_filename, error_lineno, _message);
#endif
}

PHP_FUNCTION(_pinpoint_end_trace) {
  NodeID id = E_ROOT_NODE, cur_id = E_ROOT_NODE;

#if PHP_VERSION_ID < 70000
  long _id = -1;
  zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &_id);
#else
  zend_long _id = -1;
  zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &_id);
#endif
  if (_id == -1) {
    id = pinpoint_get_per_thread_id();
    cur_id = pinpoint_end_trace(id);
    pinpoint_update_per_thread_id(cur_id);
    RETURN_LONG((long)cur_id);
  } else {
    id = (NodeID)_id;
    cur_id = pinpoint_end_trace(id);
    RETURN_LONG((long)cur_id);
  }
}

PHP_FUNCTION(pinpoint_status) { show_status(); }

PHP_FUNCTION(_pinpoint_add_clue) {
  std::string key;
  std::string value;

#if PHP_VERSION_ID < 70000
  char *zkey = NULL, *zvalue = NULL;
  int zkey_len, value_len;
  long _id = -1;
  long _flag = E_LOC_CURRENT;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ll", &zkey,
                            &zkey_len, &zvalue, &value_len, &_id,
                            &_flag) == FAILURE) {
    zend_error(E_ERROR, "pinpoint_add_clue() expects (int, string).");
    return;
  }
  key = std::string(zkey, zkey_len);
  value = std::string(zvalue, value_len);

#else
  zend_string *zkey = nullptr;
  zend_string *zvalue = nullptr;
  zend_long _id = -1;
  zend_long _flag = E_LOC_CURRENT;
  if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|ll", &zkey, &zvalue, &_id,
                            &_flag) == FAILURE) {
    zend_error(E_ERROR, "pinpoint_add_clue() expects (int, string).");
    return;
  }
  key = std::string(zkey->val, zkey->len);
  value = std::string(zvalue->val, zvalue->len);
#endif
  NodeID Id = (_id == -1) ? (pinpoint_get_per_thread_id()) : ((NodeID)_id);
  pinpoint_add_clue(Id, key.c_str(), value.c_str(), (E_NODE_LOC)_flag);
}

PHP_FUNCTION(_pinpoint_unique_id) {
  RETURN_DOUBLE(static_cast<double>(generate_unique_id()));
}

PHP_FUNCTION(_pinpoint_mark_as_error) {
  std::string msg;
  std::string fileName;

#if PHP_VERSION_ID < 70000
  char *zkey = NULL, *zvalue = NULL;
  int zkey_len, value_len;
  long _lineno = 0;
  long _id = -1;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ll", &zkey,
                            &zkey_len, &zvalue, &value_len, &_lineno,
                            &_id) == FAILURE) {
    zend_error(E_ERROR,
               "pinpoint_mark_as_error() expects (string,string,int,int).");
    return;
  }
  msg = std::string(zkey, zkey_len);
  fileName = std::string(zvalue, value_len);
#else
  zend_string *zkey;
  zend_string *zvalue;
  zend_long _lineno = 0;
  zend_long _id = -1;
  if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|ll", &zkey, &zvalue, &_lineno,
                            &_id) == FAILURE) {
    zend_error(E_ERROR,
               "pinpoint_mark_as_error() expects (string,string,int,int).");
    return;
  }
  msg = std::string(zkey->val, zkey->len);
  fileName = std::string(zvalue->val, zvalue->len);
#endif
  NodeID id = (_id == -1) ? (pinpoint_get_per_thread_id()) : ((NodeID)_id);
  catch_error(id, msg.c_str(), fileName.c_str(), _lineno);
}

PHP_FUNCTION(_pinpoint_add_clues) {
  std::string key;
  std::string value;
#if PHP_VERSION_ID < 70000
  long _id = -1;
  long _flag = E_LOC_CURRENT;
  char *zkey = NULL, *zvalue = NULL;
  size_t zkey_len, value_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ll", &zkey,
                            &zkey_len, &zvalue, &value_len, &_id,
                            &_flag) == FAILURE) {
    zend_error(E_ERROR, "pinpoint_add_clues() expects (int, string).");
    return;
  }
  key = std::string(zkey, zkey_len);
  value = std::string(zvalue, value_len);
#else
  zend_string *zkey;
  zend_string *zvalue;
  zend_long _id = -1;
  zend_long _flag = E_LOC_CURRENT;
  int ret = zend_parse_parameters(ZEND_NUM_ARGS(), "SS|ll", &zkey, &zvalue,
                                  &_id, &_flag);
  if (ret == FAILURE) {
    zend_error(E_ERROR, "pinpoint_add_clues() expects (int, string).");
    return;
  }
  key = std::string(zkey->val, zkey->len);
  value = std::string(zvalue->val, zvalue->len);
#endif
  NodeID id = (_id == -1) ? (pinpoint_get_per_thread_id()) : ((NodeID)_id);
  pinpoint_add_clues(id, key.c_str(), value.c_str(), (E_NODE_LOC)_flag);
}

static inline zend_string *merge_pp_style_name(zend_string *scope,
                                               zend_string *func) {
#define MAX_CLASS_METHOD_SIZE 128
  char buf[MAX_CLASS_METHOD_SIZE] = {0};
  if (scope) {
    int size = snprintf(buf, MAX_CLASS_METHOD_SIZE, "%s::%s", ZSTR_VAL(scope),
                        ZSTR_VAL(func));
    zend_string *name = zend_string_init(buf, size, 0);
    zend_string *lower_name = zend_string_tolower(name);
    zend_string_release(name);
    return lower_name;
  } else {
    return zend_string_tolower(func);
  }
}

#if (PHP_MAJOR_VERSION == 8 && PHP_MINOR_VERSION >= 2)

// ref from php-8.2.19/ext/standard/var.c:137
static zval *zend_array_index(zval *ar, int index) {
  HashTable *__ht = Z_ARRVAL_P(ar);
  uint32_t _idx = 0;
  uint32_t _count = __ht->nNumUsed - _idx;
  size_t _size = ZEND_HASH_ELEMENT_SIZE(__ht);
  zval *__z = ZEND_HASH_ELEMENT_EX(__ht, _idx, _size);
  zval *ret_zval = nullptr;
  for (; _count > 0 && _idx < index; _count--) {
    ret_zval = __z;
    if (HT_IS_PACKED(__ht)) {
      __z++;
    } else {
      Bucket *_p = (Bucket *)__z;
      __z = &(_p + 1)->val;
    }
    _idx++;
  }
  return ret_zval;
}
#else
// ref from php-8.1.25/ext/standard/var.c:137
static zval *zend_array_index(zval *ar, int index) {
  HashTable *__ht = Z_ARRVAL_P(ar);
  Bucket *_p = __ht->arData;
  Bucket *_end = _p + __ht->nNumUsed;
  zval *val;
  for (int i = 0; i < index && _p != _end; _p++, i++) {
    val = &_p->val;
  }
  return val;
}
#endif

static inline pp_interceptor_v_t *find_interceptor(zend_string *func_name) {
  return (pp_interceptor_v_t *)zend_hash_str_find_ptr(
      PPG(interceptors), ZSTR_VAL(func_name), ZSTR_LEN(func_name));
}

static zend_string *
get_pp_style_function_name(zend_execute_data *execute_data) {
  zend_function *func = execute_data->func;
  zend_object *object = (Z_TYPE(execute_data->This) == IS_OBJECT)
                            ? Z_OBJ(execute_data->This)
                            : NULL;

#if PHP_MAJOR_VERSION == 8
  zend_string *function_name = NULL;
  if (func) {
    function_name = func->common.function_name;
  }
#else
  zend_string *function_name =
      (func->common.scope && func->common.scope->trait_aliases)
          ? zend_resolve_method_name((object ? object->ce : func->common.scope),
                                     func)
          : func->common.function_name;
#endif
  if (object) {
    zend_string *scope_name;
    if (func->common.scope) {
      scope_name = func->common.scope->name;
#if PHP_MAJOR_VERSION == 7 && PHP_MINOR_VERSION <= 2
    } else if (object->handlers->get_class_name ==
               std_object_handlers.get_class_name) {
#else
    } else if (object->handlers->get_class_name == zend_std_get_class_name) {
#endif
      scope_name = object->ce->name;
    } else {
      scope_name = object->handlers->get_class_name(object);
    }
    return merge_pp_style_name(scope_name, function_name);
  } else if (func->common.scope) {
    zend_string *scope = func->common.scope->name;
    return merge_pp_style_name(scope, function_name);
  } else {
    return merge_pp_style_name(nullptr, function_name);
  }
}

static void call_callback_function(zval *callback, zval *params,
                                   uint32_t params_count, int free_params,
                                   zval *needs_retval = nullptr) {
  // ref SAPI.c:139
  int error;
  zend_fcall_info fcall_info;
  char *callback_error = NULL;
  zval default_retval;
  zval *retval = &default_retval;
  if (needs_retval) {
    retval = needs_retval;
  }
  zend_fcall_info_cache fcall_cache;
  if (zend_fcall_info_init(callback, 0, &fcall_info, &fcall_cache, NULL,
                           &callback_error) == SUCCESS) {
    fcall_info.retval = retval;
    fcall_info.param_count = params_count;
    fcall_info.params = params;
    fcall_info.object = NULL;

#if PHP_MAJOR_VERSION == 7
    fcall_info.no_separation = 0;
#endif

    // fcall_cache.initialized = 1;
    // fcall_cache.function_handler = EG(autoload_func);

    fcall_cache.called_scope = NULL;
    fcall_cache.object = NULL;

    error = zend_call_function(&fcall_info, &fcall_cache);
    if (error == FAILURE) {
      goto callback_failed;
    } else {
      if (!needs_retval) {
        zval_ptr_dtor(retval);
      }
    }
  } else {
  callback_failed:
    php_error_docref(NULL, E_WARNING,
                     "Could not call the pinpoint_join_cut callback");
  }

  if (callback_error) {
    efree(callback_error);
  }

  if (free_params) {
    zend_fcall_info_args_clear(&fcall_info, 1);
  }
}

static void get_shadow_copy_current_parameters(int param_count,
                                               zval *argument_array) {
  // ref zend_API.c:46
  // _zend_get_parameters_array_ex
  zval *param_ptr;

  param_ptr = ZEND_CALL_ARG(EG(current_execute_data), 1);

  while (param_count-- > 0) {
    ZVAL_COPY(argument_array, param_ptr);
    argument_array++;
    param_ptr++;
  }
}

static void replace_ex_caller_parameters(zval *argv) {
  if (zval_get_type(argv) != IS_ARRAY) {
    pp_trace("replace_ex_caller_parameters return value must be `an array`");
    return;
  }

  int size = zend_array_count(Z_ARRVAL_P(argv));
  pp_trace("argv size:%d", size);
  uint32_t param_count = ZEND_CALL_NUM_ARGS(EG(current_execute_data));
  if (size != param_count) {
    pp_trace(
        "error: replace_ex_caller_parameters return `size` does not matched");
    return;
  }

  int i = 0;
  zval *ex_param_ptr = ZEND_CALL_ARG(EG(current_execute_data), 1);

  // check old and new
  while (i < size) {
    zval *val = zend_array_index(argv, i + 1);
    if (Z_TYPE_P(ex_param_ptr) != Z_TYPE_P(val)) {
      pp_trace("error: replace_ex_caller_parameters return `type` does not "
               "matched "
               "expected:%d give:%d",
               Z_TYPE_P(ex_param_ptr), Z_TYPE_P(val));
      return;
    }
    i++;
    ex_param_ptr++;
  }

  i = 0;
  ex_param_ptr = ZEND_CALL_ARG(EG(current_execute_data), 1);
  while (i < size) {
    zval *val = zend_array_index(argv, i + 1);

    if (Z_TYPE_P(val) == IS_ARRAY) {
      pp_trace("args[%d] type:%d in:%u va:%u", i, zval_get_type(val),
               Z_REFCOUNT_P(val), Z_REFCOUNT_P(ex_param_ptr));
    }

    zval_ptr_dtor(ex_param_ptr);
    ZVAL_COPY(ex_param_ptr, val);

    i++;
    ex_param_ptr++;
  }
}

static void call_interceptor_before(pp_interceptor_v_t *interceptor) {

  uint32_t param_count = ZEND_CALL_NUM_ARGS(EG(current_execute_data));
  zval *params = (zval *)safe_emalloc(param_count, sizeof(zval), 0);

  get_shadow_copy_current_parameters(param_count, params);
  zval retval;
  call_callback_function(&interceptor->before, params, param_count, 1, &retval);
  pp_trace(" call_interceptor_before:%s return type(%d) zval",
           ZSTR_VAL(interceptor->name), zval_get_type(&retval));

  replace_ex_caller_parameters(&retval);

  zval_ptr_dtor(&retval);
}

static void call_interceptor_end(pp_interceptor_v_t *interceptor,
                                 zval *return_value) {
  uint32_t param_count = 1;
  zval *params = (zval *)safe_emalloc(param_count, sizeof(zval), 0);
  ZVAL_COPY(params, return_value);
  call_callback_function(&interceptor->end, params, param_count, 1);
  pp_trace("call_interceptor_end: %s ", ZSTR_VAL(interceptor->name));
}
static void call_interceptor_exception(pp_interceptor_v_t *interceptor) {
  zend_object *exception = EG(exception);
  pp_trace(" exception: %p ", exception);
}

ZEND_NAMED_FUNCTION(pinpoint_interceptor_handler_entry) {
  // 1. get function/method name
  // ref zend_builtin_functions.c:2278
  zend_string *function_name = get_pp_style_function_name(execute_data);
  pp_trace("pinpoint_interceptor_handler_entry: handle func/method:%s",
           ZSTR_VAL(function_name));
  pp_interceptor_v_t *interceptor = find_interceptor(function_name);
  zend_string_release(function_name);
  if (interceptor == nullptr) {
    pp_trace(" MUST be a bug 🐞🐞🐞 !!! please send us "
             "email@dl_cd_pinpoint@navercorp.com");
    return;
  }

  // 2. call before
  // 3. call origin
  // 4. call end
  // 5. call exception if catch

  zend_try {
    call_interceptor_before(interceptor);
    interceptor->origin(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    call_interceptor_end(interceptor, return_value);
  }
  zend_catch {
    call_interceptor_exception(interceptor);
    call_interceptor_end(interceptor, return_value);
  }
  zend_end_try();
}

static pp_interceptor_v_t *make_interceptor(zend_string *name, zval *before,
                                            zval *end, zval *exception,
                                            zend_function *origin_func) {
  pp_interceptor_v_t *interceptor =
      (pp_interceptor_v_t *)malloc(sizeof(pp_interceptor_v_t));
  // bzero(interceptor, sizeof(*interceptor));
  memset(interceptor, 0, sizeof(*interceptor));
  interceptor->name = zend_string_dup(name, 0);
  interceptor->origin = origin_func->internal_function.handler;
  interceptor->origin_func = origin_func;
  ZVAL_DUP(&interceptor->before, before);
  ZVAL_DUP(&interceptor->end, end);
  ZVAL_DUP(&interceptor->exception, exception);

  return interceptor;
}

static inline void free_callback(zval *val) {
  if (Z_TYPE_P(val) != IS_UNDEF) {
    zval_ptr_dtor(val);
  }
}

static void free_interceptor(pp_interceptor_v_t *interceptor) {
  pp_trace("start free interceptor: %s", ZSTR_VAL(interceptor->name));
  zend_function *origin_func = interceptor->origin_func;
  origin_func->internal_function.handler = interceptor->origin;
  zend_string_release(interceptor->name);
  free_callback(&interceptor->before);
  free_callback(&interceptor->end);
  free_callback(&interceptor->exception);
  free(interceptor);
}

static void add_function_interceptor(zend_string *name, zval *before, zval *end,
                                     zval *exception) {
  zend_function *func = (zend_function *)zend_hash_str_find_ptr(
      CG(function_table), ZSTR_VAL(name), ZSTR_LEN(name));
  if (func != NULL &&
      func->internal_function.handler == pinpoint_interceptor_handler_entry) {
    pp_trace("function `%s` interceptor already added", ZSTR_VAL(name));
  } else if (func != NULL) {
    pp_interceptor_v_t *interceptor =
        make_interceptor(name, before, end, exception, func);
    // insert into hash
    if (!zend_hash_add_ptr(PPG(interceptors), name, interceptor)) {
      free_interceptor(interceptor);
      pp_trace("added interceptor on `function`: %s failed. reason: already "
               "exist ",
               ZSTR_VAL(name));
      return;
    }
    func->internal_function.handler = pinpoint_interceptor_handler_entry;
    pp_trace("added interceptor on `function`: %s success", ZSTR_VAL(name));
  } else {
    pp_trace("not found function: %s", ZSTR_VAL(name));
  }
}

static void add_class_method_interceptor(zend_string *cls_name,
                                         zend_string *method, zval *before,
                                         zval *end, zval *exception) {

  zend_class_entry *module = (zend_class_entry *)zend_hash_str_find_ptr(
      CG(class_table), ZSTR_VAL(cls_name), ZSTR_LEN(cls_name));
  if (module != NULL) {
    zend_function *original = (zend_function *)zend_hash_str_find_ptr(
        &module->function_table, ZSTR_VAL(method), ZSTR_LEN(method));

    if (original != NULL) {
      // overwrite with plugins eg: pinpoint_pdo_exec
#define MAX_CLASS_METHOD_SIZE 128
      char buf[MAX_CLASS_METHOD_SIZE] = {0};
      int size = snprintf(buf, MAX_CLASS_METHOD_SIZE, "%s::%s",
                          ZSTR_VAL(cls_name), ZSTR_VAL(method));

      zend_string *name = zend_string_init(buf, size, 0);

      pp_interceptor_v_t *interceptor =
          make_interceptor(name, before, end, exception, original);

      if (!zend_hash_add_ptr(PPG(interceptors), name, interceptor)) {
        free_interceptor(interceptor);
        pp_trace("added interceptor on `module`: %s failed. reason: already "
                 "exist ",
                 ZSTR_VAL(name));
        zend_string_release(name);
        return;
      }
      original->internal_function.handler = pinpoint_interceptor_handler_entry;
      pp_trace("added interceptor on `module`: %s success", ZSTR_VAL(name));
      zend_string_release(name);
    } else {
      pp_trace("add interceptor on `module`: %s::%s failed: no such method",
               ZSTR_VAL(cls_name), ZSTR_VAL(method));
    }
  } else {
    pp_trace("add interceptor on `module`: %s failed: no such module",
             ZSTR_VAL(cls_name));
    // debug CG(class_table)
    // zend_class_entry *ce;
    // void *val;
    // ZEND_HASH_FOREACH_PTR(CG(class_table), val) {
    //   zend_class_entry *ce = (zend_class_entry *)val;
    //   pp_trace("key:%s name: %s", ZSTR_VAL(_p->key), ZSTR_VAL(ce->name));
    //   // if (ce->type == ZEND_INTERNAL_CLASS &&
    //   //     ce->default_static_members_count > 0) {
    //   //   class_cleanup_handlers[--class_count] = ce;
    //   // }
    // }
    // ZEND_HASH_FOREACH_END();
  }
}

static void add_interceptor(zval *joinable, zval *before, zval *end,
                            zval *exception) {
  uint32_t join_type = zend_hash_num_elements(Z_ARRVAL_P(joinable));

  switch (join_type) {
  case 2: {
    zval *v = zend_array_index(joinable, 1);
    zend_string *module = zend_string_tolower(Z_STR_P(v));
    v = zend_array_index(joinable, 2);
    zend_string *method = zend_string_tolower(Z_STR_P(v));
    pp_trace("try to interceptor module(class)/function=%s:%s",
             ZSTR_VAL(module), ZSTR_VAL(method));
    add_class_method_interceptor(module, method, before, end, exception);
    zend_string_release(module);
    zend_string_release(method);
    break;
  }
  case 1: {
    zval *v = zend_array_index(joinable, 1);
    zend_string *function = zend_string_tolower(Z_STR_P(v));
    pp_trace("try to interceptor function=%s", ZSTR_VAL(function));
    add_function_interceptor(function, before, end, exception);
    zend_string_release(function);
    break;
  }
  default:
    pp_trace("not supported join_type:%d", join_type);
    return;
  }
}

PHP_FUNCTION(_pinpoint_join_cut) {
  zval *joinable, *before, *end, *exception;

  ZEND_PARSE_PARAMETERS_START(4, 4)
  Z_PARAM_ARRAY(joinable)
  Z_PARAM_ZVAL(before)
  Z_PARAM_ZVAL(end)
  Z_PARAM_ZVAL(exception)
  ZEND_PARSE_PARAMETERS_END();

  // check input
  if (Z_TYPE_P(joinable) != IS_ARRAY ||
      zend_hash_num_elements(Z_ARRVAL_P(joinable)) == 0) {
    goto PARAMETERS_ERROR;
  }

  if (!zend_is_callable(before, 0, NULL) || !zend_is_callable(end, 0, NULL) ||
      !zend_is_callable(exception, 0, NULL)) {
    goto PARAMETERS_ERROR;
  }
  add_interceptor(joinable, before, end, exception);

  RETURN_TRUE;

PARAMETERS_ERROR:
  php_error_docref(
      NULL, E_WARNING,
      "Parameters does not meet: joinable: %s size:%d, onBefore:%s,"
      "onEnd:%s, onException:%s",
      zend_zval_type_name(joinable),
      zend_hash_num_elements(Z_ARRVAL_P(joinable)), zend_zval_type_name(before),
      zend_zval_type_name(end), zend_zval_type_name(exception));
  RETURN_FALSE;
}
#if 0
/**
 * copy from php source zend_buildin_functions.c
 *                          ZEND_FUNCTION(func_get_args)
 * disable ZVAL_DEREF(p) make it works
 */
PHP_FUNCTION(pinpoint_get_func_ref_args) {
#if PHP_VERSION_ID < 70000
  void **p;
  int arg_count;
  int i;
  zend_execute_data *ex = EG(current_execute_data)->prev_execute_data;

  if (!ex || !ex->function_state.arguments) {
    zend_error(E_WARNING, "pinpoint_get_func_ref_args():  Called from the "
                          "global scope - no function context");
    RETURN_FALSE;
  }

  p = ex->function_state.arguments;
  arg_count = (int)(zend_uintptr_t)*p; /* this is the amount of arguments passed
                                        to func_get_args(); */

  array_init_size(return_value, arg_count);
  for (i = 0; i < arg_count; i++) {
    zval *element, *arg;

    arg = *((zval **)(p - (arg_count - i)));
    element = arg;
    Z_ADDREF_P(element);

    zend_hash_next_index_insert(return_value->value.ht, &element,
                                sizeof(zval *), NULL);
  }
#else

  zval *p, *q;
  uint32_t arg_count, first_extra_arg;
  uint32_t i, n;
  zend_execute_data *ex = EX(prev_execute_data);

  if (ZEND_CALL_INFO(ex) & ZEND_CALL_CODE) {
    zend_error(E_WARNING, "pinpoint_get_func_ref_args():  Called from the "
                          "global scope - no function context");
    RETURN_FALSE;
  }
#if PHP_VERSION_ID > 70033
  if (zend_forbid_dynamic_call("pinpoint_get_func_ref_args()") == FAILURE) {
    RETURN_FALSE;
  }
#endif
  arg_count = ZEND_CALL_NUM_ARGS(ex);

  array_init_size(return_value, arg_count);
  if (arg_count) {
    first_extra_arg = ex->func->op_array.num_args;
    zend_hash_real_init(Z_ARRVAL_P(return_value), 1);
    ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
      i = 0;
      n = 0;
      p = ZEND_CALL_ARG(ex, 1);
      if (arg_count > first_extra_arg) {
        while (i < first_extra_arg) {
          q = p;
          if (EXPECTED(Z_TYPE_INFO_P(q) != IS_UNDEF)) {
            //                        ZVAL_DEREF(q);
            if (Z_OPT_REFCOUNTED_P(q)) {
              Z_ADDREF_P(q);
            }
            n++;
          }
          ZEND_HASH_FILL_ADD(q);
          p++;
          i++;
        }
        p = ZEND_CALL_VAR_NUM(ex, ex->func->op_array.last_var +
                                      ex->func->op_array.T);
      }
      while (i < arg_count) {
        q = p;
        if (EXPECTED(Z_TYPE_INFO_P(q) != IS_UNDEF)) {
          //                    ZVAL_DEREF(q);
          if (Z_OPT_REFCOUNTED_P(q)) {
            Z_ADDREF_P(q);
          }
          n++;
        }
        ZEND_HASH_FILL_ADD(q);
        p++;
        i++;
      }
    }
    ZEND_HASH_FILL_END();
    Z_ARRVAL_P(return_value)->nNumOfElements = n;
  }
#endif
}
#endif

PHP_FUNCTION(_pinpoint_trace_limit) {
  // getchar();
  double timestamp = -1;
#if PHP_VERSION_ID < 70000

  zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|d", &timestamp);
#else
  zend_parse_parameters(ZEND_NUM_ARGS(), "|d", &timestamp);
#endif

  int64_t timestamp_64 =
      (timestamp == -1) ? (time(NULL)) : (static_cast<std::int64_t>(timestamp));

  if (check_trace_limit(timestamp_64) == 1) {
    RETURN_TRUE;
  } else {
    RETURN_FALSE;
  }
}

/* {{{ php_pinpioint_php_init_globals
 */
/* Uncomment this function if you have INI entries
 */
static void
php_pinpoint_php_init_globals(zend_pinpoint_php_globals *pinpoint_php_globals) {
  memset(pinpoint_php_globals, 0, sizeof(zend_pinpoint_php_globals));
}

/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */

PHP_MINIT_FUNCTION(pinpoint_php) {
  ZEND_INIT_MODULE_GLOBALS(pinpoint_php, php_pinpoint_php_init_globals, NULL);
  REGISTER_INI_ENTRIES();

  old_error_cb = zend_error_cb;
  zend_error_cb = apm_error_cb;
  pinpoint_set_agent(PPG(co_host), PPG(w_timeout_ms), PPG(tracelimit), 1500);

  if (PPG(debug_report) == 1) {
    register_logging_cb(nullptr, 1);
    // register_logging_cb(pinpoint_log, 1);
  } else {
    // register_logging_cb(pinpoint_log, 0);
    register_logging_cb(nullptr, 0);
  }

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(pinpoint_php) {
  /* uncomment this line if you have INI entries
  UNREGISTER_INI_ENTRIES();
  */

  return SUCCESS;
}
/* }}} */

static void zend_interceptor_hash_dtor(zval *val) {
  // pp_trace("test: zval:%p type:%d ", zv, Z_TYPE_P(zv));
  if (Z_TYPE_P(val) == IS_PTR) {
    pp_interceptor_v_t *interceptor = (pp_interceptor_v_t *)Z_PTR_P(val);
    free_interceptor(interceptor);
  }
}

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(pinpoint_php) {

#if defined(COMPILE_DL_PINPOINT_PHP) && defined(ZTS)
  ZEND_TSRMLS_CACHE_UPDATE();
#endif
  PPG(interceptors) = (HashTable *)malloc(sizeof(HashTable));
  zend_hash_init(PPG(interceptors), 0, NULL, zend_interceptor_hash_dtor, 1);
  return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(pinpoint_php) {
  NodeID _parent_id = pinpoint_get_per_thread_id();
  while (_parent_id > E_ROOT_NODE) {
    _parent_id = pinpoint_end_trace(_parent_id);
  }
  pinpoint_update_per_thread_id(E_ROOT_NODE);

  if (PPG(interceptors)) {
    zend_hash_destroy(PPG(interceptors));
    free(PPG(interceptors));
    PPG(interceptors) = nullptr;
  }

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pinpoint_php) {
  php_info_print_table_start();
  php_info_print_table_header(2, "pinpoint_php support", "enabled");
  php_info_print_table_end();

  //    /* Remove comments if you have entries in php.ini
  DISPLAY_INI_ENTRIES();
  //    */
}
/* }}} */

void pinpoint_log(char *msg) {
#if PHP_VERSION_ID >= 70100
  php_log_err_with_severity(msg, LOG_DEBUG);
#else
  TSRMLS_FETCH();
  php_log_err(msg TSRMLS_CC);
#endif
}
