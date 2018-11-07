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
#ifndef PINPOINT_LOG_UTILITY_H
#define PINPOINT_LOG_UTILITY_H

#include <boost/shared_ptr.hpp>
#include <stdio.h>
#include <boost/thread.hpp>
#define __STDC_LIMIT_MACROS 
#include <stdint.h>
#include "pinpoint_api.h"

namespace Pinpoint
{
    namespace log
    {
        class LogUtility;

        typedef boost::shared_ptr<LogUtility> LogUtilityPtr;


        class LogUtility
        {
        public:
            void setSystemLogLevel(const int iLevel);
            void setSystemLogLevel(const char*);
            int32_t getSystemLogLevel() const;
            const char *logLevelToString(const int iLevel);
            static int32_t logStrToLevel(const char *pLogStr);
            static int32_t validLogLevel(const int iLevel);
            void setLogOutputFunc( Agent::LogOutputFunc func)
            {
                logOutputFunc = func;
            }
            static int32_t initLogUtility(const char* logFilePath, const Agent::LogOutputFunc& logOutputFunc);
            static LogUtilityPtr &getInstance();
            const char* formatLogMessage(const char* format, va_list ap);
            void outputLog(const int32_t iLevel, const char *file, const int line, const char *msg);
            ~LogUtility();
        private:
            LogUtility(const char* logFilePath, const Agent::LogOutputFunc& logOutputFunc);
            Agent::LogOutputFunc logOutputFunc;
            int32_t systemLevel;
            static boost::thread_specific_ptr<char> formatBufferTls;
            static boost::thread_specific_ptr<char> formatFullBufferTls;
            static LogUtilityPtr logUtilityPtr;
            static void deleteBuffer(char* buffer);
            const static uint32_t FORMAT_LOG_BUF_SIZE = 4096;
            static std::string getLocalTimeString();
            static std::string format_1k_buffer(const char *fmt, ...);
        };
    }
}
#endif
