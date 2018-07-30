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
#include "log_utility.h"
#include "stdio.h"
#include "stdarg.h"
#include "utility.h"
#include "log.h"

#ifdef DEBUG
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
#endif

namespace Pinpoint
{
    namespace log
    {

        void default_log_output_func(const char *msg, uint32_t len)
        {
            ::fprintf(stderr, "%s", msg);
        }

        boost::thread_specific_ptr<char> LogUtility::formatBufferTls(LogUtility::deleteBuffer);
        boost::thread_specific_ptr<char> LogUtility::formatFullBufferTls(LogUtility::deleteBuffer);
        LogUtilityPtr LogUtility::logUtilityPtr;

        LogUtility::LogUtility(const char* logFilePath, const Agent::LogOutputFunc& logOutputFunc)
                : logOutputFunc(logOutputFunc),systemLevel(PP_LOG_DEBUG)
        {
            if (logFilePath != NULL)
            {
                set_logfile_root_path(logFilePath);
            }
        }

        LogUtility::~LogUtility()
        {
            free_log();
        }

        LogUtilityPtr &LogUtility::getInstance()
        {
            return logUtilityPtr;
        }

        int32_t LogUtility::initLogUtility(const char* logFilePath, const Agent::LogOutputFunc& logOutputFunc)
        {
            LogUtility::logUtilityPtr.reset();

            try
            {
                if (logOutputFunc.empty())
                {
                    Agent::LogOutputFunc defaultLogOutputFunc = boost::bind(log_message,_1,_2);
                    LogUtility::logUtilityPtr.reset(new LogUtility(logFilePath, defaultLogOutputFunc));
                }
                else
                {
                    LogUtility::logUtilityPtr.reset(new LogUtility(logFilePath, logOutputFunc));
                }
            }
            catch (std::bad_alloc &)
            {
                return OUT_OF_MEMORY;
            }



#ifndef NDEBUG
            logUtilityPtr->setSystemLogLevel(PP_LOG_INFO);
#else
            logUtilityPtr->setSystemLogLevel(PP_LOG_DEBUG);
#endif

            return SUCCESS;
        }

        int32_t LogUtility::getSystemLogLevel() const
        {
            return this->systemLevel;
        }

        void LogUtility::setSystemLogLevel(const int iLevel)
        {
            this->systemLevel = iLevel;
        }

        void LogUtility::setSystemLogLevel(const char* levelStr)
        {
            setSystemLogLevel(logStrToLevel(levelStr));
        }

        int32_t LogUtility::validLogLevel(const int iLevel)
        {
            if (iLevel > PP_LOG_DEBUG)
            {
                return PP_LOG_DEBUG;
            }

            if (iLevel < PP_LOG_ERROR)
            {
                return PP_LOG_ERROR;
            }

            return iLevel;
        }

        const char *LogUtility::logLevelToString(const int32_t iLevel)
        {
            switch (iLevel)
            {
                case PP_LOG_ERROR:
                    return "ERROR";

                case PP_LOG_WARNING:
                    return "WARN";

                case PP_LOG_DEBUG:
                    return "DEBUG";

                default:
                    return "INFO";
            };
        }

        int32_t LogUtility::logStrToLevel(const char *pLogStr)
        {
            static const char *_pLogStr[] =
                    {
                            "ERROR",
                            "WARN",
                            "INFO",
                            "DEBUG",
                            NULL
                    };

            int i = 0;
            for (; _pLogStr[i] != NULL;)
            {
                if (strcasecmp(_pLogStr[i++], pLogStr) == 0)
                    break;
            }

            return (i > 0 && i <= PP_LOG_DEBUG) ? (i) : (-1);
        }

        void LogUtility::deleteBuffer(char *buffer)
        {
            delete[] buffer;
        }

        const char *LogUtility::formatLogMessage(const char *format, va_list ap)
        {
            char *logBuf = this->formatBufferTls.get();
            if (logBuf == NULL)
            {
                try
                {
                    logBuf = new char[LogUtility::FORMAT_LOG_BUF_SIZE];
                }
                catch (std::bad_alloc &)
                {
                    return "format_log_message: Unable to allocate memory buffer";
                }

                this->formatBufferTls.reset(logBuf);
            }

            vsnprintf(logBuf, FORMAT_LOG_BUF_SIZE - 1, format, ap);

            return logBuf;
        }

        void LogUtility::outputLog(const int32_t iLevel, const char *file, const int line, const char *msg)
        {
            char *fullLogBuf = this->formatFullBufferTls.get();
            if (fullLogBuf == NULL)
            {
                try
                {
                    fullLogBuf = new char[LogUtility::FORMAT_LOG_BUF_SIZE];
                }
                catch (std::bad_alloc &)
                {
                    return;
                }

                this->formatFullBufferTls.reset(fullLogBuf);
            }

            //check level
            const int level = this->validLogLevel(iLevel);

            if (level > this->getSystemLogLevel())
            {
                return;
            }


            uint32_t msgLen = 0;
//#define DEBUG
#ifdef UNITTEST
            //output log message
            msgLen = snprintf(fullLogBuf, FORMAT_LOG_BUF_SIZE, "%s %s [pinpoint] [%4d:%ld] %s:%d [%s] %s\n \e[0m",
                    (iLevel <= PP_LOG_INFO) ? ("\e[0;31m") : "", this->getLocalTimeString().c_str(),
                    ::getpid(), gettid(), ::basename((char *) file), line, LogUtility::logLevelToString(level), msg);
#elif DEBUG
            msgLen = snprintf(fullLogBuf, FORMAT_LOG_BUF_SIZE, "%s [pinpoint] [%4d:%ld] %s:%d [%s] %s\n",
                    this->getLocalTimeString().c_str(),::getpid(), gettid(),
                    ::basename((char *) file), line, LogUtility::logLevelToString(level), msg);
#else
            msgLen = snprintf(fullLogBuf, FORMAT_LOG_BUF_SIZE, "%s [pinpoint] [%4d] %s:%d [%s] %s\n ",
                                         this->getLocalTimeString().c_str(),
                    ::getpid(), ::basename((char *) file), line, LogUtility::logLevelToString(level), msg);
#endif
            //  msglen include '\0'
            if(!this->logOutputFunc.empty())
            {
                if(msgLen > FORMAT_LOG_BUF_SIZE) // msg truncated
                {
                    static const char trun_str[]= "......\n";
                    strncpy(fullLogBuf+(FORMAT_LOG_BUF_SIZE - sizeof(trun_str)),trun_str,sizeof(trun_str));
                    msgLen = FORMAT_LOG_BUF_SIZE;
                }

                this->logOutputFunc(fullLogBuf, msgLen);
            }
        }

        std::string LogUtility::format_1k_buffer(const char *fmt, ...)
        {
            if (utils::safe_strlen(fmt) <= 0)
            {
                return "";
            }

            char buffer[1025];
            memset(buffer, '\0', 1025);

            va_list args;
            va_start(args, fmt);
#ifdef _WIN32
            _vsnprintf(buffer, 1024, fmt, args);
#else
            vsnprintf(buffer, 1024, fmt, args);
#endif
            va_end(args);

            return std::string(buffer);
        }

        std::string LogUtility::getLocalTimeString()
        {
            struct tm tmLocal;
            time_t tNow = ::time(NULL);
#ifdef _WIN32
            ::localtime_s(&tmLocal, &tNow);
#else
            ::localtime_r(&tNow, &tmLocal);
#endif
            tmLocal.tm_mon = tmLocal.tm_mon + 1;
            tmLocal.tm_year = tmLocal.tm_year + 1900;
            return format_1k_buffer("%04d-%02d-%02d %02d:%02d:%02d", tmLocal.tm_year, tmLocal.tm_mon,
                                    tmLocal.tm_mday, tmLocal.tm_hour, tmLocal.tm_min, tmLocal.tm_sec);
        }


        const char *format_log_message_ex(const char *format, ...)
        {
            LogUtilityPtr &logUtilityPtr = LogUtility::getInstance();
            if (logUtilityPtr != NULL)
            {
                va_list ap;
                va_start(ap, format);
                const char *msg = logUtilityPtr->formatLogMessage(format, ap);
                va_end(ap);
                return msg;
            }
            return NULL;
        }

        void printf_to_stderr_ex(const int iLevel, const char *file, const int line, const char *msg)
        {
            LogUtilityPtr &logUtilityPtr = LogUtility::getInstance();
            if (logUtilityPtr != NULL)
            {
                logUtilityPtr->outputLog(iLevel, file, line, msg);
            }
        }

        void set_logging_file(const char* pathNameStr)
        {
            LogUtility::initLogUtility(pathNameStr, NULL);
        }

        void set_log_level(const char* levelNameStr)
        {
            LogUtility::getInstance()->setSystemLogLevel(levelNameStr);
        }

        void set_logout_func_cb(const Agent::LogOutputFunc& func)
        {
            LogUtility::getInstance()->setLogOutputFunc(func);
        }

    }
}
