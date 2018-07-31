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

#include "php_function_helper.h"

int call_php_kernel_unserialize(zval* retval_ptr, zval *objzval)
{
    TSRMLS_FETCH();

    zval function_name;
    ZVAL_STRING_SET(&function_name, "unserialize");
    int ret = 0;
    zval** tmpVal = &objzval;

    w_call_user_function(ret, FunTable() , NULL , &function_name, retval_ptr, 1, tmpVal TSRMLS_CC);
    if (ret != SUCCESS)
    {

        LOGE("call func_get_args error");
        ret = -1;
        w_zval_ptr_dtor(retval_ptr);
        goto EXIT;
    }

EXIT:
    zval_dtor(&function_name);
    return ret;
}

int call_php_kernel_serialize(zval *retval, zval* objzval)
{
    TSRMLS_FETCH();

    zval function_name;
    ZVAL_STRING_SET(&function_name, "serialize");
    zval** tmpVal = &objzval;

    int ret = 0;
    w_call_user_function(ret,FunTable() , NULL , &function_name, retval, 1, tmpVal TSRMLS_CC);

    if (ret != SUCCESS)
    {

        LOGE("call func_get_args error");
        ret = -1;
        goto EXIT;
    }

EXIT:
    zval_dtor(&function_name);

    return ret;
}


int call_php_kernel_print_r(zval *retval, zval* objzval)
{
    TSRMLS_FETCH();

    zval function_name;
    ZVAL_STRING_SET(&function_name, "print_r");

    zval* tmpVal[2] = {0};

    ALLOC_INIT_ZVAL(tmpVal[1]);

    tmpVal[0] = objzval;

    zend_bool val = 1;
    ZVAL_BOOL(tmpVal[1], val);

    int ret = 0;
    w_call_user_function(ret,
                FunTable() , NULL , &function_name,
                retval, 2 , tmpVal TSRMLS_CC);
    if (ret != SUCCESS)
    {
        LOGE("call func_get_args error");
        ret = -1;
        goto EXIT;
    }

EXIT:

    zval_dtor(&function_name);

#if PHP_VERSION_ID >= 70000

    efree(tmpVal[1]);

#else
    zval_ptr_dtor(&tmpVal[1]);
#endif

    return ret;
}


void full_fill_frame_with_zval(pt_frame_t* frame, zval*val)
{
    //args
#if PHP_MAJOR_VERSION < 7
    zval **tmpzval = NULL;
    if(zend_hash_find(Z_ARRVAL_P(val), EXP_STR("args"), (void **)&tmpzval) == 0 ) // success
    {
         ZVAL_ZVAL(frame->args, *tmpzval, 1, 0);
    }
#else
    zval* p_zval= NULL;
    zend_string* key =  zend_string_init(EXP_STR("args"), 0);
    if((p_zval = zend_hash_find(Z_ARRVAL_P(val), key)) != NULL)
    {
         ZVAL_ZVAL(frame->args, p_zval, 1, 0);
    }
    zend_string_release(key);
#endif

}


void call_php_kernel_debug_backtrace(int skip_last, pt_frame_t &frame, int options)
{
//    zval ret_val;
#if PHP_MAJOR_VERSION < 7
    zval * ret_val  = NULL;
    MAKE_STD_ZVAL(ret_val);

#if PHP_VERSION_ID >= 50400
    zend_fetch_debug_backtrace(ret_val, skip_last, options, 1);
#else
    zend_fetch_debug_backtrace(ret_val, skip_last, options);
#endif

    zval** args = NULL;
    zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(ret_val), NULL);
    if(zend_hash_get_current_data_ex(Z_ARRVAL_P(ret_val), (void **) &args, NULL) == 0)
    {
        full_fill_frame_with_zval(&frame, *args);
    }
    w_zval_ptr_dtor(ret_val);
#else
    zval  ret_val;
    zend_fetch_debug_backtrace(&ret_val, skip_last, options, 1);

    HashPosition pos;
    zval * current = NULL;
    zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(&ret_val), &pos);
    if( current = zend_hash_get_current_data_ex(Z_ARRVAL_P(&ret_val), &pos))
    {
        full_fill_frame_with_zval(&frame, current);
    }
    zval_dtor(&ret_val);
#endif

}
