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
#include "pinpoint_api.h"

using namespace Pinpoint::log;

namespace Pinpoint
{
    namespace Plugin
    {

        //<editor-fold desc="Interceptor">

        boost::atomic<uint64_t> Interceptor::callCounter(CALL_ID_START);

        Interceptor::Interceptor(const std::string& interceptedFuncName)
                : interceptedFuncName(interceptedFuncName)
        {

        }


        const std::string& Interceptor::getInterceptedFuncName() const
        {
            return interceptedFuncName;
        }


        int32_t Interceptor::init()
        {
            return SUCCESS;
        }

        uint64_t Interceptor::assignCallId()
        {
            return callCounter.fetch_add(1);
        }

        void Interceptor::before(uint64_t callId, FuncArgFetcher &argFetcher)
        {
            onBefore(callId, argFetcher);
        }

        void Interceptor::end(uint64_t callId, FuncArgFetcher &argFetcher, FuncResultFetcher &resultFetcher)
        {
            Trace::TracePtr tracePtr = Trace::Trace::getCurrentTrace();
            if (tracePtr == NULL)
            {
                return;
            }

            if (tracePtr->isCodeError())
            {
                LOGI("error happen! skip onend call.");
                return;
            }

            onEnd(callId, argFetcher, resultFetcher);
        }

        void Interceptor::error(const ErrorInfo &errorInfo)
        {
            onError(errorInfo);
        }

        void Interceptor::exception(uint64_t callId, const ExceptionInfo &exceptionInfo)
        {
            onException(callId, exceptionInfo);
        }

        void Interceptor::onError(const ErrorInfo& errorInfo)
        {
            if (Trace::Trace::isStarted())
            {
                Trace::TracePtr tracePtr = Trace::Trace::getCurrentTrace();
                PINPOINT_ASSERT (tracePtr != NULL);
                tracePtr->setErr(1);

                tracePtr->markCodeError();

                std::string errorMsg = errorInfo.toString();
                tracePtr->setExceptionInfo(ERROR_STRING_ID, errorMsg);

                if (Trace::Trace::endTrace() != SUCCESS)
                {
                    LOGE("endTrace failed.");
                }
            }
        }

        void Interceptor::onException(uint64_t callId, const ExceptionInfo& exceptionInfo)
        {
            if (Trace::Trace::isStarted())
            {
                Trace::TracePtr tracePtr = Trace::Trace::getCurrentTrace();
                PINPOINT_ASSERT (tracePtr != NULL);

                std::string errorMsg = exceptionInfo.toString();

                Trace::SpanEventRecorderPtr spanEventRecorderPtr = tracePtr->getSpanEventRecorderPtr(callId);
                if (spanEventRecorderPtr == NULL)
                {
                    tracePtr->setExceptionInfo(EXCEPTION_STRING_ID, errorMsg);
                }
                else
                {
                    spanEventRecorderPtr->setExceptionInfo(EXCEPTION_STRING_ID, errorMsg);
                }

            }
        }
        //</editor-fold>

        std::string Plugin::getName()
        {
            std::string name = typeid(*this).name();

            // 10CurlPlugin ==> CurlPlugin
            size_t start = 0;
            while(name[start] >= '0' && name[start] <= '9') start++;
            return name.substr(start);
        }

        NotSupportedFuncArgFetcher notSupportedFuncArgFetcher;
        NotSupportedFuncResultFetcher notSupportedFuncResultFetcher;
    }

    namespace Configuration{

        ConfFileObject::ConfFileObject(const char* pFileName)
        {
            ptree ptConfTree;
            try
            {
                LOGI("pinpoint config file path %s",pFileName);
                boost::property_tree::ini_parser::read_ini(pFileName, ptConfTree);
            }
            catch (boost::property_tree::ini_parser_error &error)
            {
                LOGE("pinpoint config file  %s failed",pFileName);
                throw std::invalid_argument(pFileName);
            }
            _pteeObj = ptConfTree;
        }

#pragma GCC diagnostic ignored "-Wstrict-aliasing"

        std::string ConfFileObject::readString(const std::string& configNodeName,
                const std::string& defaultValue) const
        {
            std::string s = _pteeObj.get<std::string>(configNodeName, "");

            if (s.length() == 0)
            {
                LOGI(" [%s] config not find use default [%s]", configNodeName.c_str(),
                        defaultValue.c_str());
                return defaultValue;
            }

            LOGT("%s=%s", configNodeName.c_str(), s.c_str());

            return s;
        }

#pragma GCC diagnostic ignored "-Wstrict-aliasing"

        uint32_t ConfFileObject::readUInt32(const std::string &configNodeName,
                uint32_t defaultValue) const
        {
            const int32_t omit = -1;

            int32_t i32_value = _pteeObj.get < int32_t > (configNodeName, omit);
            if (i32_value == omit)
            {
                LOGI(" [%s] config not find use default [%d]", configNodeName.c_str(),
                        defaultValue);
                return defaultValue;
            }

            LOGI("%s=%d", configNodeName.c_str(), i32_value);
            return (uint32_t) i32_value;
        }

#pragma GCC diagnostic ignored "-Wstrict-aliasing"

        int32_t ConfFileObject::readInt32(const std::string &configNodeName,
                                          int32_t defaultValue) const
        {
            const int32_t omit = -1;

            int32_t i32_value = _pteeObj.get < int32_t > (configNodeName, omit);
            if (i32_value == omit)
            {
                LOGT(" [%s] config not find use default [%d]", configNodeName.c_str(),
                         defaultValue);
                return defaultValue;
            }

            LOGI("%s=%d", configNodeName.c_str(), i32_value);
            return i32_value;
        }

    }

}

