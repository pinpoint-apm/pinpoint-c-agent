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
#ifndef PINPOINT_TRACE_H
#define PINPOINT_TRACE_H

#include <boost/asio/detail/shared_ptr.hpp>
#include <vector>
#include <string>
#include <boost/thread.hpp>
#include "thrift/gen-cpp/Trace_types.h"
#include "pinpoint_api.h"
#include "memory_pool.h"
//#include "utility.h"
#include "pinpoint_agent_context.h"

namespace Pinpoint
{
    namespace Trace
    {


        class SpanEvent;

        class CallStack;

        class Span;

        class Trace;

        class TraceId;

        class TransactionId;

        class SpanId;

        class PinpointHttpHeader;

        class DefaultTrace;

        class DefaultAnnotation;

        typedef boost::shared_ptr<Trace> TracePtr;
        typedef boost::shared_ptr<DefaultTrace> DefaultTracePtr;
        typedef boost::shared_ptr<TraceId> TraceIdPtr;
        typedef boost::shared_ptr<SpanEvent> SpanEventPtr;
        typedef boost::shared_ptr<Span> SpanPtr;
        typedef boost::shared_ptr<DefaultAnnotation> DefaultAnnotationPtr;

        typedef enum
        {
            VALID_HTTP_HEADER = 0,
            INVALID_HTTP_HEADER,
            NULL_HTTP_HEADER
        } PinpointHttpHeaderType;

        class PinpointHttpHeader
        {
        public:
            static PinpointHttpHeader createHeader(const Plugin::HttpHeader* pHeader);

            PinpointHttpHeaderType type;
            std::string traceId;
            int64_t spanId;
            int64_t pSpanId;
            bool sampled;
            int16_t flag;
            std::string parentApplicationName;
            int16_t parentApplicationType;
            std::string host;
        };

        class SpanEvent : public SpanEventRecorder
        {
        public:
            explicit SpanEvent(const SpanPtr& spanPtr);
            void recordTraceId(const TraceIdPtr &traceIdPtr);

            virtual void markBeforeTime();

            virtual void markAfterTime();

            virtual void setApiId(int32_t apiId);

            virtual void setServiceType(int16_t serviceType);

            virtual void addAnnotationPtr(const AnnotationPtr& annotationPtr);

            virtual void setEndPoint(const std::string& endPoint);
            virtual void setDestinationId(const std::string& destinationId);
            virtual void setRpc(const std::string& rpc);
            virtual void setExceptionInfo(int32_t errId, const std::string& errMsg);

            virtual void setNextSpanId(int64_t nextSpanId);

            void setSequence(int16_t sequence);
            void setDepth(int32_t depth);

            void completeSpanEvent();

            const TSpanEvent &getTSpanEvent() const ;

            const static SpanEventPtr NULL_SPAN_EVENT_PTR;

        private:
            const static int32_t DEFAULT_ANNOTATION_NUM = 5;
            TSpanEvent tSpanEvent;
            SpanPtr spanPtr;
            std::vector<AnnotationPtr> annotationPtrList;
        };

        class DefaultAnnotation : public Annotation
        {
        public:
           explicit DefaultAnnotation(int32_t key);
            virtual void addByteValue(int8_t value);
            virtual void addShortValue(int16_t value);
            virtual void addIntValue(int32_t value);
            virtual void addLongValue(int64_t value);
            virtual void addStringValue(const std::string& s);
            virtual void addBinaryValue(const std::string& v);
            virtual void addBoolValue(bool b);
            virtual void addDoubleValue(double d);
            virtual void addIntStringValue(int32_t intValue, const std::string& s);
            virtual void addIntStringStringValue(int32_t intValue, const std::string& s1, const std::string& s2);
            virtual void addTLongIntIntByteByteStringValue(std::string&,int);
            virtual ~DefaultAnnotation(){}
            const TAnnotation& getTAnnotation()const ;
        private:
            TAnnotation tAnnotation;
        };


        class CallStack
        {
        public:
            CallStack();
            int32_t push(SpanEventPtr& spanEventPtr);
            SpanEventPtr pop();
            SpanEventPtr peek();
            SpanEventPtr get(int32_t index);
            bool empty();
        private:
            static const int32_t DEFAULT_STACK_LEN = 10;
            std::vector<SpanEventPtr> stack;
            int32_t sequence;
            int32_t index;
            int32_t latestStackIndex;
        };


        class Span
        {
        public:
            // new trace
            void recordTraceId(const TraceIdPtr &traceIdPtr);

            // continued trace
            void recordTraceId(const TraceIdPtr &traceIdPtr, const PinpointHttpHeader& header);

            void markBeforeTime();

            void markAfterTime();

            void setApiId(int32_t apiId);

            void setEndPoint(const std::string& endPoint);

            void setRemoteAddr(const std::string& remoteAddr);

            void setAcceptorHost(const std::string& acceptorHost);

            void setRpc(const std::string& rpc);

            void setSpanEventList(const std::vector<TSpanEvent>& tSpanEventVector);

            void setAnnotationList(const std::vector<TAnnotation>& tAnnotationVector);

            void setExceptionInfo(int32_t errId, const std::string& errMsg);

            void setErr(int32_t err);

            const TSpan &getTSpan() const ;

            int64_t getAfterTime() const ;

            int64_t getBeforeTime() const;
        private:
            TSpan tSpan;
        };

        typedef std::map<uint64_t, SpanEventPtr> SpanEventPtrMap;
        class DefaultTrace : public Trace
        {
        public:
            DefaultTrace(const PinpointHttpHeader& header, const TraceIdPtr &traceIdPtr,
                         const SpanPtr &spanPtr, bool isContinued);

            virtual void markBadTrace();
            virtual bool isBadTrace();
            virtual const std::string& getTransactionId() const;
            virtual SpanEventRecorderPtr getSpanEventRecorderPtr(uint64_t callId);

            virtual void setApiId(int32_t apiId);
            virtual void addAnnotationPtr(const AnnotationPtr& annotationPtr);

            virtual ~DefaultTrace();
            virtual SpanEventRecorderPtr traceBlockBegin(uint64_t callId);
            virtual void traceBlockEnd(const SpanEventRecorderPtr& spanEventRecorderPtr=Trace::NULL_SPAN_EVENT_RECODER_PTR);
            virtual SpanEventRecorderPtr getCurrentTraceBlock();

            virtual void setEndPoint(const std::string& endPoint);
            virtual void setRemoteAddr(const std::string& remoteAddr);
            virtual void setAcceptorHost(const std::string& acceptorHost);
            virtual void setRpc(const std::string& rpc);
            virtual void setExceptionInfo(int32_t errId, const std::string& errMsg);
            virtual void setErr(int32_t err);

            virtual bool isCodeError();
            virtual void markCodeError();

            virtual void finishTrace();

            virtual int32_t getTraceElapse();

            virtual int32_t getNextSpanInfo(Plugin::HeaderMap& nextHeader, int64_t& nextSpanId);

            const TraceIdPtr& getTraceIdPtr() const;

            SpanPtr& getSpanPtr();

            void completeTrace();

        private:
            const static int32_t DEFAULT_SPAN_EVENT_NUM = 20;
            const static int32_t DEFAULT_ANNOTATION_NUM = 5;
            TraceIdPtr traceIdPtr;
            SpanPtr spanPtr;
            CallStack callStack;
            bool isContinued;
            SpanEventPtrMap spanEventPtrMap;
            std::vector<SpanEventPtr> spanEventPtrList;
            std::vector<AnnotationPtr> annotationPtrList;
            PinpointHttpHeader header;
            bool isBad;
            bool isCodeError_;
            static boost::thread_specific_ptr<TracePtr> currentTracePtrTls;
        };


// todo rewrite for FormatConverter
        class TransactionId
        {
        public:
            friend bool operator==(const TransactionId &a, const TransactionId &b);

            TransactionId(const std::string &agentId, int64_t agentStartTime, int64_t transactionSequence)
                    : agentId(agentId), agentStartTime(agentStartTime), transactionSequence(transactionSequence)
            {

            }

            std::string getAgentId() const
            {
                return agentId;
            }

            void setAgentId(const std::string &agentId)
            {
                this->agentId = agentId;
            }

            int64_t getAgentStartTime() const
            {
                return agentStartTime;
            }

            void setAgentStartTime(int64_t agentStartTime)
            {
                this->agentStartTime = agentStartTime;
            }

            int64_t getTransactionSequence() const
            {
                return transactionSequence;
            }

            void setTransactionSequence(int64_t transactionSequence)
            {
                this->transactionSequence = transactionSequence;
            }

            static const std::string TRANSACTION_ID_DELIMITER;
            static const int8_t TVERSION = 0;

            static std::string formatString(const TransactionId &transactionId);

            static std::string formatString(const std::string &agentId, const int64_t agentStartTime,
                                            int64_t transactionSequence);

            static std::vector<int8_t> formatBytes(const std::string &agentId, int64_t agentStartTime,
                                                   int64_t transactionSequence);

            static std::vector<int8_t> formatBytes(const TransactionId &transactionId);

            static TransactionId parseTransactionId(
                    const std::vector<int8_t> &transactionId);

            static TransactionId parseTransactionId(const std::string &transactionId);

            static bool isValidTransactionIdStr(const std::string &transactionId);

        private:
            std::string agentId;
            int64_t agentStartTime;
            int64_t transactionSequence;
        };

        class SpanId
        {
        public:
            static const int64_t NULL_ID = -1;

            static int64_t newSpanId();

            static int64_t nextSpanId(int64_t spanId, int64_t parentSpanId);

        private:
            static int64_t createSpanId();

            static int64_t randInt64();
        };

        class TraceId
        {
        public:
#ifdef UNUSED_CODE
            TraceId(const std::string &agentId, int64_t agentStartTime, int64_t transactionSequence);
            TraceId getNextTraceId();
#endif

            TraceId(const std::string &agentId, int64_t agentStartTime, int64_t transactionSequence,
                    int64_t parentSpanId, int64_t spanId, int16_t flags);

            static TraceIdPtr parse(std::string transactionIdStr, int64_t parentSpanID, int64_t spanID, int16_t flags);

            const std::string &getAgentId() const
            {
                return agentId;
            }

            const int64_t getAgentStartTime() const
            {
                return agentStartTime;
            }

            const int64_t getTransactionSequence() const
            {
                return transactionSequence;
            }

            const int64_t getParentSpanId() const
            {
                return parentSpanId;
            }

            const int64_t getSpanId() const
            {
                return spanId;
            }

            const int16_t getFlags() const
            {
                return flags;
            }

            bool isRoot() const
            {
                return this->parentSpanId == SpanId::NULL_ID;
            }

            const std::string &getTransactionId() const
            {
                return this->transactionId;
            }

        private:
            const std::string agentId;
            const int64_t agentStartTime;
            const int64_t transactionSequence;
            const int64_t parentSpanId;
            const int64_t spanId;
            const int16_t flags;
            const std::string transactionId;
        };

        using Pinpoint::utils::get_current_unixsec_stamp;
        using Pinpoint::memory::SafeShareMemory;
        using Pinpoint::memory::REMOVE_WHEN_EXIT;
        class ProcessShareDataManager
        {

        private:
            typedef struct _procsSharefeData
            {
                int32_t iVersion;
                int64_t tSequenceGenerator;
                // API add here
                _procsSharefeData()
                {
                    tSequenceGenerator = 1;
                    iVersion = 0; // bread crumbs
                }
            } procsSharefeData;

            ProcessShareDataManager()
            {
                using Pinpoint::Agent::PinpointAgentContext;

                std::string shmname = "";

#ifdef UNITTEST
                shmname = shmname + "ProcessShareDataManager" + "-procsSharefeData-" + "0";
#else
                shmname = shmname + "ProcsSharefeData-"
                                        + boost::lexical_cast<std::string>(PinpointAgentContext::getContextPtr()->startTimestamp);
#endif
                pSShmFlag = boost::shared_ptr<SafeShareMemory<procsSharefeData> >(
                        new SafeShareMemory<procsSharefeData>(shmname.c_str()));
            }
            virtual ~ProcessShareDataManager(){}
        public:
            int64_t generateTranSequence();
            static ProcessShareDataManager* instance();
        private:
            boost::shared_ptr<SafeShareMemory<procsSharefeData> > pSShmFlag;
        };

    }
}

#endif
