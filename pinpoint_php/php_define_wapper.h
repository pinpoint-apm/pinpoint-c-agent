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

#ifndef PINPOINT_PHP_PHP7_PHP_DEFINE_WAPPER_H_
#define PINPOINT_PHP_PHP7_PHP_DEFINE_WAPPER_H_

#include "php.h"
#include "zend_interfaces.h"
#include "zend_extensions.h"
#include "zend_exceptions.h"


#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
typedef enum {
    W_ZEND_CALL_SUCCESS = 0,
    W_ZEND_CALL_FAILED,
    W_ZEND_CALL_EXCEPTION
} W_ZEND_CALL_RESULT;

#if PHP_MAJOR_VERSION < 7

#include <Zend/zend_interfaces.h>

extern "C" {
#ifndef SUCCESS
#define SUCCESS (0)
#endif

#include <ext/standard/php_var.h>
//#include <ext/standard/php_smart_string.h>
}

#define zend_long int64_t
#define ZVAL_STRING_SET(zvl,str)   ZVAL_STRING(zvl, str,1)
#define STR_TYPE   char
#define _PSTR(v)       v
#define _PSTR_LEN(v)   strlen(v)
#define STR_SPEC        "s"

#define w_php_var_serialize(buf,struc,data) php_var_serialize(buf,struc,data)
#define w_php_var_unserialize(rval, p, max, var_hash, classes) php_var_unserialize(rval, p, max, var_hash)

#define P7_STR(v)       v
#define P7_STR_LEN(v)   strlen(v)

#define w_smt_prt_str(v) (v.c)
#define w_zend_fetch_class(cn) zend_fetch_class(cn,strlen(cn),ZEND_FETCH_CLASS_SILENT TSRMLS_CC)

// note: p is drop
#define w_zend_string_init(str,l,p) estrndup(str,l)
#define zend_string_release(str) efree(str)


// caution: w_zend_call_method is called by PHP plugin. We assume the plugin is coded correctly and don't throw
// anything. But if throw, we clear the exception and turn off the collection.
typedef zval* EG_EXP_TPYE;
#if PHP_VERSION_ID <50400

#define w_zend_call_method(_1, _2, _3, _4, _5, _6, _7, _8, _9) \
        do{\
            zval* t_1 = _1;\
            zval* t_6 = _6;\
            zend_call_method(&t_1, _2, _3, const_cast<char*>(_4), _5, (_6 == NULL)? (NULL):(&t_6), _7, _8, _9 TSRMLS_CC);\
         }while(0)

#else

#define w_zend_call_method( _1, _2, _3, _4, _5, _6, _7, _8, _9) \
        do{\
            zval* t_1 = _1;\
            zval* t_6 = _6;\
            zend_call_method(&t_1, _2, _3, _4, _5,(_6 == NULL)? (NULL):(&t_6), _7, _8, _9 TSRMLS_CC);\
         }while(0)

#endif

#define FunTable() EG(function_table)

#define w_call_user_function(ret,_1, _2, _3, _4, _5, _6) \
    do{\
        ret = call_user_function(_1, _2, _3, _4, _5, _6);\
    }while(0);


//#if PHP_VERSION_ID >= 50527  &&  PHP_VERSION_ID <= 50610
//#define w_zval_ptr_dtor(ret)
//#else

#define w_zval_ptr_dtor(ret) \
        do{\
            if(ret){\
            zval* tval = (ret);\
            zval_ptr_dtor(&tval);\
            }\
        }while(0);

//#endif

#define w_ref_pre_excute_date (EG(current_execute_data)->prev_execute_data)
#define W_ZVAL_STRINGL(_1,_2,_3) ZVAL_STRINGL(_1,_2,_3,1)

#define w_zend_read_property(index,_1,_2,_3,_4,_5)\
        do{\
            index = zend_read_property(_1,_2,const_cast<char*>(_3),_4,_5 TSRMLS_CC);\
        }while(0);

#define EXP_STR(x) (x),(sizeof(x))

#else

#include "zend_types.h"
extern "C" {
#include "zend_smart_str.h"
#include <ext/standard/php_var.h>
#include <ext/standard/php_smart_string.h>
}

#define zend_get_class_entry Z_OBJCE_P
#define STR_TYPE   zend_string
#define _PSTR(v)     ZSTR_VAL(v)
#define _PSTR_LEN(v) ZSTR_LEN(v)
#define HASH_KEY_STR(h) _PSTR((h->key))



#define FunTable()     CG(function_table)
#define ZVAL_STRING_SET(zvl,str)   ZVAL_STRING(zvl, str)
#define STR_SPEC        "S"
#define P7_EX_OBJCE(ex) Z_OBJCE(ex->This)
#define P7_EX_OPARR(ex) (&(ex->func->op_array))
#define P7_STR(v)       ZSTR_VAL(v)
#define P7_STR_LEN(v)   ZSTR_LEN(v)

#define w_php_var_serialize(buf,struc,data) php_var_serialize(buf,*struc,data)
#define w_php_var_unserialize(rval, p, max, var_hash) php_var_unserialize(*(rval), p, max, var_hash)

#define w_smt_prt_str(v) (_PSTR(v.s))
#define w_smt_prt_str_len(v) (_PSTR_LEN(v.s))

#define w_zend_fetch_class(cn) zend_fetch_class(cn,ZEND_FETCH_CLASS_SILENT)
#define w_zend_string_init(str,l,p) zend_string_init(str,l,p)

#define w_zend_call_method(_1, _2, _3, _4, _5, _6, _7, _8, _9) zend_call_method((_1), _2, _3, _4, _5, _6, _7, _8, _9)


typedef zend_object * EG_EXP_TPYE;

#define w_call_user_function(ret,_1, _2, _3, _4, _5, _6) \
    do{\
        zval tval[(_5)];\
        int i = 0;\
        for(;i<(_5);i++)\
        {\
            ZVAL_COPY_VALUE(&tval[i],(_6)[i]);\
        }\
        ret = call_user_function(_1, _2, _3, _4, _5, tval);\
    }while(0);

#define w_zval_ptr_dtor(v) \
    do{\
        if(v){\
          zval_dtor(v);\
        efree(v);\
          }\
    }while(0);

#define w_ref_pre_excute_date (EX(prev_execute_data))
#define W_ZVAL_STRINGL ZVAL_STRINGL

#define w_zend_read_property(index,_1,_2,_3,_4,_5)\
        do{\
            zval zv;\
            index = zend_read_property(_1,_2,_3,_4,_5,&zv);\
        }while(0);



#define ALLOC_INIT_ZVAL(v) \
    do{\
       v = (zval*)emalloc(sizeof(zval));\
    }while(0)


#define MAKE_STD_ZVAL ALLOC_INIT_ZVAL
#define EXP_STR(x) (x),(strlen(x))
// PHP 7
#endif



#if PHP_VERSION_ID < 70000
#define P7_EX_OBJ(ex)   ex->object
#define P7_EX_OBJCE(ex) Z_OBJCE_P(ex->object)
#define P7_EX_OPARR(ex) ex->op_array

#else

#if PHP_VERSION_ID < 70100
/* object fetching in PHP 7.0
 * object = call ? Z_OBJ(call->This) : NULL; */
#define P7_EX_OBJ(ex)   Z_OBJ(ex->This)

#else
/* object fetching in PHP 7.1
 * object = (call && (Z_TYPE(call->This) == IS_OBJECT)) ? Z_OBJ(call->This) : NULL; */
#define P7_EX_OBJ(ex)   (Z_TYPE(ex->This) == IS_OBJECT ? Z_OBJ(ex->This) : NULL)

#endif

#endif



#if PHP_VERSION_ID < 50400
#define w_zend_declare_property_long(_1,_2,_3,_4,_5) zend_declare_property_long(_1,const_cast<char*>(_2),_3,_4,_5)
#else

#define w_zend_declare_property_long zend_declare_property_long

#endif



// common macro
#define PT_FRAME_ENTRY          1 /* entry */
#define PT_FRAME_EXIT           2 /* exit */
#define PT_FRAME_STACK          3 /* backtrace stack */

#define PT_FUNC_INTERNAL        0x80 /* function is ZEND_INTERNAL_FUNCTION */

#define PT_FUNC_TYPES           0x7f /* mask for type of function call */
#define PT_FUNC_INCLUDES        0x10 /* mask for includes type */

#define PT_FUNC_UNKNOWN         0x00
#define PT_FUNC_NORMAL          0x01
#define PT_FUNC_MEMBER          0x02
#define PT_FUNC_STATIC          0x03
#define PT_FUNC_EVAL            0x04
#define PT_FUNC_INCLUDE         0x10
#define PT_FUNC_INCLUDE_ONCE    0x11
#define PT_FUNC_REQUIRE         0x12
#define PT_FUNC_REQUIRE_ONCE    0x13

#define __STDC_LIMIT_MACROS 
#include <stdint.h>
        
#define NAME_LEN 128

typedef struct _pt_frame_t{
    uint8_t type;               /* frame type, entry or exit */
    uint8_t functype;           /* function flags of PT_FUNC_xxx */
    uint32_t lineno;            /* entry lineno */
    uint32_t level;             /* nesting level */

    uint32_t arg_count;         /* arguments number */

    zval* retval;
    zval* args;
    zval* self;

    int64_t inc_time;           /* inclusive wall time */
    int64_t exc_time;           /* exclusive wall time */
    char filename[NAME_LEN];               /* entry filename */
    char classname[NAME_LEN];                  /* class name */
    char function[NAME_LEN];               /* function name */
    char fullname[NAME_LEN*2];                /* class name + function name*/
}pt_frame_t;

#endif /* PINPOINT_PHP_PHP7_PHP_DEFINE_WAPPER_H_ */
