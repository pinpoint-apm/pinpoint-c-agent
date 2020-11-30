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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
//#include "php_var.h"
#include "ext/standard/info.h"

#include "php_pinpoint_php.h"
#include "common.h"
#include <iostream>

#ifdef COMPILE_DL_PINPOINT_PHP
#ifdef ZTS
    #if PHP_VERSION_ID > 70000
        ZEND_TSRMLS_CACHE_DEFINE()
    #else
        #include "TSRM.h"
    #endif
#endif
ZEND_GET_MODULE(pinpoint_php)
#endif

PHP_FUNCTION(pinpoint_start_trace);
PHP_FUNCTION(pinpoint_end_trace);
PHP_FUNCTION(pinpoint_add_clue);
PHP_FUNCTION(pinpoint_add_clues);
PHP_FUNCTION(pinpoint_unique_id);
PHP_FUNCTION(pinpoint_tracelimit);
PHP_FUNCTION(pinpoint_drop_trace);
PHP_FUNCTION(pinpoint_start_time);
PHP_FUNCTION(pinpoint_set_context);
PHP_FUNCTION(pinpoint_get_context);
PHP_FUNCTION(pinpoint_get_func_ref_args);

ZEND_DECLARE_MODULE_GLOBALS(pinpoint_php)

PPAgentT global_agent_info;
static void pinpoint_log(char *msg);

/* {{{ PHP_INI
 */

PHP_INI_BEGIN()

STD_PHP_INI_ENTRY("pinpoint_php.CollectorHost", "unix:/tmp/collector.sock", PHP_INI_ALL,
        OnUpdateString,co_host,zend_pinpoint_php_globals,pinpoint_php_globals)

STD_PHP_INI_ENTRY("pinpoint_php.UnitTest", "no", PHP_INI_ALL,
        OnUpdateBool,utest_flag,zend_pinpoint_php_globals,pinpoint_php_globals)

STD_PHP_INI_ENTRY("pinpoint_php.TraceLimit","-1",PHP_INI_ALL,
        OnUpdateLong,tracelimit,zend_pinpoint_php_globals,pinpoint_php_globals)

STD_PHP_INI_ENTRY("pinpoint_php.DebugReport","no",PHP_INI_ALL,
        OnUpdateBool,debug_report,zend_pinpoint_php_globals,pinpoint_php_globals)



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

ZEND_BEGIN_ARG_INFO_EX(arginfo_add_int, 0, 0, 1)
    ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_add_id, 0, 0, 1)
    ZEND_ARG_INFO(0, nodeid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_add_id_value, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, nodeid)
ZEND_END_ARG_INFO()

/* {{{ pinpioint_php_functions[]
 *
 * Every user visible function must have an entry in pinpioint_php_functions[].
 */
const zend_function_entry pinpoint_php_functions[] = {
        PHP_FE(pinpoint_start_trace,arginfo_add_id)
        PHP_FE(pinpoint_end_trace,arginfo_add_id)
        PHP_FE(pinpoint_unique_id,NULL)
        PHP_FE(pinpoint_get_func_ref_args,NULL)
        PHP_FE(pinpoint_drop_trace,arginfo_add_id)
        PHP_FE(pinpoint_start_time,NULL)
        PHP_FE(pinpoint_set_context,arginfo_add_id_key_value)
        PHP_FE(pinpoint_get_context,arginfo_add_id_value)
        PHP_FE(pinpoint_tracelimit,arginfo_add_int)
        PHP_FE(pinpoint_add_clue,arginfo_add_id_key_value_flag)
        PHP_FE(pinpoint_add_clues,arginfo_add_id_key_value_flag)
        PHP_FE_END  /* Must be the last line in pinpioint_php_functions[] */
};
/* }}} */

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
    PHP_RINIT(pinpoint_php),       /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(pinpoint_php),   /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(pinpoint_php),
#if ZEND_MODULE_API_NO >= 20010901
    PHP_PINPOINT_PHP_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */



void (*old_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);

#define safe_free(x)\
    if((x)){ \
        free((x));\
        (x) = NULL;\
    }

PHP_FUNCTION(pinpoint_drop_trace)
{
    long _id = -1;
    NodeID id = 0,cur_id = 0;
    zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &_id);

    if(_id == -1){
        id = pinpoint_get_per_thread_id();
    }else{
        id = _id;
    }
    mark_current_trace_status(id,E_TRACE_BLOCK);
    RETURN_TRUE;
}


PHP_FUNCTION(pinpoint_set_context)
{
    long _id = -1;
    std::string key;
    zval* zvalue;
#if PHP_VERSION_ID < 70000
    char* zkey = NULL;
    int zkey_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|l", &zkey, &zkey_len,&zvalue,&_id) == FAILURE)
    {
      zend_error(E_ERROR,"key/value required");
      return ;
    }
    key = std::string(zkey,zkey_len);

#else
    zend_string* zkey;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Sz|l", &zkey ,&zvalue,&_id) == FAILURE)
    {
        zend_error(E_ERROR,"key/value required");
        return ;
    }
    key = std::string(zkey->val,zkey->len);
#endif

    if(_id == -1){
        _id = pinpoint_get_per_thread_id();;
    }

    switch(Z_TYPE_P(zvalue)){
        // case IS_LONG:
        //     pinpoint_set_context_long(_id,key.c_str(),Z_LVAL_P(zvalue));
//             break;
        case IS_STRING:
            {
                std::string value(Z_STRVAL_P(zvalue), Z_STRLEN_P(zvalue));
                pinpoint_set_context_key(_id,key.c_str(),value.c_str());
            }
            break;
        default:
            zend_error(E_WARNING,"value only support string");
            return ;
    }
    RETURN_TRUE;
}



PHP_FUNCTION(pinpoint_get_context)
{
    long _id = -1;
    std::string key;
#if PHP_VERSION_ID < 70000
    char* zkey = NULL;
    int zkey_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &zkey, &zkey_len,&_id) == FAILURE)
    {
      zend_error(E_ERROR,"key/value required");
      return ;
    }
    key = std::string(zkey,zkey_len);

#else
    zend_string* zkey;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S|l", &zkey,&_id) == FAILURE)
    {
        zend_error(E_ERROR,"key/value required");
        return ;
    }
    key = std::string(zkey->val,zkey->len);
#endif

    if(_id == -1){
        _id = pinpoint_get_per_thread_id();
    }

    const char* value = pinpoint_get_context_key(_id,key.c_str());
    if(value){
#if PHP_VERSION_ID < 70000
        RETURN_STRING(value,1);
#else
        RETURN_STRING(value);
#endif

    }else{
        RETURN_FALSE;
    }

}


PHP_FUNCTION(pinpoint_start_time)
{
    RETURN_LONG(pinpoint_start_time());
}

PHP_FUNCTION(pinpoint_start_trace)
{

    long _id = -1;
    NodeID id = 0,cur_id = 0;
    zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &_id);

    if(_id == -1){
        id = pinpoint_get_per_thread_id();
        cur_id = pinpoint_start_trace(id);
        pinpoint_update_per_thread_id(cur_id);
        RETURN_LONG((long)cur_id);
    }else{
        id = _id;
        cur_id = pinpoint_start_trace(id);
        RETURN_LONG((long)cur_id);
    }

}

void apm_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args)
{
    char *msg;
    va_list args_copy;
    TSRMLS_FETCH();
    va_copy(args_copy, args);
    vspprintf(&msg, 0, format, args_copy);
    va_end(args_copy);

    if(!(EG(error_reporting) & type) )
    {
       return;
    }

    catch_error(pinpoint_get_per_thread_id(),msg,error_filename,error_lineno);
    pp_trace("apm_error_cb called");
    efree(msg);
    /// call origin cb
    old_error_cb(type, error_filename, error_lineno, format, args);
}

PHP_FUNCTION(pinpoint_end_trace)
{

    long _id = -1;
    NodeID id = 0,cur_id = 0;
    zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &_id);

    if(_id == -1){
        id = pinpoint_get_per_thread_id();
        cur_id = pinpoint_end_trace(id);
        pinpoint_update_per_thread_id(cur_id);
        RETURN_LONG((long)cur_id);
    }else{
        id = _id;
        cur_id = pinpoint_end_trace(id);
        RETURN_LONG((long)cur_id);
    }
}

PHP_FUNCTION(pinpoint_add_clue)
{
       std::string key;
       std::string value;
       long _id = -1;
       long _flag = E_CURRENT_LOC;
   #if PHP_VERSION_ID < 70000
       char* zkey = NULL,* zvalue =  NULL;
       int zkey_len,value_len;

       if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ll", &zkey, &zkey_len,&zvalue, &value_len,&_id,&_flag) == FAILURE)
       {
           zend_error(E_ERROR, "pinpoint_add_clue() expects (int, string).");
           return;
       }
       key = std::string(zkey,zkey_len);
       value = std::string(zvalue,value_len);

   #else
       zend_string* zkey;
       zend_string* zvalue;
       if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "SS|ll", &zkey ,&zvalue,&_id,&_flag) == FAILURE)
       {
          zend_error(E_ERROR, "pinpoint_add_clue() expects (int, string).");
          return;
       }
       key = std::string(zkey->val,zkey->len);
       value = std::string(zvalue->val,zvalue->len);
   #endif
       NodeID Id = (_id== -1) ?(pinpoint_get_per_thread_id()):(_id);
       pinpoint_add_clue(Id,key.c_str(),value.c_str(),(E_NODE_LOC)_flag);

}

PHP_FUNCTION(pinpoint_unique_id)
{
    RETURN_LONG(generate_unique_id());
}


PHP_FUNCTION(pinpoint_add_clues)
{
       std::string key;
       std::string value;
       long _id = -1;
       long _flag = E_CURRENT_LOC;
   #if PHP_VERSION_ID < 70000
       char* zkey = NULL,* zvalue =  NULL;
       int zkey_len,value_len;

       if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ll", &zkey, &zkey_len,&zvalue, &value_len,&_id,&_flag) == FAILURE)
       {
           zend_error(E_ERROR, "pinpoint_add_clues() expects (int, string).");
           return;
       }
       key = std::string(zkey,zkey_len);
       value = std::string(zvalue,value_len);
   #else
       zend_string* zkey;
       zend_string* zvalue;
       if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "SS|ll", &zkey ,&zvalue,&_id,&_flag) == FAILURE)
       {
          zend_error(E_ERROR, "pinpoint_add_clues() expects (int, string).");
          return;
       }
       key = std::string(zkey->val,zkey->len);
       value = std::string(zvalue->val,zvalue->len);
   #endif
       NodeID id =( _id== -1) ?(pinpoint_get_per_thread_id()):(_id);
       pinpoint_add_clues(id,key.c_str(),value.c_str(),(E_NODE_LOC)_flag);

}

/**
 * copy from php source zend_buildin_functions.c
 *                          ZEND_FUNCTION(func_get_args)
 * disable ZVAL_DEREF(p) make it works
*/
PHP_FUNCTION(pinpoint_get_func_ref_args)
{
#if PHP_VERSION_ID < 70000
    void **p;
    int arg_count;
    int i;
    zend_execute_data *ex = EG(current_execute_data)->prev_execute_data;

    if (!ex || !ex->function_state.arguments) {
        zend_error(E_WARNING, "pinpoint_get_func_ref_args():  Called from the global scope - no function context");
        RETURN_FALSE;
    }

    p = ex->function_state.arguments;
    arg_count = (int)(zend_uintptr_t) *p;       /* this is the amount of arguments passed to func_get_args(); */

    array_init_size(return_value, arg_count);
    for (i=0; i<arg_count; i++) {
        zval *element, *arg;

        arg = *((zval **) (p-(arg_count-i)));
        element = arg;
        Z_ADDREF_P(element);

        zend_hash_next_index_insert(return_value->value.ht, &element, sizeof(zval *), NULL);
    }
#else

      zval *p, *q;
    uint32_t arg_count, first_extra_arg;
    uint32_t i, n;
    zend_execute_data *ex = EX(prev_execute_data);

    if (ZEND_CALL_INFO(ex) & ZEND_CALL_CODE) {
        zend_error(E_WARNING, "pinpoint_get_func_ref_args():  Called from the global scope - no function context");
        RETURN_FALSE;
    }
#if     PHP_VERSION_ID > 70033
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
                p = ZEND_CALL_VAR_NUM(ex, ex->func->op_array.last_var + ex->func->op_array.T);
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
        } ZEND_HASH_FILL_END();
        Z_ARRVAL_P(return_value)->nNumOfElements = n;
    }
#endif
}


PHP_FUNCTION(pinpoint_tracelimit)
{

    long timestamp = -1;
    zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &timestamp);
    timestamp = (timestamp == -1)?(time(NULL)):(timestamp);

    if(check_tracelimit(timestamp) == 1)
    {
        RETURN_TRUE
    }else{
        RETURN_FALSE
    }

}

/* {{{ php_pinpioint_php_init_globals
 */
/* Uncomment this function if you have INI entries
 */
static void php_pinpoint_php_init_globals(zend_pinpoint_php_globals *pinpoint_php_globals)
{
    memset(pinpoint_php_globals,0,sizeof(zend_pinpoint_php_globals));

}

/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */

PHP_MINIT_FUNCTION(pinpoint_php)
{
    ZEND_INIT_MODULE_GLOBALS(pinpoint_php,php_pinpoint_php_init_globals, NULL);
    REGISTER_INI_ENTRIES();

    old_error_cb = zend_error_cb;
    zend_error_cb = apm_error_cb;
    // global_agent_info.
    global_agent_info.co_host =  PPG(co_host);
    global_agent_info.inter_flag = PPG(debug_report);
    global_agent_info.trace_limit =PPG(tracelimit);
    global_agent_info.agent_type = 1500; // PHP

    if (PPG(utest_flag) == 1){
        global_agent_info.inter_flag |= E_UTEST;
    }else{
        register_error_cb(pinpoint_log);
    }

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(pinpoint_php)
{
    /* uncomment this line if you have INI entries
    UNREGISTER_INI_ENTRIES();
    */

    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(pinpoint_php)
{

#if defined(COMPILE_DL_PINPIOINT_PHP) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();2
#endif

    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(pinpoint_php)
{
    NodeID _id = pinpoint_get_per_thread_id();
    if(_id != 0){
        pinpoint_force_end_trace(_id,300);
        pinpoint_update_per_thread_id(0);
    }

    return SUCCESS;
}
/* }}} */


/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pinpoint_php)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "pinpoint_php support", "enabled");
    php_info_print_table_end();

//    /* Remove comments if you have entries in php.ini
    DISPLAY_INI_ENTRIES();
//    */
}
/* }}} */


void pinpoint_log(char *msg)
{

#if PHP_VERSION_ID >= 70100
    php_log_err_with_severity( msg, LOG_DEBUG);
#else
    TSRMLS_FETCH();
    php_log_err(msg TSRMLS_CC);
#endif
}
