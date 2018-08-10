/*
 * log.cpp
 *
 *  Created on: Jan 11, 2017
 *      Author: bluse
 */


#include "log.h"

#ifdef _MSC_VER
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>  /* ipv6 */
#include <mswsock.h>
#include <shellapi.h>
#include <stddef.h>    /* offsetof() */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <locale.h>
typedef long  time_t;
typedef HANDLE fd_t;
// @todo

/**
 *  set_logfile_root_path
 *  free_log
 *  log_message
 */

https://msdn.microsoft.com/en-us/library/windows/desktop/aa365683(v=vs.85).aspx

https://msdn.microsoft.com/en-us/library/windows/desktop/aa363858(v=vs.85).aspx

D:\workplace\open_code\webserver\nginx\src\os\win32\ngx_files.h
D:\workplace\open_code\webserver\nginx\src\os\win32\ngx_files.c

D:\Program Files\boost_1_63_0\libs\iostreams\src\file_descriptor.cpp



#endif 
#ifdef __GNUC__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdint.h>

typedef int fd_t;

#endif

#define MAXFILESTRLEN 128
#define MAX_LINE_LENGTH 1024
#define STD_ERROR_MASK 0x1
#define HASERROR 0x2


typedef struct _LogInfo
{
    fd_t fileFd;
    char fileName[MAXFILESTRLEN];
    char prefixFilePath[MAXFILESTRLEN];
}LogInfo;

LogInfo _logInfo ={ -1,{0},{0}};

#ifdef __GNUC__

int set_logfile_root_path(const char* logFileDirectory)
{
    if( logFileDirectory == NULL)
    {
        return -1;
    }
    char filePathName[MAXFILESTRLEN] = {0};
    snprintf(filePathName,MAXFILESTRLEN,"%s/pinpoint_log.txt",logFileDirectory);
    int fd = open(filePathName, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    if (0 > fd)
    {
        fprintf(stderr,"open %s failed : %s \n",filePathName,strerror(errno));
        return -1;
    }
    _logInfo.fileFd = fd;
    strncpy(_logInfo.prefixFilePath,logFileDirectory,MAXFILESTRLEN);
    return 0;
}

void free_log()
{
    // much possible a file
    if(_logInfo.fileFd > STDERR_FILENO)
    {
        close(_logInfo.fileFd);
    }
    //
    LogInfo t_logInfo ={ -1,{0},{0}};
    _logInfo = t_logInfo;
}

int64_t log_message(const char *msgStr,uint32_t msgLen)
{
    if( msgStr == NULL ||msgLen == 0 )
    {
        return 0;
    }

    return write(_logInfo.fileFd > -1 ? _logInfo.fileFd : STDERR_FILENO, msgStr,msgLen);
}
#endif 
