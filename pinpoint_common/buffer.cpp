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
#include <assert.h>
#include <string.h>
#include "buffer.h"
#include "utility.h"

using namespace std;

#define CHANNEL_BUFFER_THROW do {\
            throw Pinpoint::Agent::ChannelBufferException(*this, __func__); \
        } while(0);

namespace Pinpoint
{
    namespace Agent
    {
        ChannelBufferException::ChannelBufferException(const ChannelBuffer &buffer, const std::string &funcName)
        {
            try
            {
                std::string base64Buffer;
                utils::Base64Encode(base64Buffer, buffer.getBuffer());
                std::stringstream ss;
                ss << funcName << " throw: bufferSize=" << buffer.getSize() << ", buffer=[" << base64Buffer << "].";
                errMsg = ss.str();
            }
            catch (...)
            {
                errMsg = "unknown exception!!!";
            }
        }

        //<editor-fold desc="ChannelBufferV1">

        ChannelBufferV1::ChannelBufferV1()
        {

        }

        void ChannelBufferV1::reset()
        {
            buffer.clear();
        }

        void ChannelBufferV1::setBuffer(const std::string &buffer_)
        {
            this->buffer.clear();
            std::copy(buffer_.begin(), buffer_.end(), std::back_inserter(this->buffer));
        }


        void ChannelBufferV1::writeByte(int8_t i)
        {
            buffer.push_back(i);
        }

        void ChannelBufferV1::writeShort(int16_t i)
        {
            uint16_t ui = (uint16_t) i;

            buffer.push_back((int8_t) (ui >> 8));
            buffer.push_back((int8_t) (ui));
        }

        void ChannelBufferV1::writeInt(int32_t i)
        {
            uint32_t ui = (uint32_t) i;

            buffer.push_back((int8_t) (ui >> 24));
            buffer.push_back((int8_t) (ui >> 16));
            buffer.push_back((int8_t) (ui >> 8));
            buffer.push_back((int8_t) (ui));
        }

        void ChannelBufferV1::writeLong(int64_t i)
        {
            uint64_t ui = (uint64_t) i;

            buffer.push_back((int8_t) (ui >> 56));
            buffer.push_back((int8_t) (ui >> 48));
            buffer.push_back((int8_t) (ui >> 40));
            buffer.push_back((int8_t) (ui >> 32));
            buffer.push_back((int8_t) (ui >> 24));
            buffer.push_back((int8_t) (ui >> 16));
            buffer.push_back((int8_t) (ui >> 8));
            buffer.push_back((int8_t) (ui));
        }

        void ChannelBufferV1::writeBytes(const std::vector<int8_t> &value)
        {
            buffer.insert(buffer.end(), value.begin(), value.end());
        }


        void ChannelBufferV1::appendPayload(const std::string &payload)
        {
            int32_t length = (int32_t) payload.size();

            writeInt(length);

            if (length > 0)
            {
                buffer.insert(buffer.end(), payload.begin(), payload.end());
            }
        }

        void ChannelBufferV1::appendBuffer(const std::string &buffer_)
        {
            buffer.insert(buffer.end(), buffer_.begin(), buffer_.end());
        }

        std::string ChannelBufferV1::readPayLoad()
        {
            int32_t length = readInt();

            assert (length <= (int32_t)buffer.size());

            std::string payLoad = std::string(buffer.begin(), buffer.begin() + length);
            pop_front(length);
            return payLoad;
        }

        std::string ChannelBufferV1::getBuffer() const
        {
            return std::string(buffer.begin(), buffer.end());
        }

        std::string ChannelBufferV1::getBuffer(uint32_t size) const
        {
            assert ((uint32_t)buffer.size() >= size);
            return std::string(buffer.begin(), buffer.begin() + size);
        }

        int32_t ChannelBufferV1::getSize() const
        {
            return (int32_t)this->buffer.size();
        }

        int8_t ChannelBufferV1::readByte()
        {
            assert (buffer.size() >= 1);
            int8_t res = buffer.front();
            buffer.pop_front();
            return res;
        }

        int16_t ChannelBufferV1::readShort()
        {
            assert (buffer.size() >= 2);
            uint8_t r1 = (uint8_t)readByte();
            uint8_t r2 = (uint8_t)readByte();
            return (int16_t)(((uint16_t)r1 << 8) | r2);
        }

        int32_t ChannelBufferV1::readInt()
        {
            assert (buffer.size() >= 4);
            uint8_t r1 = (uint8_t)readByte();
            uint8_t r2 = (uint8_t)readByte();
            uint8_t r3 = (uint8_t)readByte();
            uint8_t r4 = (uint8_t)readByte();
            uint32_t res = ((uint32_t)r1 << 24) | ((uint32_t)r2 << 16) | ((uint32_t)r3 << 8) | r4;
            return (int32_t)res;
        }

        int64_t ChannelBufferV1::readLong()
        {
            assert (buffer.size() >= 8);
            uint32_t r1 = (uint32_t)readInt();
            uint32_t r2 = (uint32_t)readInt();

            uint64_t res = ((uint64_t)r1 << 32) | ((uint64_t)r2);

            return (int64_t)res;
        }

        void ChannelBufferV1::readBytes(std::vector<int8_t> &value, uint32_t size)
        {
            assert ((uint32_t)buffer.size() >= size);
            copy(buffer.begin(), buffer.begin() + size, std::back_inserter(value));
            buffer.erase(buffer.begin(), buffer.begin() + size);
        }

        void ChannelBufferV1::pop_front(int32_t size)
        {
            assert ((int32_t)buffer.size() >= size);
            buffer.erase(buffer.begin(), buffer.begin() + size);
        }

        //</editor-fold>

        //<editor-fold desc="AutoBuffer">

        AutoBuffer::AutoBuffer()
        {

        }

        AutoBuffer::AutoBuffer(const deque<int8_t> &buffer_) : buffer(buffer_)
        {

        }

        AutoBuffer::AutoBuffer(const vector<int8_t> &buffer_) : buffer(buffer_.begin(), buffer_.end())
        {

        }

        AutoBuffer::AutoBuffer(const std::string &data) : buffer(data.begin(), data.end())
        {

        }

        vector<int8_t> AutoBuffer::getBuffer()
        {
            return vector<int8_t>(buffer.begin(), buffer.end());
        }

        void AutoBuffer::putPrefixedString(const char *s)
        {
            if (s == NULL)
            {
                putSVar(POINTER_NULL);
            }
            else
            {
                string src(s);
                putSVar((int32_t)src.length());
                put(src);
            }
        }

        int8_t AutoBuffer::bufferFrontPop()
        {
            int8_t back = this->buffer.front();
            this->buffer.pop_front();
            return back;
        }

        int32_t AutoBuffer::readVarInt()
        {
            int32_t result = 0;

            /* 32/7 + 1, varInt occupy 6 byte. */

            for (int i = 0; i < 6; i++)
            {
                int8_t value;
                value = bufferFrontPop();
                result = result | (uint32_t) ((uint8_t) value & (uint8_t) 0x7f) << (7 * i);
                if (value >= 0)
                {
                    return result;
                }
            }

            assert(false);
            return result;
        }

        void AutoBuffer::putVar32(int32_t v)
        {
            int32_t value = v;

            while (true)
            {
                if ((value & ~0x7F) == 0)
                {
                    buffer.push_back((int8_t) value);
                    break;
                }
                else
                {
                    buffer.push_back((int8_t) (((uint8_t) value & (uint8_t) 0x7F) | (uint8_t) 0x80));
                    value = (int32_t) (((uint32_t) value) >> 7);
                }
            }
        }

        char *AutoBuffer::readPrefixedString()
        {
            int32_t size = readSVarInt();

            if (size == POINTER_NULL)
            {
                return NULL;
            }
            else if (size == 0)
            {
                char *s = (char*)::malloc(1);
                if (s == NULL)
                {
                    return NULL;
                }
                s[0] = '\0';
                return s;
            }
            string result = readString(size);

            return strdup(result.c_str());
        }

        string AutoBuffer::readString(int32_t size)
        {
            assert (buffer.begin() + size <= buffer.end());
            string result(buffer.begin(), buffer.begin() + size);

            buffer.erase(buffer.begin(), buffer.begin() + size);

            return result;
        }

        void AutoBuffer::put(const string &src)
        {
            for (uint32_t i = 0; i < src.size(); i++)
            {
                this->buffer.push_back(src[i]);
            }
        }

        void AutoBuffer::putSVar(int32_t v)
        {
            int32_t value = AutoBuffer::intToZigZag(v);

            putVar32(value);
        }

        int32_t AutoBuffer::readSVarInt()
        {
            return AutoBuffer::zigzagToInt(readVarInt());
        }

        void AutoBuffer::putByte(int8_t b)
        {
            buffer.push_back(b);
        }

        int8_t AutoBuffer::readByte()
        {
            return bufferFrontPop();
        }

        void AutoBuffer::putLong(int64_t value)
        {
            int8_t test = (int8_t) (value >> 56);
            (void) test;
            buffer.push_back((int8_t) (value >> 56));
            buffer.push_back((int8_t) (value >> 48));
            buffer.push_back((int8_t) (value >> 40));
            buffer.push_back((int8_t) (value >> 32));
            buffer.push_back((int8_t) (value >> 24));
            buffer.push_back((int8_t) (value >> 16));
            buffer.push_back((int8_t) (value >> 8));
            buffer.push_back((int8_t) (value));
        }

        int64_t AutoBuffer::readLong()
        {
            int64_t result = 0;
            for (int i = 0; i < 8; i++)
            {
                result = (result << (int64_t) 8) | (uint8_t) bufferFrontPop();
            }
            return result;
        }

        void AutoBuffer::putVar64(int64_t value)
        {

            while (true)
            {
                if ((value & ~0x7FL) == 0)
                {
                    buffer.push_back((int8_t) value);
                    break;
                }
                else
                {
                    buffer.push_back((int8_t) ((value & 0x7F) | 0x80));
                    value = ((uint64_t) value) >> 7;
                }
            }
        }

        int64_t AutoBuffer::readVarLong()
        {
            /* 64/7, So a varLong can occupy 10 byte. */
            int64_t result = 0;

            for (int i = 0; i < 10; i++)
            {
                int8_t value;
                value = bufferFrontPop();
                result = (uint64_t) (value & 0x7f) << (7 * i) | result;
                if (value >= 0)
                {
                    return result;
                }
            }

            assert(false);
            return result;
        }
        //</editor-fold>

        //<editor-fold desc="ChannelBufferV2">

        ChannelBufferV2::ChannelBufferV2()
        {
            this->readPos = 0;
            this->writePos = 0;
            this->size = 0;
            this->array = NULL;
        }

        ChannelBufferV2::~ChannelBufferV2()
        {
            if (this->array != NULL)
            {
                delete[] this->array;
                this->array = NULL;
            }
        }

        bool ChannelBufferV2::ensureWriteCapacity(uint32_t length)
        {
            uint32_t old_size = size;
            while (getWriteCapacity() < length)
            {
                size = size == 0 ? DEFAULT_SIZE : 2 * size;
            }

            if (old_size < size)
            {
                int8_t* new_array = NULL;
                try
                {
                    new_array = new int8_t[size];
                }
                catch (...)
                {
                    LOGE("expend ChannelBufferV2 failed.")
                    return false;
                }

                if (this->array != NULL)
                {
                    memcpy(new_array, this->array, old_size);
                    delete this->array;
                }
                this->array = new_array;
            }

            return true;
        }

        bool ChannelBufferV2::ensureReadCapacity(uint32_t length) const
        {
            return length <= getReadCapacity();
        }

        void ChannelBufferV2::reset()
        {
            this->readPos = 0;
            this->writePos = 0;
        }

        void ChannelBufferV2::updateWritePos(uint32_t length)
        {
            this->writePos += length;
        }

        void ChannelBufferV2::setBuffer(const std::string &buffer)
        {
            reset();
            uint32_t new_size = (uint32_t)buffer.size();
            if (!ensureWriteCapacity(new_size))
            {
                CHANNEL_BUFFER_THROW;
            }

            std::copy(buffer.begin(), buffer.end(), this->array + this->writePos);

            this->writePos += new_size;
        }

        int8_t* ChannelBufferV2::getWriteAddr(uint32_t write_size)
        {
            uint32_t new_size = write_size;
            if (!ensureWriteCapacity(new_size))
            {
                CHANNEL_BUFFER_THROW;
            }
            int8_t* writeAddr = this->array + writePos;
            this->writePos += new_size;
            return writeAddr;
        }

        int8_t* ChannelBufferV2::getWriteAddr()
        {
            return this->array + writePos;
        }

        void ChannelBufferV2::writeByte(int8_t i)
        {
            uint32_t new_size = 1;
            if (!ensureWriteCapacity(new_size))
            {
                CHANNEL_BUFFER_THROW;
            }
            this->array[this->writePos] = i;
            this->writePos += new_size;
        }

        void ChannelBufferV2::writeShort(int16_t i)
        {
            uint16_t ui = (uint16_t) i;

            writeByte((int8_t) (ui >> 8));
            writeByte((int8_t) (ui));
        }

        void ChannelBufferV2::writeInt(int32_t i)
        {
            uint32_t ui = (uint32_t) i;

            writeByte((int8_t) (ui >> 24));
            writeByte((int8_t) (ui >> 16));
            writeByte((int8_t) (ui >> 8));
            writeByte((int8_t) (ui));
        }

        void ChannelBufferV2::writeLong(int64_t i)
        {
            uint64_t ui = (uint64_t) i;

            writeByte((int8_t) (ui >> 56));
            writeByte((int8_t) (ui >> 48));
            writeByte((int8_t) (ui >> 40));
            writeByte((int8_t) (ui >> 32));
            writeByte((int8_t) (ui >> 24));
            writeByte((int8_t) (ui >> 16));
            writeByte((int8_t) (ui >> 8));
            writeByte((int8_t) (ui));
        }

        void ChannelBufferV2::writeBytes(const std::vector<int8_t> &value)
        {
            uint32_t new_size = (uint32_t)value.size();
            if (!ensureWriteCapacity(new_size))
            {
                CHANNEL_BUFFER_THROW;
            }

            std::copy(value.begin(), value.end(), this->array + this->writePos);

            this->writePos += new_size;
        }

        void ChannelBufferV2::appendPayload(const std::string &payload)
        {
            int32_t length = (int32_t) payload.size();

            writeInt(length);

            if (length > 0)
            {
                appendBuffer(payload);
            }
        }

        void ChannelBufferV2::appendBuffer(const std::string &buffer_)
        {
            uint32_t new_size = (uint32_t)buffer_.size();
            if (!ensureWriteCapacity(new_size))
            {
                CHANNEL_BUFFER_THROW;
            }

            std::copy(buffer_.begin(), buffer_.end(), this->array + this->writePos);
            this->writePos += new_size;
        }

        int8_t ChannelBufferV2::readByte()
        {
            uint32_t read_size = 1;
            if (!ensureReadCapacity(read_size))
            {
                CHANNEL_BUFFER_THROW;
            }

            int8_t b = this->array[this->readPos];
            this->readPos += read_size;
            return b;
        }

        int16_t ChannelBufferV2::readShort()
        {
            uint32_t read_size = 2;
            if (!ensureReadCapacity(read_size))
            {
                CHANNEL_BUFFER_THROW;
            }

            uint8_t r1 = (uint8_t)readByte();
            uint8_t r2 = (uint8_t)readByte();
            return (int16_t)(((uint16_t)r1 << 8) | r2);
        }

        int32_t ChannelBufferV2::readInt()
        {
            uint32_t read_size = 4;
            if (!ensureReadCapacity(read_size))
            {
                CHANNEL_BUFFER_THROW;
            }

            uint8_t r1 = (uint8_t)readByte();
            uint8_t r2 = (uint8_t)readByte();
            uint8_t r3 = (uint8_t)readByte();
            uint8_t r4 = (uint8_t)readByte();
            uint32_t res = ((uint32_t)r1 << 24) | ((uint32_t)r2 << 16) | ((uint32_t)r3 << 8) | r4;

            return (int32_t)res;
        }

        int64_t ChannelBufferV2::readLong()
        {
            uint32_t read_size = 8;
            if (!ensureReadCapacity(read_size))
            {
                CHANNEL_BUFFER_THROW;
            }

            uint32_t r1 = (uint32_t)readInt();
            uint32_t r2 = (uint32_t)readInt();

            uint64_t res = ((uint64_t)r1 << 32) | ((uint64_t)r2);

            return (int64_t)res;
        }

        void ChannelBufferV2::readBytes(std::vector<int8_t> &value, uint32_t size)
        {
            uint32_t read_size = size;
            if (!ensureReadCapacity(read_size))
            {
                CHANNEL_BUFFER_THROW;
            }

            copy(this->array + this->readPos, this->array + this->readPos + size, std::back_inserter(value));

            this->readPos += read_size;
        }

        void ChannelBufferV2::pop_front(uint32_t size)
        {
            uint32_t read_size = size;
            if (!ensureReadCapacity(read_size))
            {
                CHANNEL_BUFFER_THROW;
            }

            this->readPos += read_size;
        }

        std::string ChannelBufferV2::readPayLoad()
        {
            int32_t length = readInt();

            if (!ensureReadCapacity((uint32_t)length))
            {
                CHANNEL_BUFFER_THROW;
            }

            std::string payLoad(this->array + this->readPos,  this->array + this->readPos + length);
            pop_front((uint32_t)length);

            return payLoad;
        }

        int32_t ChannelBufferV2::peekPayLoadLength()
        {
            uint32_t read_size = 4;
            if (!ensureReadCapacity(read_size))
            {
                CHANNEL_BUFFER_THROW;
            }

            uint8_t r1 = (uint8_t)this->array[this->readPos];
            uint8_t r2 = (uint8_t)this->array[this->readPos + 1];
            uint8_t r3 = (uint8_t)this->array[this->readPos + 2];
            uint8_t r4 = (uint8_t)this->array[this->readPos + 3];
            uint32_t res = ((uint32_t)r1 << 24) | ((uint32_t)r2 << 16) | ((uint32_t)r3 << 8) | r4;

            return (int32_t)res;
        }

        int8_t ChannelBufferV2::peekByte()
        {
            uint32_t read_size = 1;
            if (!ensureReadCapacity(read_size))
            {
                CHANNEL_BUFFER_THROW;
            }

            return this->array[this->readPos];
        }

        int32_t ChannelBufferV2::getSize() const
        {
            return this->writePos - this->readPos;
        }

        std::string ChannelBufferV2::getBuffer() const
        {
            return std::string(this->array + this->readPos,  this->array + this->writePos);
        }

        std::string ChannelBufferV2::getBuffer(uint32_t size) const
        {
            uint32_t read_size = size;
            if (!ensureReadCapacity(read_size))
            {
                CHANNEL_BUFFER_THROW;
            }

            return std::string(this->array + this->readPos,  this->array + size);
        }
        //</editor-fold>
    }
}


