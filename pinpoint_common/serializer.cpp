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
#include "thrift/gen-cpp/Pinpoint_types.h"
#include "thrift/gen-cpp/Trace_types.h"
#include "serializer.h"
#include "utility.h"

namespace Pinpoint
{
    namespace Agent
    {
        const Header DefaultTBaseLocator::UNKNOWN_HEADER(DefaultTBaseLocator::UNKNOWN);
        const Header DefaultTBaseLocator::SPAN_HEADER(DefaultTBaseLocator::SPAN);
        const Header DefaultTBaseLocator::AGENT_INFO_HEADER(DefaultTBaseLocator::AGENT_INFO);
        const Header DefaultTBaseLocator::AGENT_STAT_HEADER(DefaultTBaseLocator::AGENT_STAT);
        const Header DefaultTBaseLocator::AGENT_STAT_BATCH_HEADER(DefaultTBaseLocator::AGENT_STAT_BATCH);
        const Header DefaultTBaseLocator::API_META_DATA_HEADER(DefaultTBaseLocator::API_META_DATA);
        const Header DefaultTBaseLocator::STRING_META_DATA_HEADER(DefaultTBaseLocator::STRING_META_DATA);
        const Header DefaultTBaseLocator::RESULT_HEADER(DefaultTBaseLocator::RESULT);

        Header::Header()
        {
            this->signature = SIGNATURE;
            this->version = HEADER_VERSION;
            this->type = DefaultTBaseLocator::UNKNOWN;
        }

        Header *DefaultTBaseLocator::createHeader(int16_t type)
        {
            try
            {
                Header *header = new Header();
                header->setType(type);
                return header;
            }
            catch (std::exception& exception)
            {
                LOGE("DefaultTBaseLocator::createHeader throw exception=%s", exception.what());
            }
            return NULL;
        }

        apache::thrift::TBase *DefaultTBaseLocator::tBaseLookup(short type)
        {
            switch (type)
            {
                case SPAN:
                    return new TSpan();
                case AGENT_INFO:
                    return new TAgentInfo();
                default:
                    return NULL;
            }
        }

        Header DefaultTBaseLocator::headerLookup(const apache::thrift::TBase &tbase)
        {
            const std::type_info &tb = typeid(tbase);
            static const std::type_info &spanType = typeid(TSpan);
            static const std::type_info &apiMetaDataType = typeid(TApiMetaData);
            static const std::type_info &agentInfoType = typeid(TAgentInfo);
            static const std::type_info &agentStatType = typeid(TAgentStat);
            static const std::type_info &agentStatBatchType = typeid(TAgentStatBatch);
            static const std::type_info &stringMetaDataType = typeid(TStringMetaData);
            static const std::type_info &resultType = typeid(TResult);

            if (tb == spanType)
            {
                return SPAN_HEADER;
            }
            else if (tb == agentInfoType)
            {
                return AGENT_INFO_HEADER;
            }
            else if (tb == agentStatType)
            {
                return AGENT_STAT_HEADER;
            }
            else if (tb == agentStatBatchType)
            {
                return AGENT_STAT_BATCH_HEADER;
            }
            else if (tb == apiMetaDataType)
            {
                return API_META_DATA_HEADER;
            }
            else if(tb == stringMetaDataType)
            {
                return STRING_META_DATA_HEADER;
            }
            else if(tb == resultType)
            {
                return RESULT_HEADER;
            }
            else
            {
                assert (false);
            }

            return UNKNOWN_HEADER;
        }

        HeaderTBaseSerializer::HeaderTBaseSerializer() :
                transportOut(new TMemoryBuffer()), protocolOut(new TCompactProtocol(this->transportOut))
        {

        }

        void HeaderTBaseSerializer::reset()
        {
            transportOut->resetBuffer();
        }

        void HeaderTBaseSerializer::writeHeader(Header &header)
        {
            protocolOut->writeByte(header.getSignature());
            protocolOut->writeByte(header.getVersion());

            int16_t type = header.getType();

            protocolOut->writeByte(type >> 8);
            protocolOut->writeByte(type & 0xff);
        }

        int32_t HeaderTBaseSerializer::serializer(const apache::thrift::TBase &tbase, std::string &data)
        {
            Header header = DefaultTBaseLocator::headerLookup(tbase);

            if (header.getType() == DefaultTBaseLocator::UNKNOWN)
            {
                assert(false);
                return FAILED;
            }

            transportOut->resetBuffer();

            writeHeader(header);

            tbase.write(protocolOut.get());

            data = transportOut->getBufferAsString();

            return SUCCESS;
        }


        HeaderTBaseDeSerializer::HeaderTBaseDeSerializer():
                transportOut(new TMemoryBuffer()), protocolOut(new TCompactProtocol(this->transportOut))
        {

        }

        int32_t HeaderTBaseDeSerializer::deSerializer(Header& header, boost::shared_ptr<apache::thrift::TBase> &tbasePtr,
                                                      const std::string &data)
        {
            try
            {
                transportOut->resetBuffer((uint8_t*)(const_cast<char*>(data.c_str())),
                                          (uint32_t)data.size(), TMemoryBuffer::COPY);

                /* read header */
                int8_t signature;
                int8_t version;
                int8_t type_1, type_2;
                int16_t type;
                (void)protocolOut->readByte(signature);
                (void)protocolOut->readByte(version);
                (void)protocolOut->readByte(type_1);
                (void)protocolOut->readByte(type_2);

                type = ((int16_t)type_1 << 8) | type_2;

                header.setSignature(signature);
                header.setType(type);
                header.setVersion(version);

                if (!header.isValid())
                {
                    LOGE("header is invalid. header=%s", header.toString().c_str())
                    return FAILED;
                }

                switch(header.getType())
                {
                    case DefaultTBaseLocator::SPAN:
                        tbasePtr.reset(new TSpan);
                        break;
                    case DefaultTBaseLocator::AGENT_INFO:
                        tbasePtr.reset(new TAgentInfo);
                        break;
                    case DefaultTBaseLocator::AGENT_STAT:
                        tbasePtr.reset(new TAgentStat);
                        break;
                    case DefaultTBaseLocator::AGENT_STAT_BATCH:
                        tbasePtr.reset(new TAgentStatBatch);
                        break;
                    case DefaultTBaseLocator::API_META_DATA:
                        tbasePtr.reset(new TApiMetaData);
                        break;
                    case DefaultTBaseLocator::STRING_META_DATA:
                        tbasePtr.reset(new TStringMetaData);
                        break;
                    case DefaultTBaseLocator::RESULT:
                        tbasePtr.reset(new TResult);
                        break;
                    default:
                        return NOT_SUPPORTED;

                }

                tbasePtr->read(protocolOut.get());
                LOGI("get tbase: type=%d, context=%s", header.getType(),
                     utils::TBaseToString2(tbasePtr).c_str());
                return SUCCESS;
            }
            catch (std::exception& exception)
            {
                LOGE("HeaderTBaseDeSerializer::deSerializer throw exception=%s", exception.what());
                assert (false);
                return FAILED;
            }

        }

        void HeaderTBaseDeSerializer::reset()
        {
            transportOut->resetBuffer();
        }

        void ControlMessageEncoder::startEncoder()
        {
            channelBuffer.reset();
            channelBuffer.writeByte((int8_t) ControlMessageProtocolConstant::CONTROL_CHARACTER_MAP_START);
        }

        std::string ControlMessageEncoder::endEncoder()
        {
            channelBuffer.writeByte((int8_t) ControlMessageProtocolConstant::CONTROL_CHARACTER_MAP_END);
            return channelBuffer.getBuffer();
        }


        void ControlMessageEncoder::addInt32(int32_t i)
        {
            channelBuffer.writeByte((int8_t) ControlMessageProtocolConstant::TYPE_CHARACTER_INT);

            channelBuffer.writeByte((int8_t) (i >> 24));
            channelBuffer.writeByte((int8_t) (i >> 16));
            channelBuffer.writeByte((int8_t) (i >> 8));
            channelBuffer.writeByte((int8_t) (i));
        }

        void ControlMessageEncoder::addString(const std::string &s)
        {
            channelBuffer.writeByte((int8_t) ControlMessageProtocolConstant::TYPE_CHARACTER_STRING);

            // putPrefixedBytes(value.getBytes(charset), out);
            // todo charset
            std::vector<int8_t> value(s.begin(), s.end());
            putPrefixedBytes(value);
        }

        void ControlMessageEncoder::addNull()
        {
            channelBuffer.writeByte((int8_t) ControlMessageProtocolConstant::TYPE_CHARACTER_NULL);
        }

        void ControlMessageEncoder::addBool(bool value)
        {
            if (value)
            {
                channelBuffer.writeByte((int8_t) ControlMessageProtocolConstant::TYPE_CHARACTER_BOOL_TRUE);
            }
            else
            {
                channelBuffer.writeByte((int8_t) ControlMessageProtocolConstant::TYPE_CHARACTER_BOOL_FALSE);
            }
        }

        void ControlMessageEncoder::addInt64(int64_t value)
        {
            channelBuffer.writeByte((int8_t) ControlMessageProtocolConstant::TYPE_CHARACTER_LONG);

            channelBuffer.writeByte((int8_t) (value >> 56));
            channelBuffer.writeByte((int8_t) (value >> 48));
            channelBuffer.writeByte((int8_t) (value >> 40));
            channelBuffer.writeByte((int8_t) (value >> 32));
            channelBuffer.writeByte((int8_t) (value >> 24));
            channelBuffer.writeByte((int8_t) (value >> 16));
            channelBuffer.writeByte((int8_t) (value >> 8));
            channelBuffer.writeByte((int8_t) (value));
        }

        void ControlMessageEncoder::addDouble(double value)
        {
            channelBuffer.writeByte((int8_t) ControlMessageProtocolConstant::TYPE_CHARACTER_DOUBLE);
            int64_t longValue = utils::double_to_long_bits(value);
            channelBuffer.writeLong(longValue);
        }

        void ControlMessageEncoder::addSetBegin()
        {
            channelBuffer.writeByte((int8_t) ControlMessageProtocolConstant::CONTROL_CHARACTER_LIST_START);
        }

        void ControlMessageEncoder::addSetEnd()
        {
            channelBuffer.writeByte((int8_t) ControlMessageProtocolConstant::CONTROL_CHARACTER_LIST_END);
        }

        void ControlMessageEncoder::putPrefixedBytes(const std::vector<int8_t> &value)
        {
            int32_t length = (int32_t) value.size();

            int8_t lengthBuf[5] = {0, 0, 0, 0, 0};

            int idx = 0;
            while (true)
            {
                if ((length & 0xFFFFFF80) == 0)
                {
                    lengthBuf[(idx++)] = (int8_t) length;
                    break;
                }

                lengthBuf[(idx++)] = (int8_t) ((length & 0x7F) | 0x80);

                length = (int8_t) (((uint8_t) length) >> 7);
            }

            for (int i = 0; i < idx; i++)
            {
                channelBuffer.writeByte(lengthBuf[i]);
            }

            channelBuffer.writeBytes(value);
        }

        void ControlMessageDecoder::setBuffer(std::string &data)
        {
            this->channelBuffer.setBuffer(data);
        }

        int8_t ControlMessageDecoder::getType()
        {
            return this->channelBuffer.readByte();
        }

        ControlMessagePtr ControlMessageDecoder::decode()
        {
            int8_t type = getType();
            // throw
            ControlMessage* message = new ControlMessage();
            ControlMessagePtr controlMessagePtr;

            ControlMessageList* answerList = NULL;
            ControlMessageMap* answerMap = NULL;
            switch (type) {
                case ControlMessageProtocolConstant::TYPE_CHARACTER_NULL:
                    message->type = ControlMessage::CONTROL_MESSAGE_NULL;
                    break;
                case ControlMessageProtocolConstant::TYPE_CHARACTER_BOOL_TRUE:
                    message->type = ControlMessage::CONTROL_MESSAGE_BOOL;
                    message->data.b = true;
                    break;
                case ControlMessageProtocolConstant::TYPE_CHARACTER_BOOL_FALSE:
                    message->type = ControlMessage::CONTROL_MESSAGE_BOOL;
                    message->data.b = false;
                    break;
                case ControlMessageProtocolConstant::TYPE_CHARACTER_INT:
                    message->type = ControlMessage::CONTROL_MESSAGE_LONG;
                    message->data.i = channelBuffer.readInt();
                    break;
                case ControlMessageProtocolConstant::TYPE_CHARACTER_LONG:
                    message->type = ControlMessage::CONTROL_MESSAGE_LONG;
                    message->data.i = channelBuffer.readLong();
                    break;
                case ControlMessageProtocolConstant::TYPE_CHARACTER_DOUBLE:
                    message->type = ControlMessage::CONTROL_MESSAGE_DOUBLE;
                    message->data.d = utils::long_bits_to_double(channelBuffer.readLong());
                    break;
                case ControlMessageProtocolConstant::TYPE_CHARACTER_STRING:
                    message->type = ControlMessage::CONTROL_MESSAGE_STRING;
                    decodeString(message);
                    break;
                case ControlMessageProtocolConstant::CONTROL_CHARACTER_LIST_START:
                    message->type = ControlMessage::CONTROL_MESSAGE_LIST;
                    answerList = new ControlMessageList();
                    while (!isListFinished()) {
                        answerList->push_back(decode());
                    }
                    channelBuffer.readByte(); // Skip the terminator
                    message->data.ref = answerList;
                    break;
                case ControlMessageProtocolConstant::CONTROL_CHARACTER_MAP_START:
                    message->type = ControlMessage::CONTROL_MESSAGE_MAP;
                    answerMap = new ControlMessageMap();
                    while (!isMapFinished()) {
                        ControlMessagePtr key = decode();
                        ControlMessagePtr value = decode();
                        answerMap->add(key, value);
                    }
                    channelBuffer.readByte(); // Skip the terminator
                    message->data.ref = answerMap;
                    break;
                default:
                    throw ControlMessageDecoderException();
            }
            controlMessagePtr.reset(message);

            return controlMessagePtr;
        }

        void ControlMessageDecoder::decodeString(ControlMessage *message)
        {
            int32_t length = readStringLength();
            std::vector<int8_t> data;
            channelBuffer.readBytes(data, (uint32_t)length);
            message->data.ref = new std::string(data.begin(), data.end());
        }

        int32_t ControlMessageDecoder::readStringLength()
        {
            int32_t result = 0;
            int32_t shift = 0;

            while (true) {
                int8_t b = channelBuffer.readByte();
                result |= (b & 0x7F) << shift;
                if ((b & 0x80) != 128)
                    break;
                shift += 7;
            }
            return result;
        }

        bool ControlMessageDecoder::isListFinished()
        {
            return channelBuffer.peekByte() == ControlMessageProtocolConstant::CONTROL_CHARACTER_LIST_END;
        }

        bool ControlMessageDecoder::isMapFinished()
        {
            return channelBuffer.peekByte() == ControlMessageProtocolConstant::CONTROL_CHARACTER_MAP_END;
        }

        ControlMessagePtr ControlMessageMap::get(int32_t type, void* value)
        {
            ControlMessagePtr controlMessagePtr;
            assert (value != NULL);
            try
            {
                ControlMessage* message = new ControlMessage();
                controlMessagePtr.reset(message);
                std::string* st_p = NULL;
                message->type = type;
                switch (type)
                {
#ifdef UNUSED_CODE
                    case ControlMessage::CONTROL_MESSAGE_LONG:
                        message->data.i = *(int64_t*)value;
                        break;
                    case ControlMessage::CONTROL_MESSAGE_BOOL:
                        message->data.b = *(bool*)value;
                        break;
                    case ControlMessage::CONTROL_MESSAGE_DOUBLE:
                        message->data.d = *(double*)value;
                        break;
#endif
                    case ControlMessage::CONTROL_MESSAGE_STRING:
                        st_p = (std::string*)value;
                        message->data.ref = new std::string(st_p->begin(), st_p->end());
                        break;
                    default:
                        LOGE("not support type=%d as map key", type)
                        return ControlMessagePtr();
                }
            }
            catch (...)
            {
                LOGE("new message key failed");
                return ControlMessagePtr();
            }
            std::map<ControlMessagePtr, ControlMessagePtr>::iterator it;
            it = map_.find(controlMessagePtr);
            if (it == map_.end())
            {
                return ControlMessagePtr();
            }
            return it->second;
        }

        ControlMessage::~ControlMessage()
        {
            if (type == CONTROL_MESSAGE_STRING)
            {
                std::string* p = (std::string*)this->data.ref;
                delete p;
            }
            else if (type == CONTROL_MESSAGE_LIST)
            {
                ControlMessageList* p = (ControlMessageList*)this->data.ref;
                delete p;
            }
            else if (type == CONTROL_MESSAGE_MAP)
            {
                ControlMessageMap* p = (ControlMessageMap*)this->data.ref;
                delete p;
            }

            this->data.ref = NULL;
        }

        std::string ControlMessage::toString() const
        {
            std::stringstream s;
            std::string* s_p = NULL;

            s << "ControlMessage[type=";
            s << type2String(this->type);
            s << "; data=(";

            switch (this->type)
            {
                case CONTROL_MESSAGE_NULL:
                    break;
                case CONTROL_MESSAGE_BOOL:
                    s << this->data.b;
                    break;
                case CONTROL_MESSAGE_DOUBLE:
                    s << this->data.d;
                    break;
                case CONTROL_MESSAGE_LONG:
                    s << this->data.i;
                    break;
                case CONTROL_MESSAGE_STRING:
                    s_p = (std::string*)this->data.ref;
                    if (s_p == NULL)
                    {
                        s << "null string";
                    }
                    else
                    {
                        s << *s_p;
                    }
                    break;
                case CONTROL_MESSAGE_MAP:
                    s << this->mapToString();
                    break;
                case CONTROL_MESSAGE_LIST:
                    s << this->listToString();
                    break;
                default:
                    assert (false);
                    break;
            }
            s << ")]";
            return s.str();
        }

        std::string ControlMessage::type2String(int32_t type)
        {
            switch (type)
            {
                case CONTROL_MESSAGE_NULL:
                    return "null";
                case CONTROL_MESSAGE_BOOL:
                    return "bool";
                case CONTROL_MESSAGE_DOUBLE:
                    return "double";
                case CONTROL_MESSAGE_LONG:
                    return "long";
                case CONTROL_MESSAGE_STRING:
                    return "string";
                case CONTROL_MESSAGE_MAP:
                    return "map";
                case CONTROL_MESSAGE_LIST:
                    return "list";
                default:
                    assert (false);
                    return "unknown";
            }
        }

        std::string ControlMessage::listToString() const
        {
            ControlMessageList* controlMessageList = (ControlMessageList*)this->data.ref;
            ControlMessageList::iterator ip;
            std::stringstream s;

            if (controlMessageList == NULL)
            {
                return "null";
            }

            for (ip = controlMessageList->begin(); ip != controlMessageList->end(); ++ip)
            {
                if (ip != controlMessageList->begin())
                {
                    s << ", ";
                }

                s << (*ip)->toString();
            }

            return s.str();
        }

        std::string ControlMessage::mapToString() const
        {
            ControlMessageMap* controlMessageMap = (ControlMessageMap*)this->data.ref;
            std::map<ControlMessagePtr, ControlMessagePtr, ControlMessagePtrComparator>::iterator ip;
            std::stringstream s;

            if (controlMessageMap == NULL)
            {
                return "null";
            }

            for (ip = controlMessageMap->map_.begin(); ip != controlMessageMap->map_.end(); ++ip)
            {
                if (ip != controlMessageMap->map_.begin())
                {
                    s << ", ";
                }

                s << ip->first->toString();
                s << "=>";
                s << ip->second->toString();
            }

            return s.str();
        }
    }
}
