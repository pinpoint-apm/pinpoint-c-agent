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
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "test_env.h"
#include "pinpoint_api.h"


void test_split_string(const std::string&src,
                       const std::vector<std::string>& answer,
                       const std::string& delimiter)
{
    std::vector<std::string> result;
    Pinpoint::utils::split_string(src, result, delimiter);
    check_vector(result, const_cast<std::vector<std::string>&>(answer));
}

TEST(utility_test, safe_strlen)
{
    int32_t len = Pinpoint::utils::safe_strlen(NULL);
    ASSERT_EX((len == -1), len, -1);
}

TEST(utility_test, split_string)
{
    std::vector<std::string> vec;

    // 1, null
    test_split_string("", vec, ";");

    // 2,
    INIT_VECTOR(vec, std::string, "abc", "123", "tt");
    test_split_string("abc;123;tt", vec, ";");

    INIT_VECTOR(vec, std::string, "", "abc", "123", "tt");
    test_split_string(";abc;123;tt", vec, ";");

    INIT_VECTOR(vec, std::string, "abc", "123", "tt", "");
    test_split_string("abc;123;tt;", vec, ";");

    INIT_VECTOR(vec, std::string, "abc", "123", "tt");
    test_split_string("abc;;123;;tt", vec, ";;");
}

TEST(utility_test, double_long_conv)
{
    for(int i = 0; i < 10; i++)
    {
        int64_t a = rand64();
        double da = Pinpoint::utils::long_bits_to_double(a);
        int64_t aa = Pinpoint::utils::double_to_long_bits(da);
        ASSERT_EX((a == aa), a, aa);
    }

}

TEST(utility_test, fixed_length_queue_test)
{
    int32_t err;
    Pinpoint::utils::FixedLengthQueue<int32_t> fixedLengthQueue1(1);
    assert (fixedLengthQueue1.capacity() == 1);
    assert (fixedLengthQueue1.size() == 0);
    assert(fixedLengthQueue1.empty());
    err = fixedLengthQueue1.push_back(1);
    assert (err == Pinpoint::SUCCESS);
    assert (fixedLengthQueue1.size() == 1);
    err = fixedLengthQueue1.push_back(2);
    assert (err == Pinpoint::QUEUE_FULL);

    int32_t v1;
    err = fixedLengthQueue1.front(v1);
    assert (err == Pinpoint::SUCCESS);
    assert(v1 == 1);

    err = fixedLengthQueue1.pop_front(v1);
    assert (err == Pinpoint::SUCCESS);
    assert(v1 == 1);

    err = fixedLengthQueue1.front(v1);
    assert (err == Pinpoint::QUEUE_EMPTY);

    err = fixedLengthQueue1.pop_front(v1);
    assert (err == Pinpoint::QUEUE_EMPTY);

    // random test

    srand((unsigned int)Pinpoint::utils::get_current_microsec_stamp64());

    std::deque<int32_t> dq;
    Pinpoint::utils::FixedLengthQueue<int32_t> fixedLengthQueue2(50);
    for (int i = 0; i < 1000; ++i)
    {
        int32_t err;
        int32_t op = rand() % 3;
        if (op == 0) // push back
        {
            int32_t value = rand();
            err = fixedLengthQueue2.push_back(value);
            if (err == Pinpoint::SUCCESS)
            {
                dq.push_back(value);
            }
        }
        else if (op == 1)
        {
            int32_t value = rand();
            err = fixedLengthQueue2.push_front(value);
            if (err == Pinpoint::SUCCESS)
            {
                dq.push_front(value);
            }
        }
        else // pop
        {
            int32_t value;
            err = fixedLengthQueue2.pop_front(value);
            if (err == Pinpoint::QUEUE_EMPTY)
            {
                assert (dq.empty());
            }
            else
            {
                int32_t value2 = dq.front();
                assert (value == value2);
                dq.pop_front();
            }
        }
    }

    for (std::deque<int32_t>::iterator ip = dq.begin(); ip != dq.end(); ++ip)
    {
        int32_t value;
        int32_t err = fixedLengthQueue2.pop_front(value);
        assert (err == Pinpoint::SUCCESS);
        assert (value == *ip);
    }
}

TEST(utility_test, base64)
{
    std::string raw = "abcdefg12345678900191928372847384\n\t:";
    std::string coded;
    std::string decoded;
    assert(Pinpoint::utils::Base64Encode(coded, raw));
    assert(Pinpoint::utils::Base64Decode(decoded, coded));
    ASSERT_EX((raw == decoded), raw, decoded);
}

TEST(utility_test,dotsec_to_milisec)
{
    using Pinpoint::utils::dotsec_to_milisec;
    EXPECT_EQ(0,dotsec_to_milisec(NULL));
    EXPECT_EQ(1000,dotsec_to_milisec("1.0"));
    EXPECT_EQ(1001,dotsec_to_milisec("1.1"));
    EXPECT_EQ(999,dotsec_to_milisec("1.-1"));
    EXPECT_EQ(0,dotsec_to_milisec("-1.1"));
    EXPECT_EQ(0,dotsec_to_milisec("-abc.d"));

}



