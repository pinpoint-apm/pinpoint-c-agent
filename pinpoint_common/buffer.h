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
#ifndef PINPOINT_BUFFER_H
#define PINPOINT_BUFFER_H

#include "stdint.h"
#include <deque>
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>

namespace Pinpoint
{
    namespace Agent
    {
        class ChannelBuffer
        {
        public:
            virtual void setBuffer(const std::string& buffer) = 0;
            virtual void reset() = 0;
            virtual void writeByte(int8_t i) = 0;
            virtual void writeShort(int16_t i) = 0;
            virtual void writeInt(int32_t i) = 0;
            virtual void writeLong(int64_t i) = 0;
            virtual void writeBytes(const std::vector<int8_t> &value) = 0;

            virtual int8_t readByte() = 0;
            virtual int16_t readShort() = 0;
            virtual int32_t readInt() = 0;
            virtual int64_t readLong() = 0;
            virtual void readBytes(std::vector<int8_t> &value, uint32_t size) = 0;

            virtual void appendPayload(const std::string &payload) = 0;
            virtual void appendBuffer(const std::string& buffer_) = 0;
            virtual std::string readPayLoad() = 0;

            virtual int32_t getSize() const = 0;

            virtual std::string getBuffer() const = 0;
            virtual std::string getBuffer(uint32_t size) const = 0;
        };

        class ChannelBufferException : public std::exception
        {
        public:
            ChannelBufferException(const ChannelBuffer& buffer, const std::string& funcName);

            virtual ~ChannelBufferException() throw () {}

            virtual const char* what() const throw()
            {
                return errMsg.c_str();
            }

        private:
            std::string errMsg;
        };


        class ChannelBufferV1 : public ChannelBuffer
        {
        public:
            ChannelBufferV1();

            void setBuffer(const std::string& buffer);

            void reset();

            void writeByte(int8_t i);
            void writeShort(int16_t i);
            void writeInt(int32_t i);
            void writeLong(int64_t i);
            void writeBytes(const std::vector<int8_t> &value);

            int8_t readByte();
            int16_t readShort();
            int32_t readInt();
            int64_t readLong();
            void readBytes(std::vector<int8_t> &value, uint32_t size);

            void appendPayload(const std::string &payload);
            void appendBuffer(const std::string& buffer_);
            std::string readPayLoad();

            int32_t getSize() const;

            std::string getBuffer() const;
            std::string getBuffer(uint32_t size) const;

        protected:
            void pop_front(int32_t size);

        private:
            std::deque<int8_t> buffer;
        };

        class ChannelBufferV2 : public ChannelBuffer
        {
        public:
            static const uint32_t DEFAULT_SIZE = 1024;

            ChannelBufferV2();

            bool ensureWriteCapacity(uint32_t length);
            bool ensureReadCapacity(uint32_t length) const;
            void updateWritePos(uint32_t length);

            void setBuffer(const std::string& buffer);
            void reset();


            void writeByte(int8_t i);
            void writeShort(int16_t i);
            void writeInt(int32_t i);
            void writeLong(int64_t i);
            void writeBytes(const std::vector<int8_t> &value);

            int8_t readByte();
            int16_t readShort();
            int32_t readInt();
            int64_t readLong();
            void readBytes(std::vector<int8_t> &value, uint32_t size);


            void appendPayload(const std::string &payload);
            void appendBuffer(const std::string& buffer_);
            std::string readPayLoad();

            int32_t peekPayLoadLength();
            int8_t peekByte();

            int32_t getSize() const;

            int8_t* getWriteAddr(uint32_t write_size);
            int8_t* getWriteAddr();

            std::string getBuffer() const;
            std::string getBuffer(uint32_t size) const;

            ~ChannelBufferV2();

        protected:
            inline uint32_t getWriteCapacity() { return size > writePos ? size - writePos : 0; }
            inline uint32_t getReadCapacity() const { return writePos > readPos ? writePos - readPos : 0; }
            void pop_front(uint32_t size);

        private:
            int8_t* array;
            uint32_t size;
            uint32_t writePos;
            uint32_t readPos;

        };


        class AutoBuffer
        {
        public:
            static const int32_t POINTER_NULL = -1;

            AutoBuffer();

            explicit AutoBuffer(const std::deque<int8_t> &buffer_);

            explicit AutoBuffer(const std::vector<int8_t> &buffer_);

            explicit AutoBuffer(const std::string& data);

            virtual std::vector<int8_t> getBuffer();

            virtual void putPrefixedString(const char *s);

            /* caution, the return pointer should be deleted. */
            virtual char* readPrefixedString();

            virtual void put(const std::string &src);

            virtual std::string readString(int32_t size);

            virtual void putSVar(int32_t v);

            virtual int32_t readSVarInt();

            virtual void putByte(int8_t b);

            virtual int8_t readByte();

            virtual void putLong(int64_t l);

            virtual int64_t readLong();

            virtual void putVar32(int32_t v);

            virtual int32_t readVarInt();

            virtual void putVar64(int64_t value);

            virtual int64_t readVarLong();

        private:

            inline static int32_t intToZigZag(int32_t n)
            {
                return (n << 1) ^ (n >> 31);
            }

            inline static int32_t zigzagToInt(int32_t n)
            {
                return ((uint32_t) n >> 1) ^ -(n & 1);
            }

            std::deque<int8_t> buffer;

            /* return the first item in buffer and remove it. */
            int8_t bufferFrontPop();

            // int32_t offset;
        };
    }
}

#endif
