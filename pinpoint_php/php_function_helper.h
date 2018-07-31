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

#ifndef PINPOINT_PHP_PHP7_PHP_FUNCTION_HELPER_H_
#define PINPOINT_PHP_PHP7_PHP_FUNCTION_HELPER_H_

#include "php_define_wapper.h"
#include "pinpoint_api.h"

#include "zend_builtin_functions.h"
extern "C"{
    #include "ext/standard/php_var.h"
}


ZEND_FUNCTION(func_get_args);
int call_php_kernel_unserialize(zval* retval_ptr, zval *params);
int call_php_kernel_serialize(zval *retval, zval* objzval);
int call_php_kernel_print_r(zval *retval, zval* objzval);

inline int call_php_kernel_array_push(zval* dstArray, const char *str, uint length)
{
#if PHP_MAJOR_VERSION < 7
    add_next_index_stringl(dstArray, str, length, 1);
#else
    add_next_index_stringl(dstArray, str, length);
#endif

    return 0;
}

void call_php_kernel_debug_backtrace(int internal, pt_frame_t &frame, int options);


#endif /* PINPOINT_PHP_PHP7_PHP_FUNCTION_HELPER_H_ */
