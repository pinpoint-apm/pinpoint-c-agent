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
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "test_env.h"
#include "buffer.h"

using Pinpoint::Agent::ChannelBuffer;
using Pinpoint::Agent::ChannelBufferV1;
using Pinpoint::Agent::ChannelBufferV2;
using Pinpoint::Agent::ChannelBufferException;
using Pinpoint::Agent::AutoBuffer;


void test_buffer(ChannelBuffer& buffer1, ChannelBuffer& buffer2)
{
    // 1, byte
    do
    {
        buffer1.reset();
        buffer2.reset();

        buffer1.writeByte(0);
        buffer1.writeByte(1);
        buffer1.writeByte(255);

        buffer2.setBuffer(buffer1.getBuffer());
        ASSERT_TRUE(buffer2.readByte() == 0);
        ASSERT_TRUE(buffer2.readByte() == 1);
        ASSERT_TRUE(buffer2.readByte() == (int8_t) 255);
    } while(0);

    // 2, short
    do
    {
        buffer1.reset();
        buffer2.reset();

        buffer1.writeShort(0);
        buffer1.writeShort(1);
        buffer1.writeShort(-1);

        buffer2.setBuffer(buffer1.getBuffer());
        ASSERT_TRUE(buffer2.readShort() == 0);
        ASSERT_TRUE(buffer2.readShort() == 1);
        ASSERT_TRUE(buffer2.readShort() == -1);
    } while(0);

    // 3, int
    do
    {
        buffer1.reset();
        buffer2.reset();

        buffer1.writeInt(0);
        buffer1.writeInt(1);
        buffer1.writeInt(-1);

        buffer2.setBuffer(buffer1.getBuffer());
        ASSERT_TRUE(buffer2.readInt() == 0);
        ASSERT_TRUE(buffer2.readInt() == 1);
        ASSERT_TRUE(buffer2.readInt() == -1);
    } while(0);

    // 4, long
    do
    {
        buffer1.reset();
        buffer2.reset();

        buffer1.writeLong(0);
        buffer1.writeLong(1);
        buffer1.writeLong(-1);

        buffer2.setBuffer(buffer1.getBuffer());
        ASSERT_TRUE(buffer2.readLong() == 0);
        ASSERT_TRUE(buffer2.readLong() == 1);
        ASSERT_TRUE(buffer2.readLong() == -1);
    } while(0);

    // 5, bytes
    do
    {
        buffer1.reset();
        buffer2.reset();

        std::vector<int8_t> data1, data2;
        data1.push_back(0);
        data1.push_back(1);
        data1.push_back(-1);

        buffer1.writeBytes(data1);

        buffer2.setBuffer(buffer1.getBuffer());
        buffer2.readBytes(data2, (uint32_t)data1.size());

        check_vector(data1, data2);
    } while(0);

    // 5, payLoad
    do
    {
        buffer1.reset();
        buffer2.reset();

        std::string s1, s2;
        s1 = "abcd1234";

        buffer1.appendPayload(s1);

        buffer2.setBuffer(buffer1.getBuffer());
        s2 = buffer2.readPayLoad();

        ASSERT_TRUE(s1 == s2);
    } while(0);

    // 6,
    do
    {
        buffer1.reset();
        buffer2.reset();

        std::string s1, s2;
        s1 = "1234abcd";

        buffer1.appendPayload(s1);
        buffer2.setBuffer(buffer1.getBuffer());

        ASSERT_TRUE(buffer1.getSize() == buffer2.getSize());
        ASSERT_TRUE(buffer1.getBuffer() == buffer2.getBuffer());
        ASSERT_TRUE(buffer1.getBuffer(4) == buffer2.getBuffer(4));

        buffer1.appendBuffer(buffer2.getBuffer());
        buffer2.appendBuffer(buffer2.getBuffer());

        ASSERT_TRUE(buffer1.getSize() == buffer2.getSize());
        ASSERT_TRUE(buffer1.getBuffer() == buffer2.getBuffer());
        ASSERT_TRUE(buffer1.getBuffer(4) == buffer2.getBuffer(4));

        s2 = buffer2.readPayLoad();
        s2 = buffer2.readPayLoad();
        ASSERT_TRUE(s1 == s2);

    } while(0);
}


TEST(buffer_test, basic_test)
{
    ChannelBufferV1 bufferV1;
    ChannelBufferV2 bufferV2;

    test_buffer(bufferV1, bufferV1);
    test_buffer(bufferV2, bufferV2);
    test_buffer(bufferV1, bufferV2);
    test_buffer(bufferV2, bufferV1);

}

TEST(buffer_test, channel_buffer_v2_expand)
{
    std::string nullStr;
    std::string str = random_string(5000);
    int32_t a = 100;
    ChannelBufferV2 bufferV2;
    bufferV2.writeInt(a);
    bufferV2.appendPayload(str);
    bufferV2.appendPayload(nullStr);

    int32_t ra = bufferV2.readInt();
    ASSERT_EX((a == ra), a, ra);

    std::string rstr = bufferV2.readPayLoad();
    ASSERT_EX((str == rstr), str, rstr);

    std::string rNullStr = bufferV2.readPayLoad();
    ASSERT_EX((nullStr == rNullStr), nullStr, rNullStr);
}


#define TEST_CATCH_EXCEPTION(func) do { \
    try \
    { \
        func; \
    } \
    catch (std::exception& e) \
    { \
        LOGI("get exception=%s", e.what()); \
    } \
} while(0);


TEST(buffer_test, channel_buffer_exception)
{
    ChannelBufferV2 bufferV2;
    std::vector<int8_t> value;
    TEST_CATCH_EXCEPTION((bufferV2.getBuffer(1)));
    TEST_CATCH_EXCEPTION((bufferV2.peekByte()));
    TEST_CATCH_EXCEPTION((bufferV2.peekPayLoadLength()));
    TEST_CATCH_EXCEPTION((bufferV2.readByte()));
    TEST_CATCH_EXCEPTION((bufferV2.readShort()));
    TEST_CATCH_EXCEPTION((bufferV2.readInt()));
    TEST_CATCH_EXCEPTION((bufferV2.readLong()));
    TEST_CATCH_EXCEPTION((bufferV2.readBytes(value, 10)));
    TEST_CATCH_EXCEPTION((bufferV2.readPayLoad()));

    bufferV2.appendPayload("abcdef");
    TEST_CATCH_EXCEPTION((bufferV2.getBuffer(1)));
    TEST_CATCH_EXCEPTION((bufferV2.peekByte()));
    TEST_CATCH_EXCEPTION((bufferV2.peekPayLoadLength()));
    TEST_CATCH_EXCEPTION((bufferV2.readByte()));
    TEST_CATCH_EXCEPTION((bufferV2.readShort()));
    TEST_CATCH_EXCEPTION((bufferV2.readInt()));
    TEST_CATCH_EXCEPTION((bufferV2.readLong()));
    TEST_CATCH_EXCEPTION((bufferV2.readBytes(value, 10)));
    TEST_CATCH_EXCEPTION((bufferV2.readPayLoad()));
}



TEST(buffer_test, auto_buffer_test)
{
    AutoBuffer buffer;

    // prefixed string
    {
        const char* s = "testabc";
        buffer.putPrefixedString(s);

        char* r = buffer.readPrefixedString();

        ASSERT_EX((strcmp(s, r) == 0), s, r);
        free(r);

        buffer.putPrefixedString(NULL);
        r = buffer.readPrefixedString();

        assert (r == NULL);

        const char* s1 = "";
        buffer.putPrefixedString(s1);
        r = buffer.readPrefixedString();
        ASSERT_EX((strcmp(s1, r) == 0), s, r);
        free(r);
    }

    // string
    {
        std::string s = "abcdef";
        buffer.put(s);
        std::string r = buffer.readString(s.size());
        ASSERT_EX((s == r), s, r);

        s = "";
        buffer.put(s);
        r = buffer.readString(s.size());
        ASSERT_EX((s == r), s, r);
    }

    // SVar
    {
        int32_t a = 100;
        buffer.putSVar(a);
        int32_t b = buffer.readSVarInt();
        ASSERT_EX((a == b), a, b);
    }

    // byte
    {
        int8_t a = 23;
        buffer.putByte(a);
        int8_t b = buffer.readByte();
        ASSERT_EX((a == b), a, b);
    }

    // long
    {
        int64_t a = -1;
        buffer.putLong(a);
        int64_t b = buffer.readLong();
        ASSERT_EX((a == b), a, b);
    }

    // var32
    {
        int32_t a = -1;
        buffer.putVar32(a);
        int32_t b = buffer.readVarInt();
        ASSERT_EX((a == b), a, b);
    }

    // var64
    {
        int64_t a = -1;
        buffer.putVar64(a);
        int64_t b = buffer.readVarLong();
        ASSERT_EX((a == b), a, b);
    }

    // copy
    {
        buffer.putPrefixedString("abcd");
        buffer.putSVar(-1);
        std::vector<int8_t> vb = buffer.getBuffer();
        std::deque<int8_t> db(vb.begin(), vb.end());
        std::string s(vb.begin(), vb.end());

        AutoBuffer buffer1(vb);
        AutoBuffer buffer2(db);
        AutoBuffer buffer3(s);

        std::vector<int8_t> v1 = buffer1.getBuffer();
        std::vector<int8_t> v2 = buffer2.getBuffer();
        std::vector<int8_t> v3 = buffer3.getBuffer();

        check_vector(v1, v2);
        check_vector(v1, v3);
    }


}
