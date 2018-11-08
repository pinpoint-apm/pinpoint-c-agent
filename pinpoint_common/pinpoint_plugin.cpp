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
#include "pinpoint_plugin.h"
#include <algorithm>
#include <boost/thread.hpp>

using namespace Pinpoint::log;

namespace Pinpoint
{
    namespace Plugin
    {
        int32_t EXCEPTION_STRING_ID = Agent::INVALID_STRING_ID;
        int32_t ERROR_STRING_ID = Agent::INVALID_STRING_ID;
        int32_t WARNINGS_STRING_ID = Agent::INVALID_STRING_ID;

        //<editor-fold desc="RequestHeader">

        class RequestHeaderImp
        {
        public:
            static HttpHeader* getCurrentRequestHeader();
        private:
            static boost::thread_specific_ptr<HttpHeader> currentHeaderTls;
        };

        class ResponseHeaderImp
        {
        public:
            static HttpHeader* getCurrentResponseHeader();
        private:
            static boost::thread_specific_ptr<HttpHeader> currentHeaderTls;
        };


        boost::thread_specific_ptr<HttpHeader> RequestHeaderImp::currentHeaderTls;
        boost::thread_specific_ptr<HttpHeader> ResponseHeaderImp::currentHeaderTls;


        HttpHeader* RequestHeaderImp::getCurrentRequestHeader()
        {
            HttpHeader* header = RequestHeaderImp::currentHeaderTls.get();

            if (header == NULL)
            {
                try
                {
                    header = new HttpHeader();
                }
                catch (std::bad_alloc&)
                {
                    LOGE("create RequestHeader failed");
                    return NULL;
                }

                RequestHeaderImp::currentHeaderTls.reset(header);
            }

            return header;
        }

        HttpHeader* RequestHeader::getCurrentRequestHeader()
        {
            return RequestHeaderImp::getCurrentRequestHeader();
        }

        HttpHeader* ResponseHeaderImp::getCurrentResponseHeader()
        {
            HttpHeader* header = ResponseHeaderImp::currentHeaderTls.get();

            if (header == NULL)
            {
                try
                {
                    header = new HttpHeader();
                }
                catch (std::bad_alloc&)
                {
                    LOGE("create RequestHeader failed");
                    return NULL;
                }

                ResponseHeaderImp::currentHeaderTls.reset(header);
            }

            return header;
        }

        HttpHeader* ResponseHeader::getCurrentResponseHeader()
        {
            return ResponseHeaderImp::getCurrentResponseHeader();
        }

        bool HttpHeader::getHeader(const std::string &name, std::string &header) const
        {

            HeaderMap::const_iterator ip = headers.find(name);
            if (ip != headers.end())
            {
                header = ip->second;
                return true;
            }


            // for php-fpm
            std::string extName;
            extName += "HTTP_" + name;
            std::transform(extName.begin(), extName.end(), extName.begin(), ::toupper);
            std::replace(extName.begin(), extName.end(), '-', '_');
            ip = headers.find(extName);
            if (ip != headers.end())
            {
                header = ip->second;
                return true;
            }

            return false;
        }

        void HttpHeader::updateHeader(const HeaderMap &headers)
        {
            this->headers = headers;
        }

        //</editor-fold>

        const std::string ExceptionInfo::toString() const
        {
            std::string errorMsg;

            try
            {
                errorMsg = utils::FormatConverter::getExceptionMsg(this->file.c_str(),
                                                                   this->line,
                                                                   this->message.c_str());
            }
            catch (...)
            {
                errorMsg = this->message;
            }
            return errorMsg;
        }

        const std::string ErrorInfo::toString() const
        {
            std::string errorMsg;

            try
            {
                errorMsg = utils::FormatConverter::getErrorMsg(this->file.c_str(),
                                                               this->line,
                                                               this->message.c_str());
            }
            catch (...)
            {
                errorMsg = this->message;
            }
            return errorMsg;
        }
    }
}
