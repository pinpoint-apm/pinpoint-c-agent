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
#ifndef PINPOINT_API_H
#define PINPOINT_API_H

#include "stdint.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include <map>
#include <boost/function.hpp>
#include "pinpoint_error.h"
#include "pinpoint_def.h"
#include "pinpoint_type.h"
#include "pinpoint_agent_context.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/atomic.hpp>

namespace Pinpoint
{
    namespace utils
    {
        uint64_t get_current_microsec_stamp64();
        uint64_t get_current_unixsec_stamp();
        int32_t get_current_process_id();
       // bool get_hostname_and_ipaddr(std::string &hostname,std::string &ipaddr);

        void split_string(const std::string& src, std::vector<std::string>& splits,
                          const std::string& delimiter);

        template <class T>
        bool is_in_vector(const std::vector<T>& vec, const T& t)
        {
            for (std::size_t i = 0;  i < vec.size(); ++i)
            {
                if (vec[i] == t)
                {
                    return true;
                }
            }

            return false;
        }

        typedef std::stringstream* PStream;

        class FormatConverter
        {
        public:
            static std::string int64ToString(int64_t value);
            static int64_t stringToInt64(const std::string& value) throw (std::invalid_argument);
            static std::string getErrorMsg(const char* file, int32_t lineno, const char* msg);
            static std::string getExceptionMsg(const char* file, int32_t lineno, const char* msg);
            static std::string formatMsg(const char* header,const char* file, int32_t lineno, const char* msg);
            static PStream getPStream();
        };
    };

    namespace log {

        void printf_to_stderr_ex(const int32_t iLevel, const char *file, const int32_t line, const char *msg);
        const char* format_log_message_ex(const char* format, ...);
        void set_logging_file(const char*);
        void set_log_level(const char*);
    };

    namespace Configuration{

        using boost::property_tree::ptree;
        /**
         * abstract of config file (ini xml rpc networkconfig)
         */
        class ConfigObject
        {
        public:
            virtual std::string readString(const std::string& configNodeName,
                                           const std::string& defaultValue) const = 0;

            virtual  uint32_t readUInt32(const std::string &configNodeName,
                                         uint32_t defaultValue) const = 0;

            virtual  int32_t readInt32(const std::string &configNodeName,
                                       int32_t defaultValue) const = 0;

            virtual ~ConfigObject(){}
        };
        /**
         * handle conf ini ....
         */
        class ConfFileObject: public ConfigObject
        {
        public:

            ConfFileObject(const char* );

            std::string readString(const std::string& configNodeName,
                                   const std::string& defaultValue) const ;

            uint32_t readUInt32(const std::string &configNodeName,
                                uint32_t defaultValue) const ;

            int32_t readInt32(const std::string &configNodeName,
                              int32_t defaultValue) const;

            virtual ~ConfFileObject(){}
        private:
            ptree _pteeObj;
        };

        /**
         *  encapsulation read info
         */
        class Config
        {
        public:
            Config(ConfigObject& config):_confObj(config)
            {
            }

            std::string readString(const std::string& configNodeName,
                    const std::string& defaultValue) const
            {
                    return _confObj.readString(configNodeName,defaultValue);
            }

            uint32_t readUInt32(const std::string &configNodeName,
                    uint32_t defaultValue) const
            {
                return _confObj.readUInt32(configNodeName,defaultValue);
            }

            int32_t readInt32(const std::string &configNodeName,
                                int32_t defaultValue) const
            {
                return _confObj.readInt32(configNodeName,defaultValue);
            }

         private:
            ConfigObject& _confObj;
        };
    }

    namespace Plugin
    {
//        class FuncArg;
//        class FuncResult;
        class RequestHeader;
        class Interceptor;
        class Plugin;

        typedef boost::shared_ptr<RequestHeader> RHeaderPtr;
//typedef Interceptor* InterceptorRawPtr;
        typedef boost::shared_ptr<Interceptor> InterceptorPtr;
        typedef std::vector<InterceptorPtr> InterceptorPtrVector;
        typedef boost::shared_ptr<Plugin> PluginPtr;
        typedef std::vector<PluginPtr> PluginPtrVector;

        const char EXCEPTION_STRING[] = "exception";
        extern int32_t EXCEPTION_STRING_ID;
        const char ERROR_STRING[] = "error";
        extern int32_t ERROR_STRING_ID;
        const char WARNINGS_STRING[] = "warning";
        extern int32_t WARNINGS_STRING_ID;

        // interceptor interprets its fetcher by specific language
        class FuncArgFetcher
        {
        public:
            virtual ~FuncArgFetcher() {}
        };

        class FuncResultFetcher
        {
        public:
            virtual ~FuncResultFetcher() {}
        };

        class NotSupportedFuncArgFetcher : public FuncArgFetcher
        {

        };

        class NotSupportedFuncResultFetcher : public FuncResultFetcher
        {

        };

        extern NotSupportedFuncArgFetcher notSupportedFuncArgFetcher;
        extern NotSupportedFuncResultFetcher notSupportedFuncResultFetcher;

        class ErrorInfo
        {
        public:
            std::string message;
            std::string file;
            int32_t line;

            ErrorInfo() : line(-1) {}

            const std::string toString() const;
        };

        class ExceptionInfo
        {
        public:
            std::string message;
            std::string file;
            int32_t line;

            ExceptionInfo() : line(-1) {}

            const std::string toString() const;
        };

        typedef std::map<std::string, std::string> HeaderMap;

        class RequestHeader;
        class ResponseHeader;

        class HttpHeader
        {
        public:
            bool getHeader(const std::string& name, std::string& header) const;
            void updateHeader(const HeaderMap& headers);
        private:
            HeaderMap headers;
        };

        class RequestHeader
        {
        public:
            static HttpHeader* getCurrentRequestHeader();
        };

        class ResponseHeader
        {
        public:
            static HttpHeader* getCurrentResponseHeader();
        };

        const uint64_t INVALID_CALL_ID = 0;
        const uint64_t IGNORE_CALL_ID = 1;
        const uint64_t CALL_ID_START = 100;

        class Interceptor
        {
        public:
            Interceptor(const std::string& interceptedFuncName);
            const std::string& getInterceptedFuncName() const;
            virtual int32_t init();
            uint64_t assignCallId();
            void before(uint64_t callId, FuncArgFetcher &argFetcher);
            void end(uint64_t callId, FuncArgFetcher& argFetcher, FuncResultFetcher& resultFetcher);
            void error(const ErrorInfo& errorInfo);
            void exception(uint64_t callId, const ExceptionInfo& exceptionInfo);
        protected:
            virtual void onBefore(uint64_t callId, FuncArgFetcher& argFetcher) = 0;
            virtual void onEnd(uint64_t callId, FuncArgFetcher& argFetcher, FuncResultFetcher& resultFetcher) = 0;
            virtual void onError(const ErrorInfo& errorInfo);
            virtual void onException(uint64_t callId, const ExceptionInfo& exceptionInfo);
            static boost::atomic<uint64_t> callCounter;
        private:
            std::string interceptedFuncName;
        };

        class Plugin
        {
        public:
            virtual int32_t init() = 0;
            virtual std::string getName();
            virtual InterceptorPtrVector& getAllInterceptors() = 0;
            virtual ~Plugin() {}
        };

       static const int32_t INDEX_ARGS[]= {
                Trace::AnnotationKey::ARGS0 ,
                Trace::AnnotationKey::ARGS1 ,
                Trace::AnnotationKey::ARGS2 ,
                Trace::AnnotationKey::ARGS3 ,
                Trace::AnnotationKey::ARGS4 ,
                Trace::AnnotationKey::ARGS5 ,
                Trace::AnnotationKey::ARGS6 ,
                Trace::AnnotationKey::ARGS7 ,
                Trace::AnnotationKey::ARGS8 ,
                Trace::AnnotationKey::ARGS9 ,
                Trace::AnnotationKey::ARGSN
       };

    };

    namespace Trace
    {

        class Trace;

        class SpanEventRecorder;

        class Annotation;

        typedef boost::shared_ptr<Trace> TracePtr;
        typedef boost::shared_ptr<SpanEventRecorder> SpanEventRecorderPtr;
        typedef boost::shared_ptr<Annotation> AnnotationPtr;
    }

    namespace Agent
    {
        typedef enum
        {
            NGINX_AGENT_TYPE,
            HTTPD_AGENT_TYPE,
            PHP_AGENT_TYPE
        } AgentType;

        typedef boost::function<int64_t(const char*, uint32_t)> LogOutputFunc;
//        typedef boost::function<int32_t()> TurnOnAOPFunc;
//        typedef boost::function<int32_t()> TurnOffAOPFunc;
        typedef boost::function<int32_t(const Plugin::InterceptorPtr& interceptorPtr)> AddInterceptorFunc;
        typedef boost::function<std::string(Pinpoint::Naming::eName) > GetHostProcessInfo;

        const int32_t INVALID_API_ID = 0;
        const int32_t INVALID_STRING_ID = 0;

        struct AgentFunction
        {
            LogOutputFunc logOutputFunc; /* how to output log */
//            TurnOnAOPFunc turnOnAOPFunc;
//            TurnOffAOPFunc turnOffAOPFunc;
            AddInterceptorFunc addInterceptorFunc;
            GetHostProcessInfo getHostProcessInfo;
        };

        class Agent;
        typedef boost::shared_ptr<Agent> AgentPtr;

        typedef enum
        {
            AGENT_CREATED = 1,
            AGENT_PRE_INITED,
            AGENT_PRE_INIT_FAILURE,
            AGENT_INITED,
            AGENT_INIT_FAILURE,
            AGENT_STARTED,
            AGENT_START_FAILURE,
            AGENT_STOPPED
        } AgentStatus;

        class Sampling
        {
        public:
            // 1 byte dummy mark for further expansion of sampling specs
            static const char* SAMPLING_RATE_PREFIX;

            static const char* SAMPLING_RATE_FALSE;
            static const char* SAMPLING_RATE_TRUE;

            virtual bool checkTraceLimit() = 0;
            virtual bool checkSkipTraceTime(int32_t elapse) = 0;
            virtual void startNewTrace(bool isContinuedTrace) = 0;
            virtual ~Sampling() {};

        };

        typedef boost::shared_ptr<Sampling> SamplingPtr;

        class Agent
        {
        public:
            static int32_t createAgent();

            static AgentPtr &getAgentPtr();
            virtual int32_t preInit(AgentType agentType,
                                    const AgentFunction& agentFunction,
                                    Pinpoint::Configuration::Config &conf) = 0;
            virtual int32_t init(Plugin::PluginPtrVector& pluginPtrVector) = 0;
            virtual int32_t start() = 0;
            virtual int32_t stop() = 0;
            virtual int32_t addApi(const char* api_info, int32_t line, ApiType type=API_UNDEFINED) = 0;
            virtual int32_t addString(const std::string& stringValue) = 0;
            virtual volatile AgentStatus getAgentStatus() const = 0;
            virtual SamplingPtr& getSamplingPtr() = 0;
            virtual int32_t sendTrace(const Trace::TracePtr &tracePtr) = 0;
        private:

            static AgentPtr agentPtr;
        };
    };

    namespace Trace
    {

        class Trace
        {
        public:
            static int32_t startTrace(const Plugin::HttpHeader* pHeader);
            static int32_t endTrace();
            static TracePtr getCurrentTrace();
            static bool isStarted();

            virtual void markBadTrace() = 0;
            virtual bool isBadTrace() = 0;
            virtual const std::string& getTransactionId() const = 0;
            virtual SpanEventRecorderPtr getSpanEventRecorderPtr(uint64_t callId) = 0;
            virtual SpanEventRecorderPtr traceBlockBegin(uint64_t callId) = 0;
            virtual void traceBlockEnd(const SpanEventRecorderPtr& spanEventRecorderPtr=NULL_SPAN_EVENT_RECODER_PTR) = 0;
            virtual SpanEventRecorderPtr getCurrentTraceBlock() = 0;

            virtual void setApiId(int32_t apiId) = 0;
            virtual void addAnnotationPtr(const AnnotationPtr& annotationPtr) = 0;
            virtual void setEndPoint(const std::string& endPoint) = 0;
            virtual void setAcceptorHost(const std::string& acceptorHost) = 0;
            virtual void setRemoteAddr(const std::string& remoteAddr) = 0;
            virtual void setRpc(const std::string& rpc) = 0;
            virtual void setExceptionInfo(int32_t errId, const std::string& errMsg) = 0;
            virtual void setErr(int32_t err) = 0;
            virtual void finishTrace() = 0;
            virtual int32_t getTraceElapse() = 0;

            virtual bool isCodeError() = 0;
            virtual void markCodeError() = 0;

            virtual int32_t getNextSpanInfo(Plugin::HeaderMap& nextHeader, int64_t& nextSpanId) = 0;

            virtual ~Trace();
        protected:
            static SpanEventRecorderPtr NULL_SPAN_EVENT_RECODER_PTR;
        };

        class SpanEventRecorder
        {
        public:
            virtual void markBeforeTime() = 0;

            virtual void markAfterTime() = 0;

            virtual void setApiId(int32_t apiId) = 0;

            virtual void setServiceType(int16_t serviceType) = 0;

            virtual void addAnnotationPtr(const AnnotationPtr& annotationPtr) = 0;

            virtual void setEndPoint(const std::string& endPoint) = 0;
            virtual void setDestinationId(const std::string& destinationId) = 0;
            virtual void setRpc(const std::string& rpc) = 0;
            virtual void setExceptionInfo(int32_t errId, const std::string& errMsg) = 0;
            virtual void setNextSpanId(int64_t nextSpanId) = 0;
            virtual ~SpanEventRecorder(){}
            const static SpanEventRecorderPtr NULL_SPAN_EVENT_RECORDER_PTR;
        };

        class Annotation
        {
        public:
            static AnnotationPtr createAnnotation(int32_t key);

            virtual void addByteValue(int8_t value) = 0;
            virtual void addShortValue(int16_t value) = 0;
            virtual void addIntValue(int32_t value) = 0;
            virtual void addLongValue(int64_t value) = 0;
            virtual void addStringValue(const std::string& s) = 0;
            virtual void addBinaryValue(const std::string& v) = 0;
            virtual void addBoolValue(bool b) = 0;
            virtual void addDoubleValue(double d) = 0;
            virtual void addIntStringValue(int32_t intValue, const std::string& s) = 0;
            virtual void addIntStringStringValue(int32_t intValue, const std::string& s1, const std::string& s2) = 0;
        };
    }
}
#define PP_LOG_DEBUG      4
#define PP_LOG_INFO       3
#define PP_LOG_WARNING    2
#define PP_LOG_ERROR      1
#define LOGD(format, ...)  Pinpoint::log::printf_to_stderr_ex(PP_LOG_DEBUG,   __FILE__, __LINE__, Pinpoint::log::format_log_message_ex(format, ##__VA_ARGS__));
#define LOGI(format, ...)   Pinpoint::log::printf_to_stderr_ex(PP_LOG_INFO,    __FILE__, __LINE__, Pinpoint::log::format_log_message_ex(format, ##__VA_ARGS__));
#define LOGW(format, ...)   Pinpoint::log::printf_to_stderr_ex(PP_LOG_WARNING, __FILE__, __LINE__, Pinpoint::log::format_log_message_ex(format, ##__VA_ARGS__));
#define LOGE(format, ...)  Pinpoint::log::printf_to_stderr_ex(PP_LOG_ERROR,   __FILE__, __LINE__, Pinpoint::log::format_log_message_ex(format, ##__VA_ARGS__));
#endif
