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
#include "utility.h"
#include "serializer.h"
#include "trace.h"
#include <boost/asio.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

namespace Pinpoint
{
    namespace utils
    {

        //<editor-fold desc="Global">

        uint64_t get_current_microsec_stamp64()
        {
            static const uint64_t ticks_per_microsecond = boost::posix_time::time_duration::ticks_per_second() / SECOND;
            boost::posix_time::ptime epoch(
                    boost::gregorian::date(1970, boost::gregorian::Jan, 1));
            boost::posix_time::time_duration time_from_epoch =
                    boost::posix_time::microsec_clock::universal_time() - epoch;


            return time_from_epoch.total_microseconds() / ticks_per_microsecond;
        }

        int32_t get_current_process_id()
        {
            return boost::interprocess::ipcdetail::get_current_process_id();
        }
        uint64_t get_current_unixsec_stamp()
        {
#ifdef __GNUC__
            return time(NULL);
#else
            boost::posix_time::ptime epoch(
                    boost::gregorian::date(1970, boost::gregorian::Jan, 1));
            boost::posix_time::time_duration time_from_epoch =
                    boost::posix_time::microsec_clock::universal_time() - epoch;
            return time_from_epoch.total_seconds();
#endif
        }
        int64_t double_to_long_bits(double value)
        {
            // todo: like java Double.doubleToLongBits
            union
            {
                int64_t ival;
                double dval;
            } val;
            val.dval = value;
            return val.ival;
        }

        double long_bits_to_double(int64_t value)
        {
            union
            {
                int64_t ival;
                double dval;
            } val;
            val.ival = value;
            return val.dval;
        }


        /// 1504248328.423 1504248328423
        int64_t dotsec_to_milisec(const char* pvalue)
        {
            if(pvalue == NULL)
            {
                return 0;
            }

            int64_t milis= 0;
            milis = atoll(pvalue);
            milis *=1000;

            const char* pdot = strchr(pvalue,'.');
            if(pdot != NULL && *(pdot + 1) !='\0')
            {
                milis+= atoi(pdot + 1);
            }

            return (milis > 0) ?(milis):(0);
        }

        int32_t safe_strlen(const char* str)
        {
            if (str == NULL)
            {
                return -1;
            }

            return (int32_t)strlen(str);
        }


        void split_string(const std::string& src, std::vector<std::string>& splits,
                          const std::string& delimiter)
        {
            splits.clear();

            if (src.length() == 0)
            {
                return;
            }

            std::size_t start = 0;
            std::size_t index = src.find_first_of(delimiter, start);
            while(index != std::string::npos)
            {
                std::string s = src.substr(start, index - start);
                splits.push_back(s);
                start = index + delimiter.length();
                index = src.find_first_of(delimiter, start);
            }

            splits.push_back(src.substr(start));

        }

        bool Base64Encode(std::string& outPut, const std::string& inPut)
        {
            using namespace boost::archive::iterators;
            typedef base64_from_binary<transform_width<std::string::const_iterator,6,8> > Base64EncodeIter;

            std::stringstream  result;
            copy(Base64EncodeIter(inPut.begin()),
                 Base64EncodeIter(inPut.end()),
                 std::ostream_iterator<char>(result));

            size_t Num = (3 - inPut.length() % 3) % 3;
            for (size_t i = 0; i < Num; i++)
            {
                result.put('=');
            }
            outPut = result.str();
            return !outPut.empty();
        }

        bool Base64Decode(std::string& outPut, const std::string& inPut)
        {
            using namespace boost::archive::iterators;
            typedef transform_width<binary_from_base64<std::string::const_iterator>, 8, 6> Base64DecodeIter;

            std::stringstream result;
            try
            {
                copy(Base64DecodeIter(inPut.begin()),
                     Base64DecodeIter(inPut.end()),
                     std::ostream_iterator<char>(result));
            }
            catch (...)
            {
                return false;
            }
            outPut = result.str();
            return !outPut.empty();
        }

        template <> std::string TBaseToString<TSpan>(const TSpan& tbase)
        {
            try
            {
                // TSpan's transactionId has '\0'
                TSpan tmp = tbase;
                std::vector<int8_t> transactionIdBytes(tmp.transactionId.begin(), tmp.transactionId.end());
                Trace::TransactionId transactionId = Trace::TransactionId::parseTransactionId(transactionIdBytes);
                tmp.__set_transactionId(Trace::TransactionId::formatString(transactionId));
                PStream pStream = FormatConverter::getPStream();
                *pStream << tmp;
                return pStream->str();
            }
            catch (std::invalid_argument&)
            {
                return "parseTransactionId failed";
            }
            catch (...)
            {
                return "get PStream failed";
            }
        }

        std::string TBaseToString2(TBasePtr &tbasePtr)
        {
            const std::type_info &tb = typeid(*tbasePtr);

            static const std::type_info &spanType = typeid(TSpan);
            static const std::type_info &apiMetaDataType = typeid(TApiMetaData);
            static const std::type_info &agentInfoType = typeid(TAgentInfo);
            static const std::type_info &agentStatType = typeid(TAgentStat);
            static const std::type_info &agentStatBatchType = typeid(TAgentStatBatch);
            static const std::type_info &stringMetaDataType = typeid(TStringMetaData);
            static const std::type_info &resultDataType = typeid(TResult);

            if (tb == spanType)
            {
                return TBaseToString(*boost::dynamic_pointer_cast<TSpan>(tbasePtr));
            }
            else if(tb == agentInfoType)
            {
                return TBaseToString(*boost::dynamic_pointer_cast<TAgentInfo>(tbasePtr));
            }
            else if(tb == agentStatType)
            {
                return TBaseToString(*boost::dynamic_pointer_cast<TAgentStat>(tbasePtr));
            }
            else if(tb == agentStatBatchType)
            {
                return TBaseToString(*boost::dynamic_pointer_cast<TAgentStatBatch>(tbasePtr));
            }
            else if(tb == apiMetaDataType)
            {
                return TBaseToString(*boost::dynamic_pointer_cast<TApiMetaData>(tbasePtr));
            }
            else if(tb == stringMetaDataType)
            {
                return TBaseToString(*boost::dynamic_pointer_cast<TStringMetaData>(tbasePtr));
            }
            else if(tb == resultDataType)
            {
                return TBaseToString(*boost::dynamic_pointer_cast<TResult>(tbasePtr));
            }
            else
            {
                return "Unknown tbase type!!!";
            }
        }

        template <>
        std::string generic_type_to_string<TBasePtr>(TBasePtr& t)
        {
            return TBaseToString2(t);
        }

        template <>
        std::string generic_type_to_string<std::vector<TBasePtr> >(std::vector<TBasePtr>& t)
        {
            std::string result = "[";
            for (std::vector<TBasePtr>::iterator ip = t.begin(); ip != t.end(); ++ip)
            {
                result = "(" + TBaseToString2(*ip) + ")";
            }
            result += "]";
            return result;
        }

        std::string trim(const std::string& str)
        {
            std::size_t first = str.find_first_not_of(' ');
            if (std::string::npos == first)
            {
                return str;
            }
            std::size_t last = str.find_last_not_of(' ');
            return str.substr(first, (last - first + 1));
        }

        //</editor-fold>

        //<editor-fold desc="FormatConverter">

        class FormatConverterImp
        {
        public:
            static std::string int64ToString(int64_t value);
            static int64_t stringToInt64(const std::string& value);
            static std::string getExceptionMsg(const char* file, int32_t lineno, const char* msg);
            static std::string getErrorMsg(const char* file, int32_t lineno, const char* msg);
            static std::string formatMsg(const char* header,const char* file, int32_t lineno, const char* msg);
            static PStream getPStream();
        private:
            static boost::thread_specific_ptr<std::stringstream> streamTls;
        };

        boost::thread_specific_ptr<std::stringstream> FormatConverterImp::streamTls;

        PStream FormatConverterImp::getPStream()
        {
            if (streamTls.get() == NULL)
            {
                // no catch, let it throw
                PStream pStream = new std::stringstream;
                streamTls.reset(pStream);
            }

            streamTls->str("");
            streamTls->clear();
            return streamTls.get();
        }

        std::string FormatConverterImp::getErrorMsg(const char* file, int32_t lineno, const char* msg)
        {
            PStream pStream = getPStream();
            *pStream << "Fatal error: " << msg << " in " << file << " on line " << lineno;

            return pStream->str();
        }

        std::string FormatConverterImp::getExceptionMsg(const char* file, int32_t lineno, const char* msg)
        {
            PStream pStream = getPStream();
            *pStream << "exception: " << msg << " in " << file << " on line " << lineno;

            return pStream->str();
        }

        std::string FormatConverterImp::formatMsg(const char* header,const char* file,
                int32_t lineno, const char* msg){
            PStream pStream = getPStream();
            *pStream << "\""<< header <<" error: " << msg << " in " << file << " on line " << lineno;
            return pStream->str();
        }

        std::string FormatConverterImp::int64ToString(int64_t value)
        {
            PStream pStream = getPStream();
            std::string tmp;
            *pStream << value;
            *pStream >> tmp;
            return tmp;
        }

        int64_t FormatConverterImp::stringToInt64(const std::string &value)
        {
            PStream pStream = getPStream();
            int64_t result;

            if (value.size() == 0)
            {
                throw std::invalid_argument(std::string("bad value=NULL"));
            }

            // todo rewrite
            const char* p = value.c_str();

            /* skip space */
            for (; p != NULL && ((char)*p == ' ' || (char)*p == '\t'); p++)
                ;

            if (strlen(p) == 0)
            {
                throw std::invalid_argument(std::string("bad value=NULL"));
            }

            *pStream << p;

            *pStream >> result;

            /* check */
            if (!pStream->eof())
            {
                /* like "123abc" "abc123"*/
                throw std::invalid_argument(std::string("bad value=NULL"));
            }

            return result;
        }

        std::string FormatConverter::formatMsg(const char *header, const char *file, int32_t lineno, const char *msg)
        {
            return FormatConverterImp::formatMsg(header, file, lineno, msg);
        }

        std::string FormatConverter::getErrorMsg(const char *file, int32_t lineno, const char *msg)
        {
            return FormatConverterImp::getErrorMsg(file, lineno, msg);
        }

        std::string FormatConverter::getExceptionMsg(const char *file, int32_t lineno, const char *msg)
        {
            return FormatConverterImp::getExceptionMsg(file, lineno, msg);
        }

        PStream FormatConverter::getPStream()
        {
            return FormatConverterImp::getPStream();
        }

        std::string FormatConverter::int64ToString(int64_t value)
        {
            return FormatConverterImp::int64ToString(value);
        }

        int64_t FormatConverter::stringToInt64(const std::string &value) 
        {
            return FormatConverterImp::stringToInt64(value);
        }

        //</editor-fold>

    }
}
