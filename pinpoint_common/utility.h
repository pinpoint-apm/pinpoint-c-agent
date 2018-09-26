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
#ifndef PINPOINT_UTILITY_H
#define PINPOINT_UTILITY_H

#include "stdint.h"
#include <sstream>
#include <boost/thread.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/interprocess/detail/os_thread_functions.hpp>
#include <boost/noncopyable.hpp>
#include <pthread.h>
#include <list>
#include <vector>
#include "thrift/gen-cpp/Pinpoint_types.h"
#include "thrift/gen-cpp/Trace_types.h"
#include "pinpoint_api.h"

typedef boost::shared_ptr<apache::thrift::TBase> TBasePtr;
typedef boost::shared_ptr<TAgentInfo> TAgentInfoPtr;
typedef boost::shared_ptr<TApiMetaData> TApiMetaDataPtr;
typedef boost::shared_ptr<TStringMetaData> TStringMetaDataPtr;
typedef boost::shared_ptr<TSpan> TSpanPtr;
typedef boost::shared_ptr<TAgentStatBatch> TAgentStatBatchPtr;

/** number of microseconds since 00:00:00 January 1, 1970 UTC */
#define MICROSECOND (1) /* use microsecond as the smallest unit. */
#define SECOND (1000*MICROSECOND)


namespace Pinpoint
{
    namespace utils
    {
        extern int64_t double_to_long_bits(double value);
        extern double long_bits_to_double(int64_t value);
        extern int64_t dotsec_to_milisec(const char*);


        extern int32_t safe_strlen(const char* str);

        extern bool Base64Encode(std::string& outPut, const std::string& inPut);
        extern bool Base64Decode(std::string& outPut, const std::string& inPut);

        extern std::string trim(const std::string& str);

        template<typename T>
        class FixedLengthQueue
        {
        public:
            explicit FixedLengthQueue(uint32_t len) :
                    m_queue(len), m_len(len)
            {

            }

            int32_t front(T& t)
            {
                if (m_queue.empty())
                {
                    return QUEUE_EMPTY;
                }

                t = m_queue.front();
                return SUCCESS;
            }

            int32_t pop_front(T& t)
            {
                int32_t err = front(t);
                if (err == QUEUE_EMPTY)
                {
                    return err;
                }

                m_queue.pop_front();
                return SUCCESS;
            }

            int32_t push_front(const T& t)
            {
                if (m_queue.full())
                {
                    return QUEUE_FULL;
                }

                m_queue.push_front(t);
                return SUCCESS;
            }

            int32_t push_back(const T& t)
            {
                if (m_queue.full())
                {
                    return QUEUE_FULL;
                }

                m_queue.push_back(t);
                return SUCCESS;
            }

            uint32_t size()
            {
                return (uint32_t)m_queue.size();
            }

            uint32_t capacity()
            {
                return m_len;
            }

            bool empty()
            {
                return m_queue.empty();
            }

            bool full()
            {
                return m_queue.full();
            }

        private:
            boost::circular_buffer<T> m_queue;
            uint32_t m_len;
        };

        template <class T> std::string TBaseToString(const T& tbase)
        {
            try
            {
                PStream pStream = FormatConverter::getPStream();
                *pStream << tbase;
                return pStream->str();
            }
            catch (...)
            {
                return "get PStream failed";
            }

        }

        template <> std::string TBaseToString<TSpan>(const TSpan& tbase);

        extern std::string TBaseToString2(TBasePtr &tbasePtr);

        template <class T>
        std::string generic_type_to_string(T& t)
        {
            static const char error[] = "Error happen!";

            PStream pStream = NULL;
            try
            {
                pStream = FormatConverter::getPStream();
            }
            catch (std::bad_alloc& )
            {
                return error;
            }

            *pStream << t;

            return pStream->str();
        }

        template <>
        std::string generic_type_to_string<TBasePtr>(TBasePtr& t);

        template <>
        std::string generic_type_to_string<std::vector<TBasePtr> >(std::vector<TBasePtr>& t);

    }
}

#endif
