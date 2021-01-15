////////////////////////////////////////////////////////////////////////////////
// Copyright 2020 NAVER Corp
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

#include "common.h"

#include <stdio.h>

#if defined(__linux__) || defined(_UNIX) 
    #include <sys/types.h>
    #include <sys/syscall.h>
    #include <stdarg.h>
    #include <unistd.h>
    #define gettid() syscall(SYS_gettid)
    #define getOSPid getpid
#endif

#if defined(__APPLE__)
    #include <sys/types.h>
    #include <sys/syscall.h>
    #include <stdarg.h>
    #include <unistd.h>
    #define gettid() (long)getpid()
    #define getOSPid getpid
#endif

#if defined(_WIN32)
    #include <processthreadsapi.h>
    #define getOSPid GetCurrentProcessId
#endif

static log_msg_cb _error_cb;

static void log_format_out(const char *format,va_list* args)
{
    char buf[LOG_SIZE]={0};
    int n = snprintf(buf,LOG_SIZE,"[pinpoint] [%d] [%ld]",getOSPid(),gettid());
    vsnprintf(buf+n, LOG_SIZE -n - 1 ,format, *args);
    
    if (_error_cb){
        _error_cb(buf);
    }else{
        fprintf(stderr,"%s\n",buf);
    }
}

/**
 *  Note: the logging should be disable when in Real env
 */
void pp_trace(const char *format,...)
{
    if( (global_agent_info.inter_flag & E_LOGGING) == 0)
    {
        return ;
    }
    va_list args;
    va_start(args, format);
    // there is no need to create a LOG_SIZE in every call
    log_format_out(format,&args);
    va_end(args);
}

// register when thread/module/process start
void register_error_cb(log_msg_cb error_cb)
{
    _error_cb  = error_cb;
}
