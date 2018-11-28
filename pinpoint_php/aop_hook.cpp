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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "aop_hook.h"
#include "php_common.h"
#include "php_pinpoint.h"



using  Pinpoint::FAILED;
using  Pinpoint::Agent::Agent;
using  Pinpoint::Plugin::Plugin;
using  Pinpoint::Plugin::RequestHeader;
using  Pinpoint::Plugin::InterceptorPtr;

using Pinpoint::utils::FormatConverter;


using Pinpoint::Plugin::notSupportedFuncResultFetcher;
using Pinpoint::Plugin::notSupportedFuncArgFetcher;

//<editor-fold desc="PHPFuncArgFetcher">

class PHPFuncArgFetcher : public AbstractPHPFuncArgFetcher
{
public:
    PHPFuncArgFetcher();
    virtual zval* getSelf();
    virtual int32_t setInArgs(pt_frame_t &pt);
    virtual zval* getArgs();
private:
    zval* args;
    zval* self;
};

PHPFuncArgFetcher::PHPFuncArgFetcher()
{
    self = NULL;
    args = NULL;
}

zval* PHPFuncArgFetcher::getArgs()
{
    return this->args;
}

zval* PHPFuncArgFetcher::getSelf()
{
    return this->self;
}

int32_t PHPFuncArgFetcher::setInArgs(pt_frame_t& pt)
{

    this->args = pt.args;
    this->self = pt.self;
    return SUCCESS;
}
//</editor-fold>

//<editor-fold desc="PhpFuncResultFetcher">

class PhpFuncResultFetcher : public AbstractPHPResultFetcher
{
public:
    PhpFuncResultFetcher(pt_frame_t& pt);
    virtual zval* getResult();
private:
    zval* result;
};

PhpFuncResultFetcher::PhpFuncResultFetcher(pt_frame_t& pt)
{
    result = pt.retval;
}

zval* PhpFuncResultFetcher::getResult()
{
    return result;
}
//</editor-fold>


#if PHP_VERSION_ID < 50500
static void (*ori_execute)(zend_op_array *op_array TSRMLS_DC);
static void (*ori_execute_internal)(zend_execute_data *execute_data_ptr, int return_value_used TSRMLS_DC);
ZEND_API void pp_execute(zend_op_array *op_array TSRMLS_DC);
ZEND_API void pp_execute_internal(zend_execute_data *execute_data, int return_value_used TSRMLS_DC);
#elif PHP_VERSION_ID < 70000
static void (*ori_execute_ex)(zend_execute_data *execute_data TSRMLS_DC);
static void (*ori_execute_internal)(zend_execute_data *execute_data_ptr, zend_fcall_info *fci, int return_value_used TSRMLS_DC);
ZEND_API void pp_execute_ex(zend_execute_data *execute_data TSRMLS_DC);
ZEND_API void pp_execute_internal(zend_execute_data *execute_data, zend_fcall_info *fci, int return_value_used TSRMLS_DC);
#else
static void (*ori_execute_ex)(zend_execute_data *execute_data);
static void (*ori_execute_internal)(zend_execute_data *execute_data, zval *return_value);
ZEND_API void pp_execute_ex(zend_execute_data *execute_data);
ZEND_API void pp_execute_internal(zend_execute_data *execute_data, zval *return_value);
#endif


void (*old_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);

void (*old_zend_throw_exception_hook)(zval* ex TSRMLS_DC);

void apm_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);


/**
 * Obtain zend function
 * -------------------
 */
static inline zend_function *obtain_zend_function(zend_bool internal, zend_execute_data *ex, zend_op_array *op_array TSRMLS_DC)
{
#if PHP_VERSION_ID < 50500
    if (internal || ex) {
        return ex->function_state.function;
    } else {
        return (zend_function *) op_array;
    }
#elif PHP_VERSION_ID < 70000
    return ex->function_state.function;
#else
    return ex->func;
#endif
}

static void frame_build(pt_frame_t *frame, zend_bool internal, unsigned char type, zend_execute_data *caller, zend_execute_data *execute_data, zend_op_array *op_array TSRMLS_DC)
{
    if (!zend_is_executing()) {
        return ;
    }

    const char* funcname  =  get_active_function_name();


/// adjust difference between php
#if PHP_VERSION_ID >= 70000
    zend_function *func;
    func = EG(current_execute_data)->func;
    if( func->type == ZEND_INTERNAL_FUNCTION &&
        func->common.function_name  == NULL)
    {
        funcname = NULL;
    }
#endif

    if(funcname && strncmp(funcname,"{closure}",sizeof("{closure}") -1) == 0)
    {
        const char* filename  = zend_get_executed_filename();
        LOGD("filename %s",filename);
        const char* offset = strrchr(filename,'/');
        const int   lineno    = zend_get_executed_lineno();
        snprintf(frame->fullname,NAME_LEN*2, "closure{%s:%d}",(offset?(offset+1):(NULL)), lineno);
    }else
    {
        const char* classname = get_active_class_name(NULL);
        if(classname[0] != 0)
        {
            snprintf(frame->fullname,NAME_LEN*2,"%s::%s",classname,funcname);
        }
        else
        {
            snprintf(frame->fullname,NAME_LEN*2,"%s",funcname);
        }
    }

#if 0
    zend_function *zf;

    /* init */
    memset(frame, 0, sizeof(pt_frame_t));

#if PHP_VERSION_ID < 50500
    if (internal || execute_data) {
        op_array = execute_data->op_array;
    }
#endif

    /* zend function */
    zf = obtain_zend_function(internal, execute_data, op_array TSRMLS_CC);
    /// add more logger
    /// PP_U_TRACE("get_active_function_name: %s ",  get_active_function_name());
    /* types, level */
    frame->type = type;
    frame->functype = internal ? PT_FUNC_INTERNAL : 0x00;

    frame->arg_count = 0;

    /* names */
    if (zf->common.function_name)
    {
        /* functype, class name */
#if PHP_VERSION_ID < 70000
        if (caller && P7_EX_OBJ(caller)) {
#else
        if (execute_data && P7_EX_OBJ(execute_data))
        {
#endif
            frame->functype |= PT_FUNC_MEMBER;
            /* User care about which method is called exactly, so use
             * zf->common.scope->name instead of ex->object->name. */
            if (zf->common.scope) {
                snprintf(frame->classname ,NAME_LEN,"%s::",P7_STR(zf->common.scope->name));
            } else {

                php_error(E_WARNING, "Trace catch a entry with ex->object but without zf->common.scope");
            }
// note self only valid in a normal member function
#if  PHP_VERSION_ID < 70000
            if (caller != NULL)
            {
                frame->self = caller->object;
            }
#else
            frame->self = &execute_data->This;
#endif
        } else if (zf->common.scope)
        {
            frame->functype |= PT_FUNC_STATIC;
            snprintf( frame->classname,NAME_LEN ,"%s::",P7_STR(zf->common.scope->name));
        } else
        {
            frame->functype |= PT_FUNC_NORMAL;
        }

        /* function name */
        if (strcmp(P7_STR(zf->common.function_name), "{closure}") == 0) {
            // drop the end line, filename+start line could make it unique
            //  snprintf(frame->function,NAME_LEN, "closure:%s:%d-%d", P7_STR(zf->op_array.filename), zf->op_array.line_start, zf->op_array.line_end);
            const char* offset = strrchr(P7_STR(zf->op_array.filename),'/');
            snprintf(frame->function,NAME_LEN, "closure{%s:%d}",(offset?(offset+1):(NULL)), zf->op_array.line_start);
        } else if (strcmp(P7_STR(zf->common.function_name), "__lambda_func") == 0) {
            snprintf( frame->function ,NAME_LEN, "{lambda:%s}", P7_STR(zf->op_array.filename));
#if PHP_VERSION_ID >= 50414
        } else if (zf->common.scope && zf->common.scope->trait_aliases) {
            strncpy( frame->function ,P7_STR(zend_resolve_method_name(P7_EX_OBJ(execute_data) ? P7_EX_OBJCE(execute_data) : zf->common.scope, zf)),NAME_LEN);
#endif
        } else {
            strncpy( frame->function , P7_STR(zf->common.function_name),NAME_LEN);
        }

        // add full name
        snprintf(frame->fullname,sizeof(frame->fullname),"%s%s",frame->classname,frame->function);
    }


#if PHP_VERSION_ID >= 70000

    if (caller && caller->opline && caller->prev_execute_data &&
            caller->func && caller->func->op_array.opcodes == NULL) {
        caller = caller->prev_execute_data;
    }

    /* skip internal handler */
    if (caller && caller->opline && caller->prev_execute_data &&
            caller->opline->opcode != ZEND_DO_FCALL &&
            caller->opline->opcode != ZEND_DO_ICALL &&
            caller->opline->opcode != ZEND_DO_UCALL &&
            caller->opline->opcode != ZEND_DO_FCALL_BY_NAME &&
            caller->opline->opcode != ZEND_INCLUDE_OR_EVAL) {
        caller = caller->prev_execute_data;
    }
#endif


    /* filename */
#if PHP_VERSION_ID < 70000
    if (caller && caller->op_array) {
        op_array = caller->op_array;
    } else if (caller && caller->prev_execute_data && caller->prev_execute_data->op_array) {
        op_array = caller->prev_execute_data->op_array; /* try using prev */
    }
#else
    if (caller->func && ZEND_USER_CODE(caller->func->common.type)) {
        op_array = &(caller->func->op_array);
    } else if (caller->prev_execute_data && caller->prev_execute_data->func &&
            ZEND_USER_CODE(caller->prev_execute_data->func->common.type)) {
        op_array = &(caller->prev_execute_data->func->op_array); /* try using prev */
    }
#endif
    /* Same as upper
     * } else if (caller != EG(current_execute_data) && EG(current_execute_data)->op_array) {
     *     op_array = EG(current_execute_data)->op_array [> try using current <]
     * }
     */
    if (op_array) {
       strncpy( frame->filename , P7_STR(op_array->filename),NAME_LEN);
    } else {
        frame->filename [0] = 0;
    }

#endif
}

#if PHP_VERSION_ID < 70000
static void frame_set_retval(pt_frame_t *frame, zend_bool internal, zend_execute_data *ex, zend_fcall_info *fci TSRMLS_DC)
{
    zval *retval = NULL;

    if (internal) {
        /* Ensure there is no exception occurs before fetching return value.
         * opline would be replaced by the Exception's opline if exception was
         * thrown which processed in function zend_throw_exception_internal().
         * It may cause a SEGMENTATION FAULT if we get the return value from a
         * exception opline. */
#if PHP_VERSION_ID >= 50500
        if (fci != NULL) {
            retval = *fci->retval_ptr_ptr;
        } else if (ex->opline && !EG(exception)) {
            retval = EX_TMP_VAR(ex, ex->opline->result.var)->var.ptr;
        }
#else
        if (ex->opline && !EG(exception)) {
#if PHP_VERSION_ID < 50400
            retval = ((temp_variable *)((char *) ex->Ts + ex->opline->result.u.var))->var.ptr;
#else
            retval = ((temp_variable *)((char *) ex->Ts + ex->opline->result.var))->var.ptr;
#endif
        }
#endif
    } else if (*EG(return_value_ptr_ptr)) {
        retval = *EG(return_value_ptr_ptr);
    }

    if (retval) {
        frame->retval = retval;
    }
}
#endif


static void free_frame(pt_frame_t & frame)
{
    if (frame.args != NULL)
    {
//        zend_hash_apply(frame.args->value.ht, free_zval);
//        LOGI("%d",zval_refcount_p(frame.args));
        if(zval_refcount_p(frame.args)>1)
        {
            zval_set_refcount_p(frame.args,1);
        }
        w_zval_ptr_dtor(frame.args);
        frame.args = NULL;
    }
}


#if PHP_VERSION_ID < 50500
ZEND_API void pp_execute_origin_core(int internal, zend_execute_data *execute_data, zend_op_array *op_array, int rvu TSRMLS_DC)
#elif PHP_VERSION_ID < 70000
ZEND_API void pp_execute_origin_core(int internal, zend_execute_data *execute_data, zend_fcall_info *fci, int rvu TSRMLS_DC)
#else
ZEND_API void pp_execute_origin_core(int internal, zend_execute_data *execute_data, zval *return_value)
#endif
{

#if PHP_VERSION_ID < 50500
        if (internal) {
            if (ori_execute_internal) {
                ori_execute_internal(execute_data, rvu TSRMLS_CC);
            } else {
                execute_internal(execute_data, rvu TSRMLS_CC);
            }
        } else {
            ori_execute(op_array TSRMLS_CC);
        }
#elif PHP_VERSION_ID < 70000
        if (internal) {
            if (ori_execute_internal) {
                ori_execute_internal(execute_data, fci, rvu TSRMLS_CC);
            } else {
                execute_internal(execute_data, fci, rvu TSRMLS_CC);
            }
        } else {
            ori_execute_ex(execute_data TSRMLS_CC);
        }
#else
        if (internal) {
            if (ori_execute_internal) {
                ori_execute_internal(execute_data, return_value);
            } else {
                execute_internal(execute_data, return_value);
            }
        } else {
            ori_execute_ex(execute_data);
        }
#endif

}


#if PHP_VERSION_ID < 50500
ZEND_API void pp_execute_plugin_core(int internal, zend_execute_data *execute_data, zend_op_array *op_array, int rvu TSRMLS_DC)
#elif PHP_VERSION_ID < 70000
ZEND_API void pp_execute_plugin_core(int internal, zend_execute_data *execute_data, zend_fcall_info *fci, int rvu TSRMLS_DC)
#else
ZEND_API void pp_execute_plugin_core(int internal, zend_execute_data *execute_data, zval *return_value)
#endif
{
    uint64_t call_id = Pinpoint::Plugin::INVALID_CALL_ID;
    zend_execute_data *caller = execute_data;

#if PHP_VERSION_ID < 70000
    zval *retval = NULL;
#else
    zval retval;
    bool retAlive = false;
#endif
    pt_frame_t frame = {0};

#if PHP_VERSION_ID >= 70000
    if (execute_data->prev_execute_data) {
        caller = execute_data->prev_execute_data;
    }

#elif PHP_VERSION_ID >= 50500
    /* In PHP 5.5 and later, execute_data is the data going to be executed, not
     * the entry point, so we switch to previous data. The internal function is
     * a exception because it's no need to execute by op_array. */
    if (!internal && execute_data->prev_execute_data) {
        caller = execute_data->prev_execute_data;
    }
#endif

    frame_build(&frame, internal, PT_FRAME_ENTRY, caller, execute_data, NULL TSRMLS_CC);

    PHPFuncArgFetcher phpFuncArgFetcher;

    PhpAop* aop = PhpAop::getInstance();
    const Pinpoint::Plugin::InterceptorPtr& interceptorPtr =
            (aop != NULL ? aop->getInterceptorPtr(frame.fullname) : InterceptorManager::NULL_INTERCEPTOR);

    if(interceptorPtr != NULL)
    {

        PP_U_TRACE("call %s's interceptorPtr::onBefore",frame.fullname);
        /* Register return value ptr */
    #if PHP_VERSION_ID < 70000
        if (!internal && EG(return_value_ptr_ptr) == NULL) {
            EG(return_value_ptr_ptr) = &retval;
        }
    #else
        if (!internal && execute_data->return_value == NULL) {
            ZVAL_UNDEF(&retval); /* initialize return value type */
    #if PHP_VERSION_ID < 70100
            Z_VAR_FLAGS(retval) = 0;
    #endif
            execute_data->return_value = &retval;
            retAlive =  true;
        }
    #endif

        MAKE_STD_ZVAL(frame.args);

        // fix php 5.6 frame.args init to NULL is a good choice
        ZVAL_NULL(frame.args);

        // fix php > 5.4 needs to setting DEBUG_BACKTRACE_PROVIDE_OBJECT
#if PHP_VERSION_ID <= 50306
        call_php_kernel_debug_backtrace(0,frame,1);
#elif PHP_VERSION_ID <= 50500
        call_php_kernel_debug_backtrace(0,frame,DEBUG_BACKTRACE_PROVIDE_OBJECT);
#elif PHP_VERSION_ID < 70000
        call_php_kernel_debug_backtrace(internal==1 ? (0): (1),frame,DEBUG_BACKTRACE_PROVIDE_OBJECT);
#else
        call_php_kernel_debug_backtrace(0,frame,DEBUG_BACKTRACE_PROVIDE_OBJECT);
#endif
        PINPOINT_G(prs).stackDepth++;
        phpFuncArgFetcher.setInArgs(frame);
        call_id = interceptorPtr->assignCallId();
        (void)aop->resetCurrentInterceptor(interceptorPtr, call_id);
        interceptorPtr->before(call_id, phpFuncArgFetcher);
        PINPOINT_G(prs).stackDepth--;
    }

#if PHP_VERSION_ID < 50500
        if (internal) {
            if (ori_execute_internal) {
                ori_execute_internal(execute_data, rvu TSRMLS_CC);
            } else {
                execute_internal(execute_data, rvu TSRMLS_CC);
            }
        } else {
            ori_execute(op_array TSRMLS_CC);
        }
#elif PHP_VERSION_ID < 70000
        if (internal) {
            if (ori_execute_internal) {
                ori_execute_internal(execute_data, fci, rvu TSRMLS_CC);
            } else {
                execute_internal(execute_data, fci, rvu TSRMLS_CC);
            }
        } else {
            ori_execute_ex(execute_data TSRMLS_CC);
        }
#else
        if (internal) {
            if (ori_execute_internal) {
                ori_execute_internal(execute_data, return_value);
            } else {
                execute_internal(execute_data, return_value);
            }
        } else {
            ori_execute_ex(execute_data);
        }
#endif


     if (interceptorPtr != NULL)
     {
#if PHP_VERSION_ID < 50500
            frame_set_retval(&frame, internal, execute_data, NULL TSRMLS_CC);

#elif PHP_VERSION_ID < 70000
            frame_set_retval(&frame, internal, execute_data, fci TSRMLS_CC);

#else
            if (return_value) { /* internal */
                frame.retval = return_value;
            } else if (execute_data->return_value) { /* user function */
                frame.retval = execute_data->return_value;
            }
#endif


         PhpFuncResultFetcher phpFuncResultFetcher(frame);
         PP_U_TRACE("call %s's interceptorPtr::onEnd",frame.fullname);
         PINPOINT_G(prs).stackDepth++;
         interceptorPtr->end(call_id, phpFuncArgFetcher, phpFuncResultFetcher);
         PINPOINT_G(prs).stackDepth--;

         (void)aop->resetCurrentInterceptor();
         free_frame(frame);

#if PHP_VERSION_ID < 70000
        /* Free return value */
    if (!internal && retval != NULL) {
        zval_ptr_dtor(&retval);
        EG(return_value_ptr_ptr) = NULL;
    }
#else

    if (retAlive) {
        zval_dtor(&retval);
    }
#endif
     }

}


#if PHP_VERSION_ID < 50500
ZEND_API void pp_execute(zend_op_array *op_array TSRMLS_DC)
{
    if(RunOriginExecute::running() == 0)
    {
        pp_execute_plugin_core(0, EG(current_execute_data), op_array, 0 TSRMLS_CC);
    }
    else
    {
        pp_execute_origin_core(0, EG(current_execute_data), op_array, 0 TSRMLS_CC);
    }
}

ZEND_API void pp_execute_internal(zend_execute_data *execute_data, int return_value_used TSRMLS_DC)
{
    if(RunOriginExecute::running() == 0)
    {
        pp_execute_plugin_core(1, execute_data, NULL, return_value_used TSRMLS_CC);
    }
    else
    {
        pp_execute_origin_core(1, execute_data, NULL, return_value_used TSRMLS_CC);
    }
}
#elif PHP_VERSION_ID < 70000
ZEND_API void pp_execute_ex(zend_execute_data *execute_data TSRMLS_DC)
{
    if(RunOriginExecute::running() == 0)
    {
        pp_execute_plugin_core(0, execute_data, NULL, 0 TSRMLS_CC);
    }
    else
    {
        pp_execute_origin_core(0, execute_data, NULL, 0 TSRMLS_CC);
    }
}

ZEND_API void pp_execute_internal(zend_execute_data *execute_data, zend_fcall_info *fci, int return_value_used TSRMLS_DC)
{
    if(RunOriginExecute::running() == 0){
        pp_execute_plugin_core(1, execute_data, fci, return_value_used TSRMLS_CC);
    }else{
       pp_execute_origin_core(1, execute_data, fci, return_value_used TSRMLS_CC);
    }
}
#else
ZEND_API void pp_execute_ex(zend_execute_data *execute_data)
{
    if(RunOriginExecute::running() == 0){
        pp_execute_plugin_core(0, execute_data, NULL);
    }else{
        pp_execute_origin_core(0, execute_data, NULL);
    }
}

ZEND_API void pp_execute_internal(zend_execute_data *execute_data, zval *return_value)
{
    if(RunOriginExecute::running() == 0){
      pp_execute_plugin_core(1, execute_data, return_value);
    }else
    {
       pp_execute_origin_core(1, execute_data, return_value);
    }
}
#endif

void apm_throw_exception_plugin_hook(EG_EXP_TPYE exception TSRMLS_DC)
{

    PhpAop* aop = PhpAop::getInstance();
    if (aop != NULL)
    {
        CurrentInterceptorInfo currentInterceptorInfo = aop->getCurrentInterceptorInfo();
        Pinpoint::Plugin::InterceptorPtr interceptorPtr = currentInterceptorInfo.first;
        uint64_t call_id = currentInterceptorInfo.second;
        if (interceptorPtr != NULL)
        {
            PINPOINT_ASSERT((call_id != Pinpoint::Plugin::INVALID_CALL_ID));

            Pinpoint::Plugin::ExceptionInfo exceptionInfo = get_exception_info(exception);
            interceptorPtr->exception(call_id, exceptionInfo);
            PP_U_TRACE("[EXCEPTION] file:[%s] line:[%d] msg:[%s]",exceptionInfo.file.c_str(),exceptionInfo.line,exceptionInfo.message.c_str());
        }
    }

}


void apm_throw_exception_hook(zval* exception TSRMLS_DC)
{
    if(!exception){
        return ;
    }
    /// dynamically exception, but must be an error
    /// error_reporting ignore this, agent does it also
    if( EG(error_reporting) & AGENT_ERROR )
    {
    #if PHP_VERSION_ID < 70000
        apm_throw_exception_plugin_hook(exception TSRMLS_CC);
    #else
        apm_throw_exception_plugin_hook(Z_OBJ_P(exception) TSRMLS_CC);
    #endif
    }

    if(old_zend_throw_exception_hook ){
#if PHP_VERSION_ID >= 70000
      zval rv;
      ZVAL_OBJ(&rv, Z_OBJ_P(exception));
      old_zend_throw_exception_hook(&rv TSRMLS_DC);
#else
      old_zend_throw_exception_hook(exception);
#endif
    }
}



/* {{{ void apm_error(int type, const char *format, ...)
    This function provides a hook for error */
void apm_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args)
{
    char *msg;
    PhpAop* aop = NULL;
    va_list args_copy;
    TSRMLS_FETCH();

    /* A copy of args is for the old_error_cb */
    va_copy(args_copy, args);
    vspprintf(&msg, 0, format, args_copy);
    va_end(args_copy);

    aop = PhpAop::getInstance();

    do
    {
        /// fatal error, avoiding calling multiple times
        if(PINPOINT_G(prs).fatal_error_catched == true)
        {
            // error had been caught
            break;
        }

        // keep the same with error_reporting
        if(!(EG(error_reporting) & type) )
        {
            break;
        }

        if (aop == NULL)
        {
            break;
        }

        Pinpoint::Plugin::InterceptorPtr interceptorPtr =
            aop->getRequestInterceptorPtr();
        if (interceptorPtr == NULL)
        {
            break;
        }

        /// treats this type as an error in agent
        if(type & AGENT_ERROR)
        {
            Pinpoint::Plugin::ErrorInfo errorInfo;
            errorInfo.message = msg;
            errorInfo.file = error_filename;
            errorInfo.line = error_lineno;
            interceptorPtr->error(errorInfo);
            PINPOINT_G(prs).fatal_error_catched = true;
        }
        else if (type & AGENT_WARNG)  /// treats this type as an warning in agent
        {
            using namespace Pinpoint::Trace;
            TracePtr tracePtr = Trace::Trace::getCurrentTrace();
            if (tracePtr == NULL)
            {
                break;
            }
            try
            {
                std::string errorMsg = Pinpoint::utils::FormatConverter::formatMsg("Warning", error_filename,
                                                                                   error_lineno, msg);
                tracePtr->setExceptionInfo(Pinpoint::Plugin::WARNINGS_STRING_ID, errorMsg);
                PINPOINT_G(prs).stackDepth = 0;
                PP_U_TRACE("[ERROR] file:[%s] line:[%d] msg:[%s]",error_filename,error_lineno,msg);
            }
            catch (...)
            {
                tracePtr->setExceptionInfo(Pinpoint::Plugin::WARNINGS_STRING_ID, msg);
            }
        }else{
            // do nothing
        }
    } while (0);

    efree(msg);

    old_error_cb(type, error_filename, error_lineno, format, args);
}
/* }}} */


int32_t init_aop()
{
    int32_t err;

    try
    {
        InterceptorManagerPtr interceptorManagerPtr(new InterceptorManagerBasedMap);
        err = PhpAop::createInstance(interceptorManagerPtr);
    }
    catch (std::bad_alloc&)
    {
        return Pinpoint::FAILED;
    }

    if (err != SUCCESS)
    {
        return Pinpoint::FAILED;
    }

    return SUCCESS;
}

int32_t turn_on_aop()
{

#if PHP_VERSION_ID < 50500
    ori_execute = zend_execute;
    zend_execute = pp_execute;
#else
    ori_execute_ex = zend_execute_ex;
    zend_execute_ex = pp_execute_ex;
#endif

    ori_execute_internal = zend_execute_internal;
    zend_execute_internal = pp_execute_internal;


    if(PINPOINT_G(trace_exception))
    {
        old_zend_throw_exception_hook = zend_throw_exception_hook;
        zend_throw_exception_hook = apm_throw_exception_hook;
    }

    old_error_cb = zend_error_cb;
    zend_error_cb = apm_error_cb;

    RunOriginExecute::stop();

    return SUCCESS;
}

int32_t turn_off_aop()
{
    PhpAop::freeAop();

    /* Restore original executor */
#if PHP_VERSION_ID < 50500
    zend_execute = ori_execute;
#else
    zend_execute_ex = ori_execute_ex;
#endif

    zend_execute_internal = ori_execute_internal;

    zend_error_cb = old_error_cb;


    if(PINPOINT_G(trace_exception))
    {
        zend_throw_exception_hook = old_zend_throw_exception_hook;
    }

    return SUCCESS;
}

int32_t add_interceptor(const Pinpoint::Plugin::InterceptorPtr& interceptorPtr)
{
    PhpAop* aop = PhpAop::getInstance();
    if (aop == NULL)
    {
        LOGE("php aop == NULL");
        return FAILED;
    }

    return aop->addInterceptorPtr(interceptorPtr);
}
