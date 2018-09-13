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
#include "php_common.h"
#include "pinpoint_type.h"
#include <cstdio>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>    // std::for_each
#include <map>
#include <boost/algorithm/string/replace.hpp>

#include "interceptor.h"
#include "php_function_helper.h"
#include "aop_hook.h"
#include "zend_types.h"

#if PHP_VERSION_ID >=50400
#include "zend_string.h"
#endif

#include "zend_hash.h"
#include "SAPI.h"
///**********************************************************/



using Pinpoint::Plugin::RequestHeader;
using Pinpoint::Plugin::HttpHeader;
using Pinpoint::Plugin::HeaderMap;

#define DEFAULT_LOGPATH "/tmp"
#define DEFAULT_LOGLEVER "DEBUG"


using std::string ;
static std::map<string, string> _moduleInfo;


int RunOriginExecute::iRunning = 0 ;
int RunOriginExecute::iTimes = 0;

static inline std::string getNameFromHeaderMap(const char* pstr)
{
    HttpHeader* header = RequestHeader::getCurrentRequestHeader();
    if( header ==  NULL)
    {
        return "";
    }
    std::string tValue;
    header->getHeader(pstr,tValue);
    return tValue;
}
/**
 * find_first_of_char(abcdef, 1,b) ->1
 * @param pstr
 * @param strLen
 * @param C
 * @return index of pstr (from 1 )
 */
static uint32_t find_first_of_char(const char*pstr, uint32_t strLen ,char C)
{
    uint32_t i =0;
    for( ;pstr != NULL && i < strLen ;pstr++,i++)
    {
        if((char)*pstr == C)
            break;
    }
    return i+1;
}

#if PHP_VERSION_ID < 70000

int32_t get_php_request_headers(HeaderMap& headerMap)
{
    TSRMLS_FETCH();
    if (!zend_is_auto_global(ZEND_STRL(PP_SERVER) TSRMLS_CC))
    {
        LOGD("fail to get _SERVER");
        return -1;
    }
    zval **data, **tmp, tmp2;
    char *string_key;
    uint string_len;
    ulong num_key;
    std::string key;
    if (zend_hash_find(&EG(symbol_table), ZEND_STRS(PP_SERVER), (void **) &data)
            != FAILURE
            && (Z_TYPE_PP(data) == IS_ARRAY))
    {
        zend_hash_internal_pointer_reset(Z_ARRVAL_PP(data));
        while (zend_hash_get_current_data(Z_ARRVAL_PP(data), (void **) &tmp)
                == 0)
        {
            switch (zend_hash_get_current_key_ex(Z_ARRVAL_PP(data), &
                                                 string_key, &string_len, &num_key, 0, NULL))
            {
                case HASH_KEY_IS_STRING:
                    key = string_key;
                    break;
                case HASH_KEY_IS_LONG:
                    key = boost::lexical_cast<std::string>(num_key);
                    break;
            }
            if (Z_TYPE_PP(tmp) != IS_ARRAY)
            {
                tmp2 = **tmp;
                if (Z_TYPE_PP(tmp) != IS_STRING)
                {
                    tmp = NULL;
                    zval_copy_ctor(&tmp2);
                    convert_to_string(&tmp2);
                }
                headerMap[key] = Z_STRVAL(tmp2);
                if (!tmp)
                {
                    zval_dtor(&tmp2);
                }
            }
            zend_hash_move_forward(Z_ARRVAL_PP(data));
        }
    }

#ifdef DBUG_PHP_COMMON
    HeaderMap::iterator it;
    for (it = headerMap.begin(); it != headerMap.end(); it++)
    {
        LOGI("header:[%s]=%s ", it->first.c_str(), it->second.c_str());
    }
#endif


    return 0;
}
#else

int add_header(zval *el, int num_args, va_list args, zend_hash_key *hash_key)
{
    zval *value = el ;
    HeaderMap* map = va_arg(args, HeaderMap *);

    if(Z_TYPE_P(value) == IS_ARRAY){
        return ZEND_HASH_APPLY_KEEP;
    }

    zval tvalue = *value;

    zval_copy_ctor(&tvalue);

    convert_to_string(&tvalue);

    (*map)[HASH_KEY_STR(hash_key)] = std::string(Z_STRVAL_P(&tvalue),Z_STRLEN_P(&tvalue));

    zval_dtor(&tvalue);

    return ZEND_HASH_APPLY_KEEP;
}

int32_t get_php_request_headers(HeaderMap& headerMap)
{
    zval *data ;

    zend_string * table = zend_string_init(PP_SERVER, sizeof(PP_SERVER)-1, 0);

    zend_is_auto_global(table);

    /* find the table and ensure it's an array */
    data = zend_hash_find(&EG(symbol_table), table);
    if (!data || Z_TYPE_P(data) != IS_ARRAY) {
        return NULL;
    }

    zend_hash_apply_with_arguments(Z_ARRVAL_P(data),(apply_func_args_t)add_header
            ,1,(void*)&headerMap);


    //  free table , or else memory leak in apache
    zend_string_free(table);
    return 0;
}

#endif


int32_t get_php_response_headers(Pinpoint::Plugin::HeaderMap& headerMap)
{
    TSRMLS_FETCH();
    zend_llist *headers = &SG(sapi_headers).headers; //headers can't be null

    zend_llist_element *element;

    for (element = headers->head; element; element = element->next)
    {
        sapi_header_struct *sapi_header = (sapi_header_struct *) element->data;
        uint32_t tret = find_first_of_char(sapi_header->header,sapi_header->header_len,':');
        // sdfas:xxxx -> tret -2 = sdfas tret = "sdfas:" tret ="xxxx"
        //
        headerMap[string(sapi_header->header,tret -2)] = string(sapi_header->header+tret,sapi_header->header_len-tret);
    }

#ifdef DBUG_PHP_COMMON
    HeaderMap::iterator it;
    for (it = headerMap.begin(); it != headerMap.end(); it++)
    {
        LOGI("header:[%s]=%s ", it->first.c_str(), it->second.c_str());
    }
#endif

    return SUCCESS;
}


// The three functions return "" if can not get these information.
std::string get_end_point()
{
    std::string strEndPoint;
    strEndPoint = getNameFromHeaderMap(PP_HTTP_X_FORWARDED_HOST);
    if(strEndPoint.empty())
    {
        strEndPoint = getNameFromHeaderMap(PP_HTTP_HOST);
    }
    return strEndPoint.empty() ? "" : strEndPoint;
}

std::string get_remote_addr()
{
    return  getNameFromHeaderMap(PP_REMOTE_ADDR);
}

std::string get_rpc()
{
    return  getNameFromHeaderMap(PP_REQUEST_URI);
}

int32_t get_http_response_status()
{
    try
    {
       std::string codeStr = getNameFromHeaderMap(PP_REDIRECT_STATUS);
       return (codeStr== "" ) ? (sapi_globals.sapi_headers.http_response_code ):(boost::lexical_cast<int32_t>(codeStr));
    }
    catch (...)
    {
        return -1;
    }
}

string get_host_process_info(eName name)
{
    TSRMLS_FETCH();

    if(name <= Pinpoint::Naming::ENAME_MIN || name >= Pinpoint::Naming::ENAME_MAX)
    {
        return "";
    }
    char* pName = const_cast<char*>(Pinpoint::Naming::gNameTable[name]);
    char *pTempValueStr = sapi_getenv(pName, strlen(pName) TSRMLS_CC);
    if (pTempValueStr)
    {
        std::string value(pTempValueStr);
        efree(pTempValueStr);
        return value;
    }
    std::map<string, string>::iterator it;
    it = _moduleInfo.find(pName);
    if (it != _moduleInfo.end())
    {
        return it->second;
    }
    return "";
}

static void init_log_from_ConfTree(Pinpoint::Configuration::Config &config)
{
    _moduleInfo["LogFileRootPath"] = config.readString( "common.LogFileRootPath",
            DEFAULT_LOGPATH);

    _moduleInfo["PPLogLevel"] = config.readString( "common.PPLogLevel",
            DEFAULT_LOGLEVER);

    Pinpoint::log::set_logging_file(_moduleInfo["LogFileRootPath"].c_str());
    Pinpoint::log::set_log_level(_moduleInfo["PPLogLevel"].c_str());
}

void init_evn_before_agent_real_startup(Pinpoint::Configuration::Config &config)
{
    init_log_from_ConfTree(config);
    LOGD(" start agent ...");
}

bool is_interface_impl(zval* obj, const char* interface_name)
{
    TSRMLS_FETCH();

    if (obj == NULL || interface_name == NULL)
    {
        return false;
    }

    if (Z_TYPE_P(obj) != IS_OBJECT)
    {
        return false;
    }

    zend_class_entry *ce = zend_get_class_entry(obj TSRMLS_CC);

    if (ce == NULL)
    {
        return false;
    }

    for (uint32_t i = 0; i < ce->num_interfaces; ++i)
    {
        zend_class_entry *interface = ce->interfaces[i];
        if (interface->name != NULL && strcmp(_PSTR(interface->name), interface_name) == 0)
        {
            return true;
        }
    }

    return false;

}

bool is_class_impl(zval* obj, const char* class_name)
{
    TSRMLS_FETCH();

    if (obj == NULL || class_name == NULL)
    {
        return false;
    }

    if (Z_TYPE_P(obj) != IS_OBJECT)
    {
        return false;
    }

    zend_class_entry *ce = zend_get_class_entry(obj TSRMLS_CC);

    if (ce == NULL)
    {
        return false;
    }

    if (ce->name != NULL && strcmp(_PSTR(ce->name), class_name) == 0)
    {
        return true;
    }

    return ce->parent != NULL && ce->parent->name != NULL && strcmp(_PSTR(ce->parent->name), class_name) == 0;
}

const std::string path_join(std::string _dir, std::string fileName)
{
    namespace fs = boost::filesystem;
    fs::path dir(_dir);
    dir /= fileName;
    return dir.string();
}

// ZEND_HASH_APPLY_STOP
#if PHP_VERSION_ID < 70000
int push_to_map(zval **el, int num_args, va_list args, zend_hash_key *hash_key)
{
    zval* value =*el;
    std::string key="";
    if(hash_key->arKey)
    {
        key.append(hash_key->arKey ,hash_key->nKeyLength);
    }
#else
int push_to_map(zval *el, int num_args, va_list args, zend_hash_key *hash_key)
{
    zval* value =el;
    std::string key;
    if(hash_key->key)
    {
        key.append(ZSTR_VAL(hash_key->key),ZSTR_LEN(hash_key->key));
    }
#endif

    KV* map = va_arg(args, KV *);
    int32_t* elementSize = va_arg(args, int32_t*);
    int32_t* dataSize = va_arg(args, int32_t*);
    int32_t* index = va_arg(args, int32_t*);
    int32_t usageSize = 0;

    if(elementSize == 0 || dataSize == 0 )
    {
        // stop
       goto STOP;
    }

    if(key.length() == 0) // not a map ,just an array
    {
        char buf[64]={0};
        snprintf(buf,sizeof(buf)-1,"%d",*index);
        key=buf;
    }

    if(Z_TYPE_P(value) == IS_ARRAY )
    {
        (*map)[key] = "array(...)";
    }
    else
    {
         std::string tstr = zval_to_string(value,*dataSize);
         usageSize  += tstr.length() + key.length();
        (*map)[key] = tstr;
    }

    if (*elementSize != -1)
    {
        (*elementSize)--;
    }

    if (*dataSize != -1)
    {
        *dataSize -= usageSize;
        *dataSize = (*dataSize < 0) ? (0) : (*dataSize);
    }
    (*index)++;
    return ZEND_HASH_APPLY_KEEP;
STOP:
    return ZEND_HASH_APPLY_STOP;
}


#if PHP_VERSION_ID < 70000
int push_to_vec(zval **el, int num_args, va_list args, zend_hash_key *hash_key)
{
    zval* value =*el;
#else
int push_to_vec(zval *el, int num_args, va_list args, zend_hash_key *hash_key)
{
    zval* value =el;
#endif

    VecStr* vec = va_arg(args, VecStr *);
    int32_t* elementSize = va_arg(args, int32_t*);
    int32_t* dataSize = va_arg(args, int32_t*);
    int32_t* index = va_arg(args, int32_t*);
    int32_t usageSize = 0;

    if (elementSize == 0 || dataSize == 0)
    {
        // stop
        goto STOP;
    }

    if (Z_TYPE_P(value) == IS_ARRAY)
    {
        vec->push_back( "array(...)");
    }
    else
    {
        std::string tstr = zval_to_string(value, *dataSize);
        usageSize += tstr.length();
        vec->push_back(tstr);
    }

    if (*elementSize != -1)
    {
        (*elementSize)--;
    }

    if (*dataSize != -1)
    {
        *dataSize -= usageSize;
        *dataSize = (*dataSize < 0) ? (0) : (*dataSize);
    }
    (*index)++;
    return ZEND_HASH_APPLY_KEEP;
    STOP: return ZEND_HASH_APPLY_STOP;

}


void zval_to_Map(KV &map, zval* val, int32_t maxELemSize,
        int32_t maxDataSize)
{
    if (val == NULL)
    {
        return;
    }

    if (Z_TYPE_P(val) != IS_ARRAY)
    {
        LOGD("why the val is not the array");
        return ;
    }
    int32_t arryIndex= 0;

    zend_hash_apply_with_arguments(Z_ARRVAL_P(val),
            (apply_func_args_t) push_to_map, 1, (void*) &map, &maxELemSize,
            &maxDataSize,&arryIndex);
}



void zval_args_to_vec(VecStr &vec, zval* val, int32_t maxELemSize,int32_t maxDataSize)
{
    if (val == NULL)
    {
        return;
    }

    if (Z_TYPE_P(val) != IS_ARRAY)
    {
        LOGD("why the val is not the array");
        return ;
    }
    int32_t arryIndex= 0;

    zend_hash_apply_with_arguments(Z_ARRVAL_P(val),
            (apply_func_args_t) push_to_vec, 1, (void*) &vec, &maxELemSize,
            &maxDataSize,&arryIndex);
}

void map_to_str(const KV &map,std::string &str)
{
    KV::const_iterator it=map.begin();
    str+="{";
    while(it!=map.end()){
        str+= "["+it->first+"]"+":"+it->second+" ";
        it++;
    }
    str+="}";
}

void vec_to_str(iVecStr istart,iVecStr iend,std::string& out)
{
    for(;istart<iend;++istart){
        out = out + '{' + *istart +"} ";
    }
}



const std::string zval_to_string(zval* value, int32_t limit)
{

    if( value == NULL || limit <=0 )
    {
        return "";
    }

    char buf[128] = { 0 };

#if PHP_VERSION_ID >= 70000
again:
#endif

    switch (Z_TYPE_P(value))
    {
#if PHP_VERSION_ID < 70000
    case IS_BOOL:
        if (Z_LVAL_P(value))
        {
            return "true";
        }
        else
        {
            return "false";
        }
#else
        case IS_TRUE:
        return "true";
        case IS_FALSE:
        return "false";
#endif
    case IS_NULL:
        return "NULL";
    case IS_LONG:
        snprintf(buf, sizeof(buf), "%ld", Z_LVAL_P(value));
        return buf;
    case IS_DOUBLE:
        snprintf(buf, sizeof(buf), "%.*G", (int) EG(precision),
                Z_DVAL_P(value));
        return buf;
    case IS_STRING:
    {
        int32_t tlen = TER_MIN(limit, MAX_ANNOTATION_SIZE, Z_STRLEN_P(value));

        std::string tvalue(Z_STRVAL_P(value), tlen);

        if (limit > 0 && (Z_STRLEN_P(value) > limit || Z_STRLEN_P(value) > MAX_ANNOTATION_SIZE ))
        {
            tvalue += "...";
        }
        return tvalue;
    }
    case IS_ARRAY:
    {
        std::string tvalue ;
        KV map;
        zval_to_Map(map,value,MAX_ARRAY_ELEMENT,MAX_ANNOTATION_SIZE);
        map_to_str(map,tvalue);
        return tvalue;
    }
    case IS_OBJECT:
    {
        std::string tvalue = "object";
#if PHP_VERSION_ID < 70000
        int32_t tlen =0;
        char * tstr = NULL;
        if (Z_OBJ_HANDLER(*value, get_class_name))
        {
#if PHP_VERSION_ID < 50400
            Z_OBJ_HANDLER(*value, get_class_name)(value, (char **) &tstr,
                    (zend_uint *) &tlen, 0 TSRMLS_CC);
#else
            Z_OBJ_HANDLER(*value, get_class_name)(value, (const char **) &tstr,
                          (zend_uint *) &tlen, 0 TSRMLS_CC);
#endif
            tvalue = tvalue +"(" + tstr + ")";
            efree(tstr);
        }
#else
       zend_string* class_name = Z_OBJ_HANDLER_P(value, get_class_name)(Z_OBJ_P(value));
        tvalue+=P7_STR(class_name);
        zend_string_release(class_name);
#endif
        return tvalue;
    }
    case IS_RESOURCE:
    {
        std::string tvalue = "resource";
        char* tstr = NULL;
#if PHP_VERSION_ID < 70000
        tstr = (char *) zend_rsrc_list_get_rsrc_type(Z_LVAL_P(value) TSRMLS_CC);
        return tvalue + "(" + (tstr ? tstr : "Unknown") + ")";
    }
#else
        tstr = (char *) zend_rsrc_list_get_rsrc_type(Z_RES_P(value) TSRMLS_CC);

        return tvalue + "(" + (tstr ? tstr : "Unknown") +")";
    }
        case IS_UNDEF:
        return "{undef}";
#endif

#if PHP_VERSION_ID >= 70000
        case IS_REFERENCE:
        value = Z_REFVAL_P(value);
        goto again;
        break;
#endif
    default:
        return "{unknown}";
    }

}

const Pinpoint::Plugin::ExceptionInfo get_exception_info(EG_EXP_TPYE exception)
{
    zval *message, *file, *line;
#if PHP_VERSION_ID >= 70000
    zval rv;
#endif
    zend_class_entry* default_ce;

    Pinpoint::Plugin::ExceptionInfo exceptionInfo;

    if (!exception)
    {
        assert (false);
        return exceptionInfo;
    }

    default_ce = zend_exception_get_default(TSRMLS_C);

#if PHP_VERSION_ID >= 70000
    zval zv ;
    ZVAL_OBJ(&zv, exception);
    message = zend_read_property(zend_get_exception_base(&zv),&zv,ZEND_STRL( "message"), 0, &rv);
    file   = zend_read_property(zend_get_exception_base(&zv), &zv, ZEND_STRL("file"), 0, &rv);
    line   = zend_read_property(zend_get_exception_base(&zv), &zv, ZEND_STRL("line"), 0, &rv);
#else
    message = zend_read_property(default_ce, exception, "message", sizeof("message")-1, 0 TSRMLS_CC);
    file = zend_read_property(default_ce, exception, "file", sizeof("file")-1, 0 TSRMLS_CC);
    line = zend_read_property(default_ce, exception, "line", sizeof("line")-1, 0 TSRMLS_CC);
#endif

    exceptionInfo.message = Z_TYPE_P(message) == IS_STRING ?
                            std::string(Z_STRVAL_P(message), Z_STRVAL_P(message) + Z_STRLEN_P(message)) : "";
    exceptionInfo.file = Z_TYPE_P(file) == IS_STRING ?
                         std::string(Z_STRVAL_P(file), Z_STRVAL_P(file) + Z_STRLEN_P(file)) : "";
    exceptionInfo.line = Z_TYPE_P(line) == IS_LONG ? (int32_t)Z_LVAL_P(line) : -1;

    return exceptionInfo;
}


PObjectCache::PObjectCache()
{
    buf = NULL;
    bufLen = 0;
    classDefinitionPath = NULL;
    isCacheSuccess = false;
    obj = NULL;
    requestNum = PhpRequestCounter::INVALID_COUNT;
    zval_class_instance= NULL;

}

PObjectCache::~PObjectCache()
{
    if (buf != NULL)
    {
        free(buf);
        buf = NULL;
    }

    if (classDefinitionPath != NULL)
    {
        free(classDefinitionPath);
        classDefinitionPath = NULL;
    }
}


int32_t PObjectCache::store(zval *z_class_instance, const char* classDefinitionPath)
{
    TSRMLS_FETCH();

    zval retval;

    if (classDefinitionPath != NULL)
    {
        this->classDefinitionPath = strdup(classDefinitionPath);
    }

    this->type = Z_TYPE_P(z_class_instance);


    if (Z_TYPE_P(z_class_instance) == IS_OBJECT)
    {
        zend_class_entry* ce = zend_get_class_entry(z_class_instance);
        PINPOINT_ASSERT_RETURN ((ce != NULL && ce->name != NULL), Pinpoint::FAILED);
        this->className = std::string(_PSTR(ce->name), _PSTR_LEN(ce->name));
    }

    if(call_php_kernel_serialize(&retval,z_class_instance) != 0)
    {
        LOGE("call_php_kernel_serialize failed %s",classDefinitionPath);
        return Pinpoint::FAILED;
    }

    this->isCacheSuccess = true;

    if(this->bufLen < Z_STRLEN_P(&retval) )
    {
        if(buf)
        {
            free(buf);
            buf = NULL;
        }
        this->bufLen = (size_t)Z_STRLEN_P(&retval);
        buf = ( unsigned char *)malloc(bufLen + 1);
        if (buf == NULL)
        {
            return Pinpoint::FAILED;
        }
    }
    memcpy(buf, Z_STRVAL_P(&retval), bufLen);
    buf[bufLen] = '\0';                       // compatible for php <7

    zval_dtor(&retval);

    return SUCCESS;

}

int32_t PObjectCache::load()
{
    TSRMLS_FETCH();

    PINPOINT_ASSERT_RETURN((bufLen > 0), Pinpoint::FAILED);

    if (this->type == IS_OBJECT && this->classDefinitionPath != NULL)
    {
        STR_TYPE* classname = w_zend_string_init(this->className.c_str(),
                                                 this->className.length(),
                                                 0);

        zend_class_entry* ce = w_zend_fetch_class(classname);
        if (ce == NULL)
        {
            zend_file_handle *prepend_file_p;
            zend_file_handle prepend_file;
            memset(&prepend_file, 0, sizeof(zend_file_handle));

            prepend_file.filename = this->classDefinitionPath;
            prepend_file.opened_path = NULL;
            prepend_file.free_filename = 0;
            prepend_file.type = ZEND_HANDLE_FILENAME;
            prepend_file_p = &prepend_file;

            zend_try
            {
                if (zend_execute_scripts(ZEND_INCLUDE_ONCE TSRMLS_CC, NULL, 1, prepend_file_p) != 0)
                {
                    zend_string_release(classname);

                    LOGE("include %s failed.", this->classDefinitionPath);
                    return Pinpoint::FAILED;
                }
            }
            zend_catch
            {
                zend_string_release(classname);

                LOGE("include %s failed.", this->classDefinitionPath);
                return Pinpoint::FAILED;
            }zend_end_try();
        }
        zend_string_release(classname);
    }

    zval* obj_seried;
    MAKE_STD_ZVAL(obj_seried);
    W_ZVAL_STRINGL(obj_seried, (char*)buf, bufLen);

    if(zval_class_instance == NULL)
    {
        MAKE_STD_ZVAL( zval_class_instance);

        if (call_php_kernel_unserialize(zval_class_instance,obj_seried) != SUCCESS)
        {
            assert (false);
            return Pinpoint::FAILED;
        }

        if(Z_TYPE_P(zval_class_instance) != IS_OBJECT)
        {
            assert(0);
            return Pinpoint::FAILED;
        }
    }

    w_zval_ptr_dtor(obj_seried);

    this->requestNum = PhpRequestCounter::getCount();

    PhpAop *aop = PhpAop::getInstance();
    PINPOINT_ASSERT_RETURN((aop != NULL), Pinpoint::FAILED);

    LOGD("%s register reqShutDownHandler", className.c_str());
    aop->addReqShutdownHandler(boost::bind(&PObjectCache::reqShutDownHandler, this));

    LOGD(" load once ");

    return SUCCESS;
}


void PObjectCache::reqShutDownHandler()
{
    if(zval_class_instance && classDefinitionPath)
    {
        store(zval_class_instance, classDefinitionPath);
        w_zval_ptr_dtor(zval_class_instance);
        zval_class_instance = NULL;
    }

    LOGD(" %s:%s reqShutDownHandler called ", classDefinitionPath, className.c_str());
}

#define RESTORE_EXCEPTION() \
        if (EG(exception)) {\
            zend_exception_set_previous(EG(exception), old_exception TSRMLS_CC);\
            LOGE("w_zend_call_method throw: [%s]", get_exception_info(EG(exception)).toString().c_str());\
            /* agent  care what happens in  plugins*/ \
            return Pinpoint::FAILED;\
        }else {\
            EG(exception) = old_exception;\
        }

int32_t PObjectCache::callMethod(const char* methodName, int32_t methodNameLen, zval* returnValue,
                                 int32_t parameterCount, zval* arg1, zval* arg2)
{
    TSRMLS_FETCH();

    EG_EXP_TPYE old_exception = NULL;


    if(EG(exception))
    {
        old_exception = EG(exception);
        EG(exception) = NULL;
    }

    if (!isCacheSuccess)
    {
        LOGE("isCacheSuccess == false!");
        goto FAILED;
    }

    if (this->requestNum == PhpRequestCounter::INVALID_COUNT
        || this->requestNum != PhpRequestCounter::getCount() || zval_class_instance == NULL)
    {
        //load could throw exception
        int32_t err = this->load();
        if (err != SUCCESS)
        {
            LOGE("objectCache.load() failed.")
            goto FAILED;
        }
    }

    w_zend_call_method(zval_class_instance, zend_get_class_entry(zval_class_instance),
                       NULL, methodName, methodNameLen, returnValue, parameterCount, arg1, arg2);



    RESTORE_EXCEPTION();

    return  SUCCESS;

FAILED:

    return  Pinpoint::FAILED;

}


int32_t PObjectCache::callMethod(const char* methodName, int32_t methodNameLen,
                                 int32_t parameterCount, zval* arg1, zval* arg2)
{
    TSRMLS_FETCH();

    EG_EXP_TPYE old_exception = NULL;


    if(EG(exception)){
        old_exception = EG(exception);
        EG(exception) = NULL;
    }

    if (!isCacheSuccess)
    {
        LOGE("isCacheSuccess == false!");
        goto FAILED;
    }

    if (this->requestNum == PhpRequestCounter::INVALID_COUNT
        || this->requestNum != PhpRequestCounter::getCount() || zval_class_instance == NULL)
    {
        //load could throw exception
        int32_t err = this->load();
        if (err != SUCCESS)
        {
            LOGE("objectCache.load() failed.")
            goto FAILED;
        }
    }

    w_zend_call_method(zval_class_instance, zend_get_class_entry(zval_class_instance),
                       NULL, methodName, methodNameLen, NULL, parameterCount, arg1, arg2);

    RESTORE_EXCEPTION();

    return SUCCESS;

FAILED:

    return  Pinpoint::FAILED;

}



boost::thread_specific_ptr<int64_t> PhpRequestCounter::pCounter(NULL);

int64_t* PhpRequestCounter::getPCounter()
{
    if (PhpRequestCounter::pCounter.get() == NULL)
    {
        try
        {
            int64_t* p = new int64_t(0);
            PhpRequestCounter::pCounter.reset(p);
        }
        catch (std::bad_alloc&)
        {
            LOGE("new int64_t(PhpRequestCounter::INVALID_COUNT) failed!");
        }
    }

    return PhpRequestCounter::pCounter.get();
}

int64_t PhpRequestCounter::getCount()
{
    int64_t *p = getPCounter();
    if (p == NULL)
    {
        return PhpRequestCounter::INVALID_COUNT;
    }

    return *p;
}

void PhpRequestCounter::increment()
{
    int64_t *p = getPCounter();
    if (p != NULL)
    {
        *p += 1;
    }
}
