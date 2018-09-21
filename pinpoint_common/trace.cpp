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
#include "trace.h"
#include "pinpoint_error.h"
#include "buffer.h"
#include "pinpoint_def.h"
#include "pinpoint_agent_context.h"
#include "pinpoint_agent.h"
#include "trace_data_sender.h"
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace Pinpoint::utils;
using namespace Pinpoint::log;
using namespace Pinpoint::Agent;

namespace Pinpoint
{
    namespace Trace
    {
        //<editor-fold desc="PinpointHttpHeader">

        PinpointHttpHeader PinpointHttpHeader::createHeader(const Plugin::HttpHeader* pHeader)
        {
            PinpointHttpHeader pinpointHttpHeader;
            pinpointHttpHeader.sampled = true;

            if (pHeader == NULL)
            {
                pinpointHttpHeader.type = NULL_HTTP_HEADER;
                assert (false);
                return pinpointHttpHeader;
            }

            std::string head;
            bool exist;

            /* 4, sampled */
            exist = pHeader->getHeader(Pinpoint::HTTP_SAMPLED, head);
            if (exist)
            {
                pinpointHttpHeader.sampled = head != Sampling::SAMPLING_RATE_FALSE;
                if (!pinpointHttpHeader.sampled)
                {
                    pinpointHttpHeader.type = VALID_HTTP_HEADER;
                    return pinpointHttpHeader;
                }
            }

            /* 1, trace id */
            exist = pHeader->getHeader(Pinpoint::HTTP_TRACE_ID, head);
            if (!exist)
            {
                pinpointHttpHeader.type = NULL_HTTP_HEADER;
                return pinpointHttpHeader;
            }

            pinpointHttpHeader.traceId = head;

            /* 2, spanid */
            exist = pHeader->getHeader(Pinpoint::HTTP_SPAN_ID, head);
            if (!exist)
            {
                pinpointHttpHeader.type = INVALID_HTTP_HEADER;
                return pinpointHttpHeader;
            }

            try
            {
                pinpointHttpHeader.spanId = FormatConverter::stringToInt64(head);
            }
            catch (...)
            {
                LOGE("get error spanId=%s", head.c_str());
                pinpointHttpHeader.type = INVALID_HTTP_HEADER;
                return pinpointHttpHeader;
            }

            /* 3, pSpanId */
            exist = pHeader->getHeader(Pinpoint::HTTP_PARENT_SPAN_ID, head);
            if (!exist)
            {
                pinpointHttpHeader.type = INVALID_HTTP_HEADER;
                return pinpointHttpHeader;
            }

            try
            {
                pinpointHttpHeader.pSpanId = FormatConverter::stringToInt64(head);
            }
            catch (...)
            {
                LOGE("get error pSpanId=%s", head.c_str());
                pinpointHttpHeader.type = INVALID_HTTP_HEADER;
                return pinpointHttpHeader;
            }


            /* 5, flag */
            exist = pHeader->getHeader(Pinpoint::HTTP_FLAGS, head);
            if (exist)
            {

                try
                {
                    pinpointHttpHeader.flag = (int16_t)FormatConverter::stringToInt64(head);
                }
                catch (...)
                {
                    LOGE("get error flag=%s", head.c_str());
                    pinpointHttpHeader.type = INVALID_HTTP_HEADER;
                    return pinpointHttpHeader;
                }
            }

            /* 5, parentApplicationName */
            exist = pHeader->getHeader(Pinpoint::HTTP_PARENT_APPLICATION_NAME, head);
            if (exist)
            {
                pinpointHttpHeader.parentApplicationName = head;

            }

            /* 6, parentApplicationType */
            exist = pHeader->getHeader(Pinpoint::HTTP_PARENT_APPLICATION_TYPE, head);
            if (exist)
            {

                try
                {
                    pinpointHttpHeader.parentApplicationType = (int16_t)FormatConverter::stringToInt64(head);
                }
                catch (...)
                {
                    LOGE("get error parentApplicationType=%s", head.c_str());
                    pinpointHttpHeader.type = INVALID_HTTP_HEADER;
                    return pinpointHttpHeader;
                }
            }

            /* 5, parentApplicationName */
            exist = pHeader->getHeader(Pinpoint::HTTP_HOST, head);
            if (exist)
            {
                pinpointHttpHeader.host = head;

            }

            pinpointHttpHeader.type = VALID_HTTP_HEADER;
            return pinpointHttpHeader;
        }
        //</editor-fold>

        //<editor-fold desc="TraceId">

        TraceId::TraceId(const std::string &agentId, int64_t agentStartTime, int64_t transactionSequence, int64_t parentSpanId,
                         int64_t spanId, int16_t flags)
                : agentId(agentId), agentStartTime(agentStartTime), transactionSequence(transactionSequence),
                  parentSpanId(parentSpanId), spanId(spanId), flags(flags),
                  transactionId(TransactionId::formatString(agentId, agentStartTime, transactionSequence))
        {

        }

#ifdef UNUSED_CODE
        TraceId::TraceId(const std::string &agentId, int64_t agentStartTime, int64_t transactionSequence)
                : agentId(agentId), agentStartTime(agentStartTime), transactionSequence(transactionSequence),
                  parentSpanId(SpanId::NULL_ID), spanId(SpanId::newSpanId()), flags(0),
                  transactionId(TransactionId::formatString(agentId, agentStartTime, transactionSequence))
        {

        }

        TraceId TraceId::getNextTraceId()
        {
            return TraceId(this->agentId, this->agentStartTime,
                           this->transactionSequence, this->spanId,
                           SpanId::nextSpanId(this->spanId, this->parentSpanId),
                           flags);
        }
#endif

        TraceIdPtr TraceId::parse(std::string transactionIdStr, int64_t parentSpanID, int64_t spanID, int16_t flags)
        {
            TraceId* pTraceId = NULL;
            try
            {
                const TransactionId transactionId = TransactionId::parseTransactionId(transactionIdStr);
                pTraceId = new TraceId(transactionId.getAgentId(), transactionId.getAgentStartTime(),
                                       transactionId.getTransactionSequence(), parentSpanID, spanID, flags);
            }
            catch (std::invalid_argument&)
            {
                LOGE("catch invalid_argument");
                pTraceId = NULL; 
            }
            catch (std::bad_alloc&)
            {
                LOGE("new traceId failed.");
                pTraceId = NULL;
            }

            return TraceIdPtr(pTraceId);
        }

        //</editor-fold>

		//<editor-fold desc="TranSequenceGenerator">
		ProcessShareDataManager* ProcessShareDataManager::instance()
		{
			static ProcessShareDataManager instance;
			return &instance;
		}

		int64_t ProcessShareDataManager::generateTranSequence()
		{
			Pinpoint::ScopeLock _lock(&pSShmFlag->getWriteLock());
			procsSharefeData* pData = pSShmFlag->getData();

			pData->iVersion++;
			pData->tSequenceGenerator++;

			return pData->tSequenceGenerator;
		}
		//</editor-fold>

        //<editor-fold desc="Trace">

        class TraceImp
        {
        public:
            static int32_t startTrace(const Plugin::HttpHeader *pHeader);

            static int32_t endTrace();

            static TracePtr getCurrentTrace();

            static bool isStarted();
        private:
            static boost::thread_specific_ptr<TracePtr> currentTracePtrTls;
        };

        boost::thread_specific_ptr<TracePtr> TraceImp::currentTracePtrTls;
        SpanEventRecorderPtr Trace::NULL_SPAN_EVENT_RECODER_PTR;

        TracePtr TraceImp::getCurrentTrace()
        {
            if (TraceImp::currentTracePtrTls.get() == NULL)
            {
                return TracePtr();
            }
            return *TraceImp::currentTracePtrTls;
        }

        int32_t TraceImp::startTrace(const Plugin::HttpHeader* pHeader)
        {
            PinpointHttpHeader header = PinpointHttpHeader::createHeader(pHeader);

            if (header.type == INVALID_HTTP_HEADER)
            {
                LOGW("get invalid http header");
                return INVALID_PINPOINT_HTTP_HEADER;
            }

            if (!header.sampled)
            {
                LOGI("upstream sample ignore!");
                return SAMPLING_IGNORE;
            }

            Agent::AgentPtr& agentPtr = Agent::Agent::getAgentPtr();
            PINPOINT_ASSERT_RETURN ((agentPtr != NULL), FAILED);
            Agent::SamplingPtr& samplingPtr = agentPtr->getSamplingPtr();
            samplingPtr->startNewTrace(header.type == VALID_HTTP_HEADER);
            if (!samplingPtr->checkTraceLimit())
            {
                LOGI("skip trace: checkTraceLimit");
                return SAMPLING_IGNORE;
            }

            if (TraceImp::currentTracePtrTls.get() != NULL)
            {
                TracePtr *tracePtrPtr = TraceImp::currentTracePtrTls.get();
                if (!(*tracePtrPtr)->isCodeError())
                {
                    LOGE("trace is not completed!!!");
                    assert (false);
                }
                TraceImp::currentTracePtrTls.reset();
            }

            TraceIdPtr traceIdPtr;
            bool isContinued = false;

            if (header.type == VALID_HTTP_HEADER)
            {
                // continue to trace
                traceIdPtr = TraceId::parse(header.traceId, header.pSpanId,
                                            header.spanId, header.flag);
                isContinued = true;
            }
            else
            {
                PINPOINT_ASSERT_RETURN ((header.type == NULL_HTTP_HEADER), FAILED);
                // new trace
                try
                {

                    Agent::PinpointAgentContextPtr contextPtr =
                            Agent::PinpointAgentContext::getContextPtr();

                    PINPOINT_ASSERT_RETURN((contextPtr != NULL), FAILED);

                    traceIdPtr.reset(
                            new TraceId(contextPtr->agentId, (int64_t) contextPtr->startTimestamp,
                                        ProcessShareDataManager::instance()->generateTranSequence(),
                                        SpanId::NULL_ID, SpanId::newSpanId(), 0));
                } catch (std::exception& exception)
                {
                    LOGE("new traceId failed. e=%s", exception.what());
                    return FAILED;
                }
            }

            try
            {
                SpanPtr spanPtr(new Span());
                if (isContinued)
                {
                    spanPtr->recordTraceId(traceIdPtr, header);
                }
                else
                {
                    spanPtr->recordTraceId(traceIdPtr);
                }
                spanPtr->markBeforeTime();
                Trace *trace = new DefaultTrace(header, traceIdPtr, spanPtr, isContinued);
                TracePtr *tracePtrPtr = new TracePtr(trace);
                TraceImp::currentTracePtrTls.reset(tracePtrPtr);

#ifdef TEST_SIMULATE
                std::string testCasePath;
                if (pHeader->getHeader(Pinpoint::HTTP_TESTCASE_PATH, testCasePath))
                {
                    AnnotationPtr annotationPtr = Annotation::createAnnotation(
                            Pinpoint::Trace::AnnotationKey::TESTCASE_PATH);
                    annotationPtr->addStringValue(testCasePath);
                    trace->addAnnotationPtr(annotationPtr);
                }
#endif
            }
            catch (std::exception& exception)
            {
                LOGE("new trace failed. e=%s", exception.what());
                return FAILED;
            }

            return SUCCESS;
        }


        int32_t TraceImp::endTrace()
        {
            if (TraceImp::currentTracePtrTls.get() == NULL)
            {
                assert (false);
                return FAILED;
            }

            int32_t err = SUCCESS;

            TracePtr *tracePtrPtr = TraceImp::currentTracePtrTls.get();
            try
            {
                TracePtr tracePtr(*tracePtrPtr);
                tracePtr->finishTrace();

                Agent::AgentPtr agentPtr = Agent::Agent::getAgentPtr();
                if (agentPtr != NULL)
                {
                    SamplingPtr &samplingPtr = agentPtr->getSamplingPtr();
                    int32_t traceElapse = tracePtr->getTraceElapse();
                    if (samplingPtr->checkSkipTraceTime(traceElapse))
                    {
                        agentPtr->sendTrace(tracePtr);
                    }
                    else
                    {
                        LOGI("skip trace: traceElapse=%d", traceElapse);
                    }
                }
            }
            catch (std::exception& e)
            {
                LOGE("TraceImp::endTrace() throw, e=%s", e.what());
                assert (false);
                err = FAILED;
            }

            // we release trace when next request start if error happen.
            // The purpose is for checking error happening
            if (!(*tracePtrPtr)->isCodeError())
            {
                TraceImp::currentTracePtrTls.reset();
            }

            return err;
        }

        bool TraceImp::isStarted()
        {
            return TraceImp::currentTracePtrTls.get() != NULL;
        }

        TracePtr Trace::getCurrentTrace()
        {
            return TraceImp::getCurrentTrace();
        }

        int32_t Trace::startTrace(const Plugin::HttpHeader *pHeader)
        {
            return TraceImp::startTrace(pHeader);
        }

        int32_t Trace::endTrace()
        {
            return TraceImp::endTrace();
        }

        bool Trace::isStarted()
        {
            return TraceImp::isStarted();
        }

        Trace::~Trace()
        {

        }


        //</editor-fold>

        //<editor-fold desc="DefaultTrace">

        DefaultTrace::DefaultTrace(const PinpointHttpHeader& header, const TraceIdPtr &traceIdPtr,
                                   const SpanPtr& spanPtr, bool isContinued)
                : traceIdPtr(traceIdPtr), spanPtr(spanPtr), isContinued(isContinued),
                  header(header), isBad(false), isCodeError_(false)
        {
            try
            {
                spanEventPtrList.reserve(DEFAULT_SPAN_EVENT_NUM);
                annotationPtrList.reserve(DEFAULT_ANNOTATION_NUM);
            }
            catch (std::bad_alloc&)
            {

            }

            if(isContinued == true){
                setAcceptorHost(header.host);
            }

        }

        void DefaultTrace::markBadTrace()
        {
            LOGE("mark bad trace !!! check your plugins...");
            // bad trace: clean up all events ...
            while (!callStack.empty())
            {
                callStack.pop();
            }
            isBad = true;
        }

        bool DefaultTrace::isBadTrace()
        {
            return isBad;
        }

        bool DefaultTrace::isCodeError()
        {
            return this->isCodeError_;
        }

        void DefaultTrace::markCodeError()
        {
            this->isCodeError_ = true;
        }

        DefaultTrace::~DefaultTrace()
        {
        }

        const std::string& DefaultTrace::getTransactionId() const
        {
            return traceIdPtr->getTransactionId();
        }

        SpanEventRecorderPtr DefaultTrace::getSpanEventRecorderPtr(uint64_t callId)
        {
            SpanEventPtrMap::iterator ip = spanEventPtrMap.find(callId);
            if (ip != spanEventPtrMap.end())
            {
                return boost::dynamic_pointer_cast<SpanEventRecorder>(ip->second);
            }

            return SpanEventRecorderPtr();
        }

        void DefaultTrace::setApiId(int32_t apiId)
        {
            spanPtr->setApiId(apiId);
        }

        const TraceIdPtr& DefaultTrace::getTraceIdPtr() const
        {
            return this->traceIdPtr;
        }

        SpanPtr& DefaultTrace::getSpanPtr()
        {
            return this->spanPtr;
        }

        void DefaultTrace::completeTrace()
        {
            vector<TAnnotation> tAnnotationVector;
            tAnnotationVector.reserve(this->annotationPtrList.size());

            for(vector<AnnotationPtr>::iterator ip = annotationPtrList.begin();
                ip != annotationPtrList.end(); ++ip)
            {

                DefaultAnnotationPtr defaultAnnotationPtr =
                        boost::dynamic_pointer_cast<DefaultAnnotation>(*ip);

                tAnnotationVector.push_back(defaultAnnotationPtr->getTAnnotation());
            }
            spanPtr->setAnnotationList(tAnnotationVector);


            vector<TSpanEvent> tSpanEventVector;
            tSpanEventVector.reserve(this->spanEventPtrList.size());
            for (vector<SpanEventPtr>::iterator ip = spanEventPtrList.begin();
                 ip != spanEventPtrList.end(); ++ip)
            {
                SpanEventPtr& spanEventPtr = (*ip);
                tSpanEventVector.push_back(spanEventPtr->getTSpanEvent());
            }

            spanPtr->setSpanEventList(tSpanEventVector);
        }

        SpanEventRecorderPtr DefaultTrace::traceBlockBegin(uint64_t callId)
        {
            SpanEventRecorderPtr spanEventRecorderPtr;
            try
            {
                SpanEventPtr spanEventPtr(new SpanEvent(spanPtr));
                spanEventPtr->recordTraceId(traceIdPtr);
                spanEventRecorderPtr = boost::dynamic_pointer_cast<SpanEventRecorder>(spanEventPtr);
                (void)callStack.push(spanEventPtr);

                if (spanEventPtrMap.count(callId) != 0)
                {
                    assert(false);
                }

                spanEventPtrMap[callId] = spanEventPtr;
            }
            catch (std::bad_alloc&)
            {
                LOGE("create spanEventRecorderPtr failed!");
                return spanEventRecorderPtr;
            }

            return spanEventRecorderPtr;
        }

        void DefaultTrace::traceBlockEnd(const SpanEventRecorderPtr &spanEventRecorderPtr)
        {
            if (this->isBad)
            {
                return;
            }

            SpanEventPtr spanEventPtr = callStack.pop();

            PINPOINT_ASSERT(spanEventPtr != NULL);

            if (spanEventRecorderPtr != NULL)
            {
                if(spanEventPtr.get() != spanEventRecorderPtr.get())
                {
                    // todo: maybe someone call before and forget to call end, try to recover stack
                    LOGE("get bad stack!!! try to recover it.");
                    while(!callStack.empty())
                    {
                        spanEventPtr = callStack.pop();
                        if (spanEventPtr.get() == spanEventRecorderPtr.get())
                        {
                            break;
                        }
                    }

                    if (spanEventPtr == NULL)
                    {
                        // can not recover, mark bad
                        LOGE("can not recover stack!!! mark bad trace.");
                        this->markBadTrace();
                        return;
                    }
                }
            }

            spanEventPtr->completeSpanEvent();

            spanEventPtrList.push_back(spanEventPtr);
        }

        void DefaultTrace::setEndPoint(const std::string& endPoint)
        {
            spanPtr->setEndPoint(endPoint);
        }

        void DefaultTrace::setRemoteAddr(const std::string &remoteAddr)
        {
            spanPtr->setRemoteAddr(remoteAddr);
        }

        void DefaultTrace::setAcceptorHost(const std::string &acceptorHost)
        {
            spanPtr->setAcceptorHost(acceptorHost);
        }

        void DefaultTrace::setRpc(const std::string &rpc)
        {
            spanPtr->setRpc(rpc);
        }

        void DefaultTrace::setExceptionInfo(int32_t errId, const std::string &errMsg)
        {
            spanPtr->setExceptionInfo(errId, errMsg);
        }

        void DefaultTrace::setErr(int32_t err)
        {
            spanPtr->setErr(err);
        }

        void DefaultTrace::addAnnotationPtr(const AnnotationPtr &annotationPtr)
        {
            annotationPtrList.push_back(annotationPtr);
        }

        void DefaultTrace::finishTrace()
        {
            PINPOINT_ASSERT (spanPtr != NULL);
            spanPtr->markAfterTime();
            this->completeTrace();
        }

        int32_t DefaultTrace::getTraceElapse()
        {
            PINPOINT_ASSERT_RETURN (spanPtr != NULL, 0);
            PINPOINT_ASSERT_RETURN (spanPtr->getTSpan().__isset.elapsed, 0);
            return spanPtr->getTSpan().elapsed;
        }

        SpanEventRecorderPtr DefaultTrace::getCurrentTraceBlock()
        {
            return boost::dynamic_pointer_cast<SpanEventRecorder>(callStack.peek());
        }

        int32_t DefaultTrace::getNextSpanInfo(Plugin::HeaderMap &nextHeader, int64_t &nextSpanId)
        {
            nextHeader.clear();
            nextSpanId = SpanId::nextSpanId(traceIdPtr->getSpanId(), traceIdPtr->getParentSpanId());

            nextHeader[Pinpoint::HTTP_TRACE_ID] = TransactionId::formatString(traceIdPtr->getAgentId(),
                                                                              traceIdPtr->getAgentStartTime(),
                                                                              traceIdPtr->getTransactionSequence());

            try
            {
                nextHeader[Pinpoint::HTTP_PARENT_SPAN_ID] = FormatConverter::int64ToString(traceIdPtr->getSpanId());
                nextHeader[Pinpoint::HTTP_SPAN_ID] = FormatConverter::int64ToString(nextSpanId);
            }
            catch (...)
            {
                LOGE("convert parentSpanId=%ld, spanId=%ld", traceIdPtr->getSpanId(), nextSpanId);
                return FAILED;
            }


            nextHeader[Pinpoint::HTTP_FLAGS] = "0";
            nextHeader[Pinpoint::HTTP_SAMPLED] = Pinpoint::Agent::Sampling::SAMPLING_RATE_TRUE;

            Agent::PinpointAgentContextPtr contextPtr = Agent::PinpointAgentContext::getContextPtr();

            PINPOINT_ASSERT_RETURN (contextPtr != NULL, FAILED);

            nextHeader[Pinpoint::HTTP_PARENT_APPLICATION_NAME] = contextPtr->applicationName;
            try
            {
                nextHeader[Pinpoint::HTTP_PARENT_APPLICATION_TYPE] = FormatConverter::int64ToString(contextPtr->serviceType);
            }
            catch (...)
            {
                LOGE("convert service=%d failed", contextPtr->serviceType);
                return FAILED;
            }

//            nextHeader[Pinpoint::HTTP_HOST] = contextPtr->hostname;

            return SUCCESS;
        }

        //</editor-fold>

        //<editor-fold desc="CallStack">

        CallStack::CallStack()
                : sequence(0), index(0), latestStackIndex(0)
        {
            try
            {
                stack.reserve(DEFAULT_STACK_LEN);
            }
            catch (std::bad_alloc)
            {

            }

        }

        int32_t CallStack::push(SpanEventPtr &spanEventPtr)
        {
            spanEventPtr->setSequence(sequence++);
            index++;
            stack.push_back(spanEventPtr);
            if(latestStackIndex != index)
            {
                latestStackIndex = index;
                spanEventPtr->setDepth(latestStackIndex);
            }

            return index - 1;
        }

        SpanEventPtr CallStack::pop()
        {
            if (stack.empty())
            {
                LOGE("try to pop empty stack!");
                assert(false);
                return SpanEventPtr();
            }

            index--;
            SpanEventPtr spanEventPtr = stack.back();
            stack.pop_back();
            return spanEventPtr;
        }

        SpanEventPtr CallStack::peek()
        {
            if (stack.empty())
            {
                return SpanEvent::NULL_SPAN_EVENT_PTR;
            }
            return stack.back();
        }

        SpanEventPtr CallStack::get(int32_t index)
        {
            if (index < 0 || index > (int32_t)this->stack.size() - 1)
            {
                return SpanEvent::NULL_SPAN_EVENT_PTR;
            }

            return stack[index];
        }

        bool CallStack::empty()
        {
            return stack.empty();
        }

        //</editor-fold>

        //<editor-fold desc="SpanEvent">

        const SpanEventPtr SpanEvent::NULL_SPAN_EVENT_PTR;

        SpanEvent::SpanEvent(const SpanPtr &spanPtr)
                : spanPtr(spanPtr)
        {
            PINPOINT_ASSERT (spanPtr != NULL);
            try
            {
                annotationPtrList.reserve(DEFAULT_ANNOTATION_NUM);
            }
            catch (std::bad_alloc&)
            {

            }

        }

        void SpanEvent::recordTraceId(const TraceIdPtr &traceIdPtr)
        {
            PINPOINT_ASSERT (traceIdPtr != NULL);
            tSpanEvent.__set_spanId(traceIdPtr->getSpanId());
        }

        void SpanEvent::markBeforeTime()
        {
            tSpanEvent.__set_startElapsed(utils::get_current_microsec_stamp64() - spanPtr->getBeforeTime());
        }

        void SpanEvent::markAfterTime()
        {
            tSpanEvent.__set_endElapsed(utils::get_current_microsec_stamp64() - spanPtr->getBeforeTime());
        }

        void SpanEvent::setApiId(int32_t apiId)
        {
            tSpanEvent.__set_apiId(apiId);
        }

        void SpanEvent::setServiceType(int16_t serviceType)
        {
            tSpanEvent.__set_serviceType(serviceType);
        }

        void SpanEvent::addAnnotationPtr(const AnnotationPtr &annotationPtr)
        {
            annotationPtrList.push_back(annotationPtr);
        }

        void SpanEvent::setEndPoint(const std::string &endPoint)
        {
            tSpanEvent.__set_endPoint(endPoint);
        }

        void SpanEvent::setRpc(const std::string &rpc)
        {
            tSpanEvent.__set_rpc(rpc);
        }

        void SpanEvent::setExceptionInfo(int32_t errId, const std::string &errMsg)
        {
            TIntStringValue value;
            value.__set_intValue(errId);
            value.__set_stringValue(errMsg);
            tSpanEvent.__set_exceptionInfo(value);
        }

        void SpanEvent::setNextSpanId(int64_t nextSpanId)
        {
            tSpanEvent.__set_nextSpanId(nextSpanId);
        }

        void SpanEvent::setDestinationId(const std::string &destinationId)
        {
            tSpanEvent.__set_destinationId(destinationId);
        }

        const TSpanEvent& SpanEvent::getTSpanEvent() const 
        {
            return tSpanEvent;
        }

        void SpanEvent::setSequence(int16_t sequence)
        {
            tSpanEvent.__set_sequence(sequence);
        }

        void SpanEvent::setDepth(int32_t depth)
        {
            tSpanEvent.__set_depth(depth);
        }

        void SpanEvent::completeSpanEvent()
        {
            vector<TAnnotation> tAnnotationVector;

            try
            {

                tAnnotationVector.reserve(this->annotationPtrList.size());

                for (vector<AnnotationPtr>::iterator ip = annotationPtrList.begin();
                     ip != annotationPtrList.end(); ++ip)
                {

                    DefaultAnnotationPtr defaultAnnotationPtr =
                            boost::dynamic_pointer_cast<DefaultAnnotation>(*ip);

                    tAnnotationVector.push_back(defaultAnnotationPtr->getTAnnotation());
                }

                tSpanEvent.__set_annotations(tAnnotationVector);
            }
            catch (std::bad_alloc& e)
            {
                LOGE("panEvent::completeSpanEvent. e=%s", e.what());
            }
            catch (std::exception& e)
            {
                LOGE("panEvent::completeSpanEvent. e=%s", e.what());
                assert(false);
            }
        }

        //</editor-fold>

        //<editor-fold desc="SpanEventRecorder">

        const SpanEventRecorderPtr SpanEventRecorder::NULL_SPAN_EVENT_RECORDER_PTR;
        //</editor-fold>


        //<editor-fold desc="TransactionId">

        bool operator==(const TransactionId &a, const TransactionId &b)
        {
            return (a.agentId == b.agentId) && \
			(a.agentStartTime == b.agentStartTime) && \
			(a.transactionSequence == b.transactionSequence);
        }

        // int8_t TransactionIdUtils::TVERSION(0);
        const string TransactionId::TRANSACTION_ID_DELIMITER("^");


        string TransactionId::formatString(const string& agentId, const int64_t agentStartTime, int64_t transactionSequence)
        {
            stringstream fmt;

            if (agentId.empty())
            {
                LOGE("agentId must not be null");
                assert (false);
                return "";
            }

            fmt << agentId;
            fmt << TransactionId::TRANSACTION_ID_DELIMITER;
            fmt << agentStartTime;
            fmt << TransactionId::TRANSACTION_ID_DELIMITER;
            fmt << transactionSequence;
            return fmt.str();
        }

        string TransactionId::formatString(const TransactionId& transactionId)
        {
            return formatString(transactionId.agentId, transactionId.agentStartTime, transactionId.transactionSequence);
        }

        vector<int8_t> TransactionId::formatBytes(const string& agentId, int64_t agentStartTime, int64_t transactionSequence)
        {
            AutoBuffer buffer;
            buffer.putByte(TVERSION);
            buffer.putPrefixedString(agentId.c_str());
            buffer.putVar64(agentStartTime);
            buffer.putVar64(transactionSequence);
            return buffer.getBuffer();
        }

        vector<int8_t> TransactionId::formatBytes(const TransactionId& transactionId)
        {
            return formatBytes(transactionId.agentId, transactionId.agentStartTime, transactionId.transactionSequence);
        }

        TransactionId TransactionId::parseTransactionId(const std::vector<int8_t>& transactionId) 
        {
            AutoBuffer buffer(transactionId);
            const int8_t version = buffer.readByte();
            if (version != TVERSION)
            {
                LOGE("bad version: version = %d", version);
                throw invalid_argument("bad version");
            }

            char* agentId = buffer.readPrefixedString();
            int64_t agentStartTime = buffer.readVarLong();
            int64_t transactionSequence = buffer.readVarLong();

            string agentIdString("");
            if (agentId != NULL)
            {
                agentIdString += agentId;
                ::free(agentId);
                agentId = NULL;
            }
            return TransactionId(agentIdString, agentStartTime,transactionSequence);
        }

        TransactionId TransactionId::parseTransactionId(const string& transactionId) 
        {
            string::size_type start = 0;
            string::size_type index = 0;

            /* agentId */
            index = transactionId.find_first_of("^", start);
            if (index == string::npos)
            {
                LOGW("invalid transactionId = %s", transactionId.c_str());
                throw invalid_argument(string("") + "invalid transactionId = " + transactionId);
            }
            string agentId = transactionId.substr(start, index-start);

            /* agentStartTime */
            start = index + 1;
            index = transactionId.find_first_of("^", start);
            if (index == string::npos)
            {
                LOGW("invalid transactionId = %s", transactionId.c_str());
                throw invalid_argument(string("") + "invalid transactionId = " + transactionId);
            }

            int64_t agentStartTime = FormatConverter::stringToInt64(transactionId.substr(start, index-start));

            /* transactionSequence */
            start = index + 1;
            index = transactionId.find_first_of("^", start);
            if (index != string::npos)
            {
                LOGW("invalid transactionId = %s", transactionId.c_str());
                throw invalid_argument(string("") + "invalid transactionId = " + transactionId);
            }

            int64_t transactionSequence = FormatConverter::stringToInt64(transactionId.substr(start).c_str());

            return TransactionId(agentId, agentStartTime, transactionSequence);
        }

        bool TransactionId::isValidTransactionIdStr(const std::string &transactionId)
        {
            try
            {
                parseTransactionId(transactionId);
            }
            catch (std::invalid_argument&)
            {
                return false;
            }
            return true;
        }

        //</editor-fold>

        //<editor-fold desc="SpanId">

        int64_t SpanId::randInt64()
        {
            int32_t high = rand();
            int32_t low = rand();
            return ((int64_t)high << 32) | low;
        }

        int64_t SpanId::createSpanId()
        {
            int64_t spanId = randInt64();
            while(spanId == NULL_ID)
            {
                spanId = randInt64();
            }
            return spanId;
        }

        int64_t SpanId::newSpanId()
        {
            return createSpanId();
        }

        int64_t SpanId::nextSpanId(int64_t spanId, int64_t parentSpanId)
        {
            int64_t nextSpanId = createSpanId();
            while(nextSpanId == spanId || nextSpanId == parentSpanId)
            {
                nextSpanId = createSpanId();
            }

            return nextSpanId;
        }
        //</editor-fold>

        //<editor-fold desc="Span">

        void Span::recordTraceId(const TraceIdPtr &traceIdPtr)
        {

            Agent::PinpointAgentContextPtr contextPtr = Agent::PinpointAgentContext::getContextPtr();

            PINPOINT_ASSERT (contextPtr != NULL);

            TransactionId transactionId = TransactionId(traceIdPtr->getAgentId(),
                                                        traceIdPtr->getAgentStartTime(),
                                                        traceIdPtr->getTransactionSequence());

            std::vector<int8_t> transactionIdBytes = TransactionId::formatBytes(transactionId);
            tSpan.__set_transactionId(string(transactionIdBytes.begin(), transactionIdBytes.end()));


            tSpan.__set_agentId(contextPtr->agentId);
            tSpan.__set_applicationName(contextPtr->applicationName);
            tSpan.__set_agentStartTime((int64_t)contextPtr->startTimestamp);
            tSpan.__set_serviceType(contextPtr->serviceType);
            tSpan.__set_spanId(traceIdPtr->getSpanId());
            tSpan.__set_parentSpanId(traceIdPtr->getParentSpanId());
            tSpan.__set_flag(traceIdPtr->getFlags());
        }

        void Span::recordTraceId(const TraceIdPtr &traceIdPtr, const PinpointHttpHeader& header)
        {
            PINPOINT_ASSERT (header.type == VALID_HTTP_HEADER);
            tSpan.__set_parentApplicationName(header.parentApplicationName);
            tSpan.__set_parentApplicationType(header.parentApplicationType);
            recordTraceId(traceIdPtr);
        }

        void Span::markBeforeTime()
        {
            tSpan.__set_startTime((int64_t)utils::get_current_microsec_stamp64());
        }

        void Span::markAfterTime()
        {
            int32_t after = (int32_t)(utils::get_current_microsec_stamp64() - tSpan.startTime);

            if (after != 0)
            {
                tSpan.__set_elapsed(after);
            }
        }

        int64_t Span::getAfterTime() const 
        {
            return tSpan.startTime + tSpan.elapsed;
        }

        int64_t Span::getBeforeTime() const 
        {
            return tSpan.startTime;
        }

        void Span::setApiId(int32_t apiId)
        {
            tSpan.__set_apiId(apiId);
        }

        void Span::setEndPoint(const std::string& endPoint)
        {
            return tSpan.__set_endPoint(endPoint);
        }

        void Span::setRemoteAddr(const std::string &remoteAddr)
        {
            return tSpan.__set_remoteAddr(remoteAddr);
        }

        void Span::setAcceptorHost(const std::string &acceptorHost)
        {
            return tSpan.__set_acceptorHost(acceptorHost);
        }

        void Span::setRpc(const std::string &rpc)
        {
            return tSpan.__set_rpc(rpc);
        }

        void Span::setSpanEventList(const std::vector<TSpanEvent> &tSpanEventVector)
        {
            tSpan.__set_spanEventList(tSpanEventVector);
        }

        void Span::setAnnotationList(const std::vector<TAnnotation> &tAnnotationVector)
        {
            tSpan.__set_annotations(tAnnotationVector);
        }

        void Span::setExceptionInfo(int32_t errId, const std::string &errMsg)
        {
            TIntStringValue value;
            value.__set_intValue(errId);
            value.__set_stringValue(errMsg);
            tSpan.__set_exceptionInfo(value);
        }

        void Span::setErr(int32_t err)
        {
            tSpan.__set_err(err);
        }

        const TSpan& Span::getTSpan() const 
        {
            return this->tSpan;
        }

        //</editor-fold>

        //<editor-fold desc="Annotation">

        AnnotationPtr Annotation::createAnnotation(int32_t key)
        {
            AnnotationPtr annotationPtr;

            try
            {
                annotationPtr.reset(new DefaultAnnotation(key));
            }
            catch (std::bad_alloc&)
            {
                LOGE("create DefaultAnnotation failed");
            }

            return annotationPtr;
        }
        //</editor-fold>

        //<editor-fold desc="DefaultAnnotation">

        DefaultAnnotation::DefaultAnnotation(int32_t key)
        {
            tAnnotation.__set_key(key);
        }

        const TAnnotation& DefaultAnnotation::getTAnnotation() const 
        {
            return tAnnotation;
        }

        void DefaultAnnotation::addBoolValue(bool b)
        {
            TAnnotationValue value;
            value.__set_boolValue(b);
            tAnnotation.__set_value(value);
        }

        void DefaultAnnotation::addIntValue(int32_t value)
        {
            TAnnotationValue v;
            v.__set_intValue(value);
            tAnnotation.__set_value(v);
        }

        void DefaultAnnotation::addBinaryValue(const std::string &v)
        {
            TAnnotationValue value;
            value.__set_binaryValue(v);
            tAnnotation.__set_value(value);
        }

        void DefaultAnnotation::addDoubleValue(double d)
        {
            TAnnotationValue value;
            value.__set_doubleValue(d);
            tAnnotation.__set_value(value);
        }

        void DefaultAnnotation::addLongValue(int64_t value)
        {
            TAnnotationValue v;
            v.__set_longValue(value);
            tAnnotation.__set_value(v);
        }

        void DefaultAnnotation::addShortValue(int16_t value)
        {
            TAnnotationValue v;
            v.__set_shortValue(value);
            tAnnotation.__set_value(v);
        }

        void DefaultAnnotation::addStringValue(const std::string &s)
        {
            TAnnotationValue v;
            v.__set_stringValue(s);
            tAnnotation.__set_value(v);
        }

        void DefaultAnnotation::addByteValue(int8_t value)
        {
            TAnnotationValue v;
            v.__set_byteValue(value);
            tAnnotation.__set_value(v);
        }

        void DefaultAnnotation::addIntStringStringValue(int32_t intValue, const std::string &s1, const std::string &s2)
        {
            TAnnotationValue v;
            TIntStringStringValue value;
            value.__set_intValue(intValue);
            value.__set_stringValue1(s1);
            value.__set_stringValue2(s2);
            v.__set_intStringStringValue(value);
            tAnnotation.__set_value(v);
        }

        void DefaultAnnotation::addIntStringValue(int32_t intValue, const std::string &s)
        {
            TAnnotationValue v;
            TIntStringValue value;
            value.__set_intValue(intValue);
            value.__set_stringValue(s);
            v.__set_intStringValue(value);
            tAnnotation.__set_value(v);
        }




        void DefaultAnnotation::addTLongIntIntByteByteStringValue(std::string &value,int type)
        {
            TAnnotationValue v;
            TLongIntIntByteByteStringValue tvalue;

            LOGD("addTLongIntIntByteByteStringValue %d %s ",type,value.c_str());

//           Pinpoint-ProxyApache:  t=991424704447256 D=3775428 i=51 b=49
//           Pinpoint-ProxyNginx:   t=1504248328.423 D=0.123
//           Pinpoint-ProxyApp:     t=1502861340 app=foo-bar


#define tSTR    "t="
#define appSTR  "app="
#define DSTR    "D="
#define iSTR    "i="
#define bSTR    "b="

#define PARSE_STR(name) ((pvalue = strstr(value.c_str(),(name))) != NULL && (pvalue += strlen(name)) != NULL)

            const char* pvalue=NULL;

            if(PARSE_STR(tSTR))
            {
                tvalue.__set_longValue(atol(pvalue));
            }

            tvalue.__set_intValue1(type);


            switch(type){
            case TYPE_APP:
                {
                    if(PARSE_STR(appSTR))
                    {
                        std::string app = std::string(pvalue); // if NULL app is null
                        boost::trim(app);
                        tvalue.__set_stringValue(app);
                    }
                }
                break;
            case TYPE_NGINX:
                {
                    if(PARSE_STR(DSTR))
                    {
                        tvalue.__set_intValue2(atoi(pvalue));
                    }
                }
                break;
            case TYPE_APACHE:
                {
                    if(PARSE_STR(iSTR))
                    {
                        tvalue.__set_byteValue1((int8_t)atoi(pvalue));
                    }

                    if (PARSE_STR(bSTR))
                    {
                        tvalue.__set_byteValue2((int8_t) atoi(pvalue));
                    }

                    if(PARSE_STR(DSTR))
                    {
                        tvalue.__set_intValue2(atoi(pvalue));
                    }

                }
                break;
            default:return ;
            }

            v.__set_longIntIntByteByteStringValue(tvalue);
            tAnnotation.__set_value(v);
        }
        //</editor-fold>
    }
}











