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
#ifndef PINPOINT_TEST_ENV_H
#define PINPOINT_TEST_ENV_H

#include <assert.h>
#include <string>
#include <sstream>
#include <vector>
#include "stdint.h"
#include "pinpoint_error.h"
#include "pinpoint_api.h"

extern int32_t init_test_env();
extern void free_test_env();

extern std::stringstream& get_test_string_stream();
extern std::string& get_test_generic_string();

#define GENERIC_STR(s) \
    do \
    { \
        std::stringstream& stream = get_test_string_stream(); \
        std::string& genericString = get_test_generic_string(); \
        stream.str(""); \
        stream.clear(); \
        stream << (s) ; \
        stream >> genericString; \
    } \
    while(0)


#define ASSERT_EX(true_or_false, lhs, rhs) \
    do \
    {  \
        if (!(true_or_false)) \
        { \
            GENERIC_STR(lhs); \
            std::string lhsString = get_test_generic_string(); \
            GENERIC_STR(rhs); \
            std::string rhsString = get_test_generic_string(); \
            LOGE("ASSERT_EX, lhs=%s, rhs=%s", lhsString.c_str(), rhsString.c_str()); \
            assert(false); \
        } \
    }  \
    while(0)

extern void random_vector(std::vector<int32_t>& v, uint32_t len);

extern std::string random_string(uint32_t len, bool readable = true);

template <class T>
void check_vector(std::vector<T>& lhs, std::vector<T>& rhs)
{
    if (lhs.size() != rhs.size())
    {
        ASSERT_EX(lhs.size() == rhs.size(), lhs.size(), rhs.size());
    }

    for (uint32_t i = 0; i < lhs.size(); ++i)
    {
        if (lhs[i] != rhs[i])
        {
            LOGE("find error: i=%d", i);
        }
        ASSERT_EX(lhs[i] == rhs[i], lhs[i], rhs[i]);
    }
}

template <class T>
void check_vector_ignore_order(std::vector<T>& lhs, std::vector<T>& rhs)
{
    if (lhs.size() != rhs.size())
    {
        ASSERT_EX(lhs.size() == rhs.size(), lhs.size(), rhs.size());
    }

    sort(lhs.begin(), lhs.end());
    sort(rhs.begin(), rhs.end());

    for (uint32_t i = 0; i < lhs.size(); ++i)
    {
        if (lhs[i] != rhs[i])
        {
            LOGE("find error: i=%d", i);
        }
        ASSERT_EX(lhs[i] == rhs[i], lhs[i], rhs[i]);
    }
}


#define INIT_VECTOR(vec, type, ...) \
    do {\
        vec.clear(); \
        type arr[] = {__VA_ARGS__}; \
        copy(arr, arr + sizeof(arr) / sizeof(arr[0]), std::back_inserter(vec)); \
    }\
    while(0)


extern bool probability(uint32_t percent);

extern int64_t rand64();

#endif
