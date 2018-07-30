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
#ifndef PINPOINT_SERIALIZER_H
#define PINPOINT_SERIALIZER_H

#include "stdint.h"
#include "buffer.h"
#include "pinpoint_api.h"
#include <string>
#include <thrift/TBase.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/transport/TBufferTransports.h>

using namespace  apache::thrift::protocol;
using namespace  apache::thrift::transport;

namespace Pinpoint
{
    namespace Agent
    {

        class Header
        {
        public:
            static const int8_t SIGNATURE = (int8_t) 0xef;
            static const int32_t HEADER_SIZE = 4;
            static const int8_t HEADER_VERSION = 0x10;

            Header();

            explicit Header(int16_t type)
            {
                this->signature = SIGNATURE;
                this->version = HEADER_VERSION;
                this->type = type;
            }

            Header(int8_t signature, int8_t version, int16_t type)
            {
                this->signature = signature;
                this->version = version;
                this->type = type;
            }

            int8_t getSignature() const
            {
                return signature;
            }

            void setSignature(int8_t signature)
            {
                this->signature = signature;
            }

            int8_t getVersion() const
            {
                return version;
            }

            void setVersion(int8_t version)
            {
                this->version = version;
            }

            int16_t getType() const
            {
                return type;
            }

            void setType(int16_t type)
            {
                this->type = type;
            }

            std::string toString() 
            {
                try
                {
                    utils::PStream pStream = utils::FormatConverter::getPStream();
                    *pStream << "Header[" \
                             << "signature=" << signature << ", " \
                             << "version=" << version << ", " \
                             << "type" << type \
                             << "]";
                    return pStream->str();
                }
                catch (...)
                {

                }

                return "Header[unknown]";
            }

            bool isValid()
            {
                return signature == SIGNATURE && version == HEADER_VERSION;
            }

        private:
            int8_t signature;
            int8_t version;
            int16_t type;
        };


        class DefaultTBaseLocator
        {
        public:
            static Header *createHeader(int16_t type);

            static apache::thrift::TBase *tBaseLookup(short type);

            static Header headerLookup(const apache::thrift::TBase &tbase);

            static const int16_t UNKNOWN = -1;
            static const Header UNKNOWN_HEADER;

            static const int16_t SPAN = 40;
            static const Header SPAN_HEADER;

            static const int16_t AGENT_INFO = 50;
            static const Header AGENT_INFO_HEADER;

            static const int16_t AGENT_STAT = 55;
            static const Header AGENT_STAT_HEADER;

            static const int16_t AGENT_STAT_BATCH = 56;
            static const Header AGENT_STAT_BATCH_HEADER;

            static const int16_t API_META_DATA = 310;
            static const Header API_META_DATA_HEADER;

            static const int16_t STRING_META_DATA = 330;
            static const Header STRING_META_DATA_HEADER;

            static const int16_t RESULT = 320;
            static const Header RESULT_HEADER;
        };

        class HeaderTBaseSerializer
        {
        public:
            HeaderTBaseSerializer();

            int32_t serializer(const apache::thrift::TBase &tbase, std::string &data);

            void reset();

        private:
            boost::shared_ptr<TMemoryBuffer> transportOut;
            boost::shared_ptr<TCompactProtocol> protocolOut;

            void writeHeader(Header &header);
        };

        class HeaderTBaseDeSerializer
        {
        public:
            HeaderTBaseDeSerializer();

            int32_t deSerializer(Header& header,
                                 boost::shared_ptr<apache::thrift::TBase> &tbasePtr,
                                 const std::string &data);

            void reset();

        private:
            boost::shared_ptr<TMemoryBuffer> transportOut;
            boost::shared_ptr<TCompactProtocol> protocolOut;

        };


        class ControlMessageProtocolConstant
        {
        public:
            static const int32_t TYPE_CHARACTER_NULL = 'N';

            static const int32_t TYPE_CHARACTER_BOOL_TRUE = 'T';

            static const int32_t TYPE_CHARACTER_BOOL_FALSE = 'F';

            static const int32_t TYPE_CHARACTER_INT = 'I';

            static const int32_t TYPE_CHARACTER_LONG = 'L';

            static const int32_t TYPE_CHARACTER_DOUBLE = 'D';

            static const int32_t TYPE_CHARACTER_STRING = 'S';

            static const int32_t CONTROL_CHARACTER_LIST_START = 'V';

            static const int32_t CONTROL_CHARACTER_LIST_END = 'z';

            static const int32_t CONTROL_CHARACTER_MAP_START = 'M';

            static const int32_t CONTROL_CHARACTER_MAP_END = 'z';

        };


        class ControlMessageEncoder
        {
        public:
            void startEncoder();

            std::string endEncoder();

            void addString(const std::string &s);

            void addInt32(int32_t i);

            void addNull();

            void addBool(bool value);

            void addInt64(int64_t value);

            void addDouble(double value);

            void addSetBegin();

            void addSetEnd();

        private:
            ChannelBufferV2 channelBuffer;

            void putPrefixedBytes(const std::vector<int8_t> &value);
        };

        class ControlMessageDecoderException : public std::exception
        {

        };

        struct ControlMessage;
        typedef boost::shared_ptr<ControlMessage> ControlMessagePtr;
        typedef std::vector<ControlMessagePtr> ControlMessageList;
        class ControlMessageMap;
//        typedef std::map<ControlMessagePtr, ControlMessagePtr> ControlMessageMap;

        struct ControlMessage
        {
            static const int32_t CONTROL_MESSAGE_UNKNOWN = -1;
            static const int32_t CONTROL_MESSAGE_NULL = 0;
            static const int32_t CONTROL_MESSAGE_BOOL = 1;
            static const int32_t CONTROL_MESSAGE_LONG = 2;
            static const int32_t CONTROL_MESSAGE_DOUBLE = 3;
            static const int32_t CONTROL_MESSAGE_STRING = 4;
            static const int32_t CONTROL_MESSAGE_LIST = 5;
            static const int32_t CONTROL_MESSAGE_MAP = 6;

            int32_t type;
            union
            {
                bool b;
                int64_t i;
                double d;
                void* ref;
            } data;

            ControlMessage() : type(CONTROL_MESSAGE_UNKNOWN) {}

            ~ControlMessage();

            std::string toString() const;

        private:
            std::string mapToString() const;
            std::string listToString() const;

            static std::string type2String(int32_t type);

        };

        class ControlMessagePtrComparator
        {
        public:
            bool operator()(const ControlMessagePtr& key1, const ControlMessagePtr& key2) const
            {
                if (key1 == NULL)
                {
                    return false;
                }

                if (key2 == NULL)
                {
                    return true;
                }

                if (key1->type != key2->type)
                {
                    return key1->type < key2->type;
                }
                else if (key1->type == ControlMessage::CONTROL_MESSAGE_LIST)
                {
                    return key1->data.ref < key2->data.ref;
                }
                else if (key1->type == ControlMessage::CONTROL_MESSAGE_MAP)
                {
                    return key1->data.ref < key2->data.ref;
                }
                else if (key1->type == ControlMessage::CONTROL_MESSAGE_STRING)
                {
                    return *(std::string*)key1->data.ref < *(std::string*)key2->data.ref;
                }
                else if (key1->type == ControlMessage::CONTROL_MESSAGE_BOOL)
                {
                    return key1->data.b < key2->data.b;
                }
                else if (key1->type == ControlMessage::CONTROL_MESSAGE_DOUBLE)
                {
                    return key1->data.d < key2->data.d;
                }
                else
                {
                    return key1->data.i < key2->data.i;
                }
            }
        };


        class ControlMessageMap
        {
        public:
            friend class ControlMessage;
            void add(ControlMessagePtr& key, ControlMessagePtr& value)
            {
                map_[key] = value;
            }

            ControlMessagePtr get(int32_t type, void* value);

        private:
            std::map<ControlMessagePtr, ControlMessagePtr, ControlMessagePtrComparator> map_;
        };

        class ControlMessageDecoder
        {
        public:
            void setBuffer(std::string& data);
            ControlMessagePtr decode();
        private:
            ChannelBufferV2 channelBuffer;
            int8_t getType();
            void decodeString(ControlMessage* message);
            int32_t readStringLength();
            bool isListFinished();
            bool isMapFinished();
        };
    }
}

#endif
