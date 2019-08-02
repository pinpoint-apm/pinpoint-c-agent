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
#include "ext/standard/info.h"

#include "php_pinpoint_php.h"

#include <iostream>
#include <string>
#include <json/json.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stack>
#include "pinpoint_helper.h"

#if HAVE_EXT_TEST
#include <string>
static bool test_main();
#endif

PHP_FUNCTION(pinpoint_start_trace);
PHP_FUNCTION(pinpoint_end_trace);
PHP_FUNCTION(pinpoint_add_clue);
PHP_FUNCTION(pinpoint_add_clues);
PHP_FUNCTION(pinpoint_unique_id);
PHP_FUNCTION(pinpoint_tracelimit);
PHP_FUNCTION(pinpoint_drop_trace);
//
PHP_FUNCTION(pinpoint_app_name);
PHP_FUNCTION(pinpoint_app_id);
PHP_FUNCTION(pinpoint_start_time);


PHP_FUNCTION(pinpoint_test_main);

ZEND_DECLARE_MODULE_GLOBALS(pinpoint_php)

typedef std::stack<TraceNode> Stack;

static int send_msg_to_collector(TransLayer *t_layer);
static int recv_msg_from_collector(TransLayer *t_layer);
static int pp_trace(const char *format,...);
static int connect_unix_remote(const char* remote);
static int connect_remote(TransLayer* t_layer,const char* statement);
static int recv_msg_from_collector(TransLayer *t_layer);
static int handle_msg_from_collector(const char* buf,size_t len);
static void handle_agent_info(int type,const char* buf,size_t len);

static void reset_remote(TransLayer* t_layer);
static size_t trans_layer_pool(TransLayer* t_layer);

static void asy_send_msg_to_agent(TransLayer* t_layer,const std::string &data);
static bool init_shared_obj();
static bool register_shared_obj_address();
static uint64_t generate_unique_id();
static bool check_tracelimit(int64_t timestamp);
static uint64_t get_current_msec_stamp();

/* {{{ PHP_INI
 */

PHP_INI_BEGIN()

STD_PHP_INI_ENTRY("pinpoint_php.CollectorHost", "unix:/tmp/collector.sock", PHP_INI_ALL,
        OnUpdateString,co_host,zend_pinpoint_php_globals,pinpoint_php_globals)

STD_PHP_INI_ENTRY("pinpoint_php.SendSpanTimeOutMs", "0", PHP_INI_ALL,
        OnUpdateLong,w_timeout_ms,zend_pinpoint_php_globals,pinpoint_php_globals)

STD_PHP_INI_ENTRY("pinpoint_php.UnitTest", "no", PHP_INI_ALL,
        OnUpdateBool,utest_flag,zend_pinpoint_php_globals,pinpoint_php_globals)

STD_PHP_INI_ENTRY("pinpoint_php.AppId", "collector_blocking", PHP_INI_ALL,
        OnUpdateString,agent_info.appid,zend_pinpoint_php_globals,pinpoint_php_globals)

STD_PHP_INI_ENTRY("pinpoint_php.AppName", "collector_blocking", PHP_INI_ALL,
        OnUpdateString,agent_info.appname,zend_pinpoint_php_globals,pinpoint_php_globals)

STD_PHP_INI_ENTRY("pinpoint_php.StartTime", "0", PHP_INI_ALL,
        OnUpdateLong,agent_info.start_time,zend_pinpoint_php_globals,pinpoint_php_globals)

STD_PHP_INI_ENTRY("pinpoint_php.TraceLimit","-1",PHP_INI_ALL,
        OnUpdateLong,tracelimit,zend_pinpoint_php_globals,pinpoint_php_globals)
STD_PHP_INI_ENTRY("pinpoint_php._limit","no",PHP_INI_ALL,
        OnUpdateBool,limit,zend_pinpoint_php_globals,pinpoint_php_globals)

PHP_INI_END()

/* }}} */


ZEND_BEGIN_ARG_INFO_EX(arginfo_add_key_value, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_add_int, 0, 0, 2)
    ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

/* {{{ pinpioint_php_functions[]
 *
 * Every user visible function must have an entry in pinpioint_php_functions[].
 */
const zend_function_entry pinpoint_php_functions[] = {
        PHP_FE(pinpoint_start_trace,NULL)
        PHP_FE(pinpoint_end_trace,NULL)
        PHP_FE(pinpoint_unique_id,NULL)
        PHP_FE(pinpoint_test_main,NULL)
        PHP_FE(pinpoint_app_name,NULL)
        PHP_FE(pinpoint_app_id,NULL)
        PHP_FE(pinpoint_drop_trace,NULL)
        PHP_FE(pinpoint_start_time,NULL)
        PHP_FE(pinpoint_tracelimit,arginfo_add_int)
        PHP_FE(pinpoint_add_clue,arginfo_add_key_value)
        PHP_FE(pinpoint_add_clues,arginfo_add_key_value)
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

#ifdef COMPILE_DL_PINPOINT_PHP
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(pinpoint_php)
#endif

#define UNIX_SOCKET "unix:"
#define TCP_SOCKET "tcp:"
#define CLUES "clues"

void (*old_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);

#define safe_free(x)\
    if((x)){ \
        free((x));\
        (x) = NULL;\
    }

PHP_FUNCTION(pinpoint_drop_trace)
{
    PPG(limit)= 1;
    RETURN_TRUE;
}

PHP_FUNCTION(pinpoint_app_name)
{
    RETURN_STRING(PPG(agent_info).appname);
}

PHP_FUNCTION(pinpoint_app_id)
{
    RETURN_STRING(PPG(agent_info).appid);
}
PHP_FUNCTION(pinpoint_start_time)
{
    RETURN_LONG(PPG(agent_info).start_time);
}

PHP_FUNCTION(pinpoint_start_trace)
{
    pp_trace("pinpoint_start_trace ");

    uint64_t timestamp =  get_current_msec_stamp();
    Stack * callstack = (Stack*)PPG(call_stack);


    if(!callstack->empty()) // child
    {
        TraceNode& parent = callstack->top();
        uint node_length = parent.node["calls"].size();
        TraceNode child(parent.node["calls"][node_length]);
        child.node["S"] = timestamp - parent.ancestor_start_time;
        child.ancestor_start_time = parent.ancestor_start_time;
        child.start_time = timestamp;
        callstack->push(child);
        PPG(fetal_error_time) = 0; // reset fetal_error_time
    }else{ // ancestor

        Json::Value* root = (Json::Value*)PPG(root);
        if(PPG(t_layer).c_fd == -1)
        {
            pp_trace("agent try to connect:(%s)",PPG(co_host));
            connect_remote(&PPG(t_layer),PPG(co_host));
        }
        TraceNode ancestor(*root);
        ancestor.node["S"] = timestamp;
        ancestor.ancestor_start_time = timestamp;
        ancestor.start_time = timestamp;
        callstack->push(ancestor);
    }
    RETURN_LONG(callstack->size());
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

    Json::Value* root = (Json::Value*)PPG(root);
    if(root){
        Json::Value eMsg ;
        eMsg["msg"] = msg;
        eMsg["file"] = error_filename;
        eMsg["line"] = error_lineno;
        (*root)["ERR"] = eMsg;
    }
    pp_trace("apm_error_cb called");
    PPG(fetal_error_time)= get_current_msec_stamp();
    efree(msg);

    /// call origin cb
    old_error_cb(type, error_filename, error_lineno, format, args);
}

PHP_FUNCTION(pinpoint_end_trace)
{
    uint64_t timestamp =  get_current_msec_stamp();

    Stack * callstack = (Stack*)PPG(call_stack);

    if( callstack->size() == 1 ) // ancestor node
    {
        TraceNode& ancestor =  callstack->top();
        if(PPG(limit)== 0)
        {
            ancestor.node["E"] = PPG(fetal_error_time) != 0? ( PPG(fetal_error_time) - ancestor.start_time ): timestamp - ancestor.start_time;

            Json::FastWriter* writer = (Json::FastWriter*)PPG(writer);
            std::string trace = writer->write(ancestor.node);
            Header header;
            header.length  = htonl(trace.size());
            header.type    = htonl(REQ_UPDATE_SPAN);

            std::string buf((char*)&header,sizeof(header));
            buf += trace;
            asy_send_msg_to_agent(&PPG(t_layer),buf);
            pp_trace("this span:(%s)",trace.c_str());
        }
        ancestor.node.clear();
        callstack->pop();
    }
    else if(callstack->size() > 1){ // descendants
         TraceNode& descendant =callstack->top();
         descendant.node["E"] =  PPG(fetal_error_time) != 0? ( PPG(fetal_error_time) - descendant.start_time ): timestamp - descendant.start_time;
         callstack->pop();
    }
    else
    {
        fprintf(stderr," trace stack overflow");
    }

    pp_trace("pinpoint_end_trace Done!");
    RETURN_LONG(callstack->size());
}

PHP_FUNCTION(pinpoint_add_clue)
{
       std::string key;
       std::string value;

   #if PHP_VERSION_ID < 70000
       char* zkey = NULL,* zvalue =  NULL;
       int zkey_len,value_len,;
       if (!Pinpoint::Trace::Trace::isStarted())
       {
           RETURN_NULL();
       }

       if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &zkey, &zkey_len,&zvalue, &zvalue_len) == FAILURE)
       {
           zend_error(E_ERROR, "pinpoint_add_clue() expects (int, string).");
           return;
       }
       key = std::string(zkey,zkey_len);
       value = std::string(zvalue,zkey_len);

   #else
       zend_string* zkey;
       zend_string* zvalue;
       if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "SS", &zkey ,&zvalue) == FAILURE)
       {
          zend_error(E_ERROR, "pinpoint_add_clue() expects (int, string).");
          return;
       }
       key = std::string(zkey->val,zkey->len);
       value = std::string(zvalue->val,zvalue->len);
   #endif

       Stack * callstack = (Stack*)PPG(call_stack);
       if(callstack->size()> 0){
           Json::Value& curNode = callstack->top().node;
           curNode[key] = value;
       }else{
           pp_trace("%d: stack is empty,error found.",__LINE__);
       }
       
       pp_trace("%s  -> %s ",key.c_str(),value.c_str());
}

PHP_FUNCTION(pinpoint_test_main)
{
#if HAVE_EXT_TEST
    if(test_main()){
        RETURN_TRUE
    }else{
        RETURN_FALSE
    }
#else
    RETURN_FALSE
#endif

}

PHP_FUNCTION(pinpoint_unique_id)
{
    RETURN_LONG(generate_unique_id());
}


PHP_FUNCTION(pinpoint_add_clues)
{
       std::string key;
       std::string value;

   #if PHP_VERSION_ID < 70000
       char* zkey = NULL,* zvalue =  NULL;
       int zkey_len,value_len,;
       if (!Pinpoint::Trace::Trace::isStarted())
       {
           RETURN_NULL();
       }

       if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &zkey, &zkey_len,&zvalue, &zvalue_len) == FAILURE)
       {
           zend_error(E_ERROR, "pinpoint_add_clues() expects (int, string).");
           return;
       }
       key = std::string(zkey,zkey_len);
       value = std::string(zvalue,zkey_len);
   #else
       zend_string* zkey;
       zend_string* zvalue;
       if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "SS", &zkey ,&zvalue) == FAILURE)
       {
          zend_error(E_ERROR, "pinpoint_add_clues() expects (int, string).");
          return;
       }
       key = std::string(zkey->val,zkey->len);
       value = std::string(zvalue->val,zvalue->len);
   #endif

       Stack * callstack = (Stack*)PPG(call_stack);
       if(callstack->size() > 0)
       {
            Json::Value& curNode = callstack->top().node;
            curNode[CLUES].append(key+":"+value);
            pp_trace("%s  -> %s ",key.c_str(),value.c_str());
       }else{
            pp_trace("%d: stack is empty,error found.",__LINE__);
       }
}

PHP_FUNCTION(pinpoint_tracelimit)
{
    int64_t timestamp = -1;
    zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &timestamp);
    timestamp = (timestamp == -1)?(time(NULL)):(timestamp);
    if(check_tracelimit(timestamp))
    {
        PPG(limit) = 1;
        pp_trace("this span should be dropped by trace limit:%d",PPG(tracelimit));
        RETURN_TRUE
    }else{
        PPG(limit) = 0;
        RETURN_FALSE
    }
}

/* {{{ php_pinpioint_php_init_globals
 */
/* Uncomment this function if you have INI entries
 */
static void php_pinpoint_php_init_globals(zend_pinpoint_php_globals *pinpoint_php_globals)
{
    pinpoint_php_globals->t_layer.c_fd = -1;
    // this address should not expose to anyone
    strcpy(pinpoint_php_globals->shared_obj.address,SOBJ_ADDRESS);
    pinpoint_php_globals->t_layer.chunks = new Chunks(1024*1024,1024);
    pinpoint_php_globals->call_stack     = new std::stack<TraceNode>();
    Json::FastWriter * writer  = new Json::FastWriter();
    writer->dropNullPlaceholders();
    writer->omitEndingLineFeed();
    pinpoint_php_globals->writer = (void*)writer;
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

    PPG(agent_info).start_time = get_current_msec_stamp();

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

#if 0
    no needs to call when module shutdown
    if(PPG(root)){
        delete (Json::Value *)PPG(root);
    }

    reset_remote();
#endif

    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(pinpoint_php)
{

#if defined(COMPILE_DL_PINPIOINT_PHP) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    if(PPG(root) == NULL){
        PPG(root) = new Json::Value();
    }

    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(pinpoint_php)
{
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



int do_write_data(const void *data,uint length,void *args)
{
    TransLayer* t_layer = (TransLayer*)args;
    const char* buf = (const char*)data;
    uint buf_ofs = 0;

    while(buf_ofs < length){
        ssize_t ret = send(t_layer->c_fd,buf + buf_ofs,length -buf_ofs ,MSG_NOSIGNAL);
        if(ret > 0){
            buf_ofs += (uint) ret;
        }else if(ret == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK || errno== EINTR){
                return buf_ofs;
            }
            pp_trace("%d send data error:(%s) fd:(%d)",__LINE__,strerror(errno),t_layer->c_fd);
            return -1;
        }
        else{
            pp_trace("%d send data return 0 error:(%s) fd:(%d)",__LINE__,strerror(errno),t_layer->c_fd);
            return -1;
        }
    }
    return length;
}

int send_msg_to_collector(TransLayer *t_layer)
{
    return t_layer->chunks->drainOutWithPipe(do_write_data,(void*)t_layer);
}

void handle_agent_info(int type,const char* buf,size_t len)
{
    std::string  msg(buf,len);
    Json::Value  root;
    Json::Reader reader;
    int ret = reader.parse(msg.c_str(),root);
    if(!ret)
    {
        return ;
    }

    if(root.isMember("time")){
        PPG(agent_info).start_time = atoll(root["time"].asCString());
    }

    if(root.isMember("id")){
        PPG(agent_info).appid      =  strdup(root["id"].asCString());
    }

    if(root.isMember("name")){
        PPG(agent_info).appname    =  strdup(root["name"].asCString());
    }

// disabled.  Shared is not a good idea
//    if(root.isMember("shared_mem_file"))
//    {
//        strncpy(PPG(shared_obj).address,root["shared_mem_file"].asCString(),NAMING_SIZE);
//        /// Get shared region
//        init_shared_obj();
//    }

    pp_trace("starttime:%ld appid:%s mem_file:%s",PPG(agent_info).start_time,PPG(agent_info).appid,PPG(shared_obj).address);
}

int handle_msg_from_collector(const char* buf,size_t len)
{

    size_t offset = 0;

    while( offset + 8 <= len ){

        Header * header= (Header*)buf;

        uint body_len = ntohl(header->length);

        if( 8+ body_len > len ){
            return offset ;
        }

        uint type = ntohl(header->type);
        switch(type){
        case RESPONSE_AGENT_INFO:
            handle_agent_info(RESPONSE_AGENT_INFO, buf+8,len - 8);
            break;
        default:
            pp_trace("unsupport type:%d",type);
        }

        offset += (8 +body_len );

    }

    return offset;
}

int recv_msg_from_collector(TransLayer *t_layer)
{
    int next_size = 0;
    while(next_size < IN_MSG_BUF_SIZE){
        int ret = recv(t_layer->c_fd,t_layer->in_buf + next_size,IN_MSG_BUF_SIZE -next_size ,0);
        if (ret > 0){
            int total = ret + next_size;
            int msg_offset = handle_msg_from_collector(t_layer->in_buf,total);
            if(msg_offset < total){
                next_size = total - msg_offset;
                memcpy(t_layer->in_buf,t_layer->in_buf + msg_offset ,next_size);
            }else{
                next_size = 0 ;
            }
        }else if(ret == 0){
            // peer close
            return -1;
        }else{
            if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR){
                return 0;
            }
            pp_trace("recv with error:%s",strerror(errno));
            return -1;
        }
    }
    return 0;
}

int pp_trace(const char *format,...)
{
    // insert more info
    int n = snprintf(&PPG(logBuffer)[0],LOG_SIZE,"[%d] ",getpid());
    //
    va_list ap;
    va_start(ap, format);
    vsnprintf(&PPG(logBuffer)[n], LOG_SIZE -n ,format, ap);
    va_end(ap);

    if (PPG(utest_flag) == 1){
        fprintf(stderr,"%s\n",PPG(logBuffer));
        return 0;
    }

    php_log_err_with_severity(&PPG(logBuffer)[0] , LOG_DEBUG);

    return 0;
}


int connect_unix_remote(const char* remote)
{
    int fd = -1,len = -1;
    struct sockaddr_un u_sock = {0};
    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        pp_trace("get socket error,(%s)",strerror(errno));
        goto ERROR;
    }

    u_sock.sun_family = AF_UNIX;
    sprintf(u_sock.sun_path, "agent:%d", getpid());
    len = offsetof(struct sockaddr_un, sun_path) + strlen(u_sock.sun_path);

    memset(&u_sock, 0, sizeof(u_sock));
    u_sock.sun_family = AF_UNIX;
    strncpy(u_sock.sun_path,remote,sizeof(u_sock.sun_path) -1);
    len =  offsetof(struct sockaddr_un, sun_path) + strlen(u_sock.sun_path);

    // mark fd as non blocking
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);

    if(connect(fd,(struct sockaddr *)&u_sock, len) != 0)
    {
        if( errno != EALREADY || errno !=  EINPROGRESS )
        {
            pp_trace("connect:(%s) failed as (%s)",remote,strerror(errno));
            goto ERROR;
        }
    }
    pp_trace("connect to %s",remote);

    return fd;

ERROR:
    if(fd > 0)
    {
       close(fd);
    }

    return -1;
}

int connect_remote(TransLayer* t_layer,const char* statement)
{
    int fd = -1;
    const char* substring = NULL;
    if(statement == NULL || statement[0] == '\0')
    {
       goto DONE;
    }

    /// unix
    substring = strcasestr(statement,UNIX_SOCKET);
    if( substring == statement )
    {
        // sizeof = len +1, so substring -> /tmp/collector.sock
        substring = substring + sizeof(UNIX_SOCKET) - 1;
        fd = connect_unix_remote(substring);
        t_layer->c_fd = fd;
        t_layer->socket_read_cb  = recv_msg_from_collector;
        t_layer->socket_write_cb = send_msg_to_collector;
        goto DONE;
    }

    ///  tcp

    ///  udp

DONE:
    /// Add  whoamI info

    return fd;
}


int insert_into_chunks(Chunks* chunks,const std::string &buffer)
{
    return chunks->copyDataIntoChunks(buffer.data(),buffer.size());
}

void reset_remote(TransLayer* t_layer)
{
    if(t_layer->c_fd > 0)
    {
        close(t_layer->c_fd);
        t_layer->c_fd = -1;
    }
    t_layer->chunks->resetChunks();

}


size_t trans_layer_pool(TransLayer* t_layer)
{
    if(t_layer->c_fd  == -1 )
    {
        connect_remote(t_layer,PPG(co_host));
        if(t_layer->c_fd == -1)
        {
            return -1;
        }
    }
    int fd = t_layer->c_fd;
    fd_set wfds,efds,rfds;
    FD_ZERO(&wfds);
    FD_ZERO(&efds);
    FD_ZERO(&rfds);
    FD_SET(fd,&wfds);
    FD_SET(fd,&efds);
    FD_SET(fd,&rfds);

    struct timeval tv = {0,PPG(w_timeout_ms)*1000};

    int retval = select(fd+1,&rfds,&wfds,&efds,&tv);
    if(retval == -1)
    {
        //pp_trace("select return error:(%s)",strerror(errno));
        return -1;
    }else if(retval >0 ){

        if(FD_ISSET(fd,&efds)){
            pp_trace("select fd:(%s) ",strerror(errno));
            goto ERROR;
        }

        if(FD_ISSET(fd,&wfds)){
            if(t_layer->socket_write_cb){
               if( t_layer->socket_write_cb(t_layer) == -1){
                   goto ERROR;
               }
            }
        }

        if(FD_ISSET(fd,&rfds)){
             if(t_layer->socket_read_cb){
                if( t_layer->socket_read_cb(t_layer) == -1){
                    goto ERROR;
                }
             }
        }

    }
    else{
        // timeout do nothing
        // total =0  ,timeout
    }

    return 0;

ERROR:

    reset_remote(t_layer);
    connect_remote(t_layer,PPG(co_host));

    return -1;
}


void asy_send_msg_to_agent(TransLayer* t_layer,const std::string &data)
{
    // append to end

    if(insert_into_chunks(t_layer->chunks,data) != 0 )
    {
        pp_trace("Send buffer is full. size:[%d]",data.length());
        return ;
    }

    // try send
    int total = trans_layer_pool(t_layer);
    if(total <= 0){
        return;
    }

}

uint64_t generate_unique_id()
{
    if(PPG(shared_obj).region == NULL)
    {
        if(register_shared_obj_address() && init_shared_obj())
        {
        }else{
            return 0L;
        }
    }

    uint64_t* value =  (uint64_t*)((char*)PPG(shared_obj).region + UNIQUE_ID_OFFSET);
    __sync_fetch_and_add(value,1);
    return *value;
}

uint64_t get_current_msec_stamp()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*1000 + tv.tv_usec /1000;
}

/**
 *                          sysconf(_SC_PAGESIZE)
 *
 * ***********************************************************************************************************
 * |unique_id(int64_t)|tracelimit(unix time,triger)|
 *    8                              8+ 8
 * ***********************************************************************************************************
 */

bool init_shared_obj()
{
    if(PPG(shared_obj).region != NULL || PPG(shared_obj).address[0] == 0 )
    {
        return true;
    }

    int fd = open(PPG(shared_obj).address,O_RDWR);
    if(fd == -1)
    {
        pp_trace("attach file:[%s] with:[%s]",PPG(shared_obj).address,strerror(errno));
        return false;
    }

    struct stat _stat;
    size_t length;
    fstat(fd,&_stat);
    length = _stat.st_size;

    void* addr = mmap(NULL,length,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(addr == (void*)-1)
    {
        close(fd);
        pp_trace("mmap file:[%s] with:[%s]",PPG(shared_obj).address,strerror(errno));
        return false;
    }

    PPG(shared_obj).region = addr;
    PPG(shared_obj).length = length;
    close(fd);
    return true;
}

bool register_shared_obj_address()
{
    if(PPG(shared_obj).address[0]=='0')
    {
        fprintf(stderr,"%s: fetal error address is null \n",__FUNCTION__);
        return false;
    }
    mode_t mode = S_IRUSR |S_IWUSR|S_IRGRP|S_IWGRP;
    int fd = open(PPG(shared_obj).address,O_WRONLY | O_CREAT,mode);
    if( fd == -1)
    {
        fprintf(stderr,"%s: open address with:%s \n",__FUNCTION__,strerror(errno));
        return false;
    }

    struct stat _stat;
    size_t length;
    fstat(fd,&_stat);
    length = _stat.st_size;
    if(length == 0)
    {
        int64_t pagesize = sysconf(_SC_PAGESIZE);
        ftruncate(fd,pagesize);
        pp_trace("pagesize =%ld ",pagesize);
    }
    close(fd);
    return true;
}

bool check_tracelimit(int64_t timestamp)
{
   if(PPG(shared_obj).region == NULL)
   {
       if(register_shared_obj_address() && init_shared_obj())
       {
       }else{
           return false;
       }
   }

   time_t ts = (timestamp == -1) ?(timestamp) :(time(NULL));
   int64_t* c_timestamp =  (int64_t*)((char*)PPG(shared_obj).region + TRACE_LIMIT);
   int64_t* triger    =  (int64_t*)((char*)PPG(shared_obj).region + TRACE_LIMIT + 8);
   if(PPG(tracelimit) < 0)
   {
   }else if(PPG(tracelimit) == 0){
       return true;
   }
   else if(*c_timestamp != ts )
   {
       *c_timestamp = ts;
       *triger    = 0 ;
   }
   else if(*triger >= PPG(tracelimit))
   {
       return true;
   }else
   {
       __sync_add_and_fetch(triger,1);
       pp_trace("triger:%ld",*triger);
   }

   return false;
}


#if HAVE_EXT_TEST

int checkData(const void* buf,uint length,void* dst)
{
    int ret = 0;
    if(length < 25)
    {
        ret =  length;
    }else{
        ret = length /2;
    }
    std::string * str = (std::string*)dst;
    str->append((const char*)buf,ret);

    return ret;
}

bool test_chunks()
{
    Chunks chunks(1024*4,128);
    std::string out,ret;
//    int size = 0;
    // user random data
    char buf1[1024];
    char buf2[1024];
    char buf3[1024];

    chunks.copyDataIntoChunks(buf1,1024);
    chunks.copyDataIntoChunks(buf2,1024);
    chunks.copyDataIntoChunks(buf3,1024);
    out.append(buf1,1024);
    out.append(buf2,1024);
    out.append(buf3,1024);

    // in and out
    chunks.drainOutWithPipe(checkData,&ret);

    assert(ret == out);

    // copy until full
    chunks.copyDataIntoChunks(buf1,1024);
    chunks.copyDataIntoChunks(buf2,1024);
    chunks.copyDataIntoChunks(buf3,1024);
    chunks.copyDataIntoChunks(buf3,1024);
    assert(chunks.getAllocSize() >= 1024*4);
    chunks.copyDataIntoChunks(buf3,1024);
    assert(chunks.copyDataIntoChunks(buf3,1024) == 1024 );

    out.clear();
    out.append(buf1,1024);
    out.append(buf2,1024);
    out.append(buf3,1024);
    out.append(buf3,1024);

    ret.clear();
    chunks.drainOutWithPipe(checkData,&ret);

    assert(chunks.getAllocSize() == 0);
    assert(ret == out);

    return true;
}

bool test_list()
{
    Node header={0};
    init_list(&header);

    int v1,v2,v3,v4;

    Node* n1 = push_back(&header);
    Node* n2 = push_back(&header);
    Node* n3 = push_back(&header);
    Node* n4 = push_back(&header);

    n1->value = &v1;
    n2->value = &v2;
    n3->value = &v3;
    n4->value = &v4;

    if( *(int*)(pop_back(&header)) != v4     ||
            *(int*)(pop_back(&header)) != v3 ||
            *(int*)(pop_back(&header)) != v2 ||
            *(int*)(pop_back(&header)) != v1)
    {
        pp_trace(" %d list size:%d",__LINE__,header.size);
        return false;
    }

    if(!is_empty(&header))
    {
        pp_trace(" %d list size:%d",__LINE__,header.size);
        return false;
    }
    pop_back(&header);
    pop_back(&header);
    pop_back(&header);

    n3 = push_back(&header);
    v3 = 1024;
    n3->value = &v3;

    if(pop_back(&header) != &v3 || !is_empty(&header))
    {
        pp_trace(" %d list size:%d",__LINE__,header.size);
        return false;
    }

    return true;
}

bool test_main()
{

    if(!test_chunks())
    {
        return false;
    }



    if(!test_list())
    {
        return false;
    }

    return true;
}


#endif
