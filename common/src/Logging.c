#include "common.h"

#include <stdio.h>

#if defined(__linux__) || defined(_UNIX)
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdarg.h>
#include <unistd.h>
#define gettid() syscall(SYS_gettid)
#define getOSPid getpid
#elif defined(_WIN32)
#include <processthreadsapi.h>
#define getOSPid GetCurrentProcessId
#else 

#endif

static log_error_cb _error_cb;

/**
 *  Note: the logging should be disable when in Real env
 */
void pp_trace(const char *format,...)
{
    if( (global_agent_info.inter_flag & E_LOGGING) == 0)
    {
        return ;
    }

    char buf[LOG_SIZE]={0};
    va_list args;
    va_start(args, format);
    
    int n = snprintf(buf,LOG_SIZE,"[pinpoint] [%d] [%ld]",getOSPid(),gettid());
    vsnprintf(buf+n, LOG_SIZE -n - 1 ,format, args);
    va_end(args);
    
    if (_error_cb){
        _error_cb(buf);
    }else{
        fprintf(stderr,"%s\n",buf);
    }
}

// register when thread/module/process start
void register_error_cb(log_error_cb error_cb)
{
    _error_cb  = error_cb;
}
