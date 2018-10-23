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
#include <pinpoint_error.h>
#include <cstdio>
#include <cassert>
#include <sstream>
#include "test_env.h"
#include "log_utility.h"
#include <boost/thread.hpp>

using namespace Pinpoint;
using namespace Pinpoint::log;

// for test-case
uint64_t test_log_output(const char* msg, uint32_t)
{
    fprintf(stderr, "%s", msg);
    return 0;
}

int32_t init_test_env()
{
    int32_t err = SUCCESS;
    err = LogUtility::initLogUtility(NULL, Agent::LogOutputFunc(test_log_output));
	
    assert (err == SUCCESS);
//<&-*> bluse  Mar 29, 2017
//fixed unitest ignore DUBUG info
    LogUtility::getInstance()->setSystemLogLevel(PP_LOG_DEBUG);

    return err;
}

void free_test_env()
{
    return;
}

std::stringstream& get_test_string_stream()
{
    static boost::thread_specific_ptr<std::stringstream> tls;
    if (!tls.get())
    {
        tls.reset(new std::stringstream);
    }

    return *tls;
}

std::string& get_test_generic_string()
{
    static boost::thread_specific_ptr<std::string> tls;
    if (!tls.get())
    {
        tls.reset(new std::string);
    }

    return *tls;
}

void random_vector(std::vector<int32_t>& v, uint32_t len)
{
    v.clear();
    v.reserve(len);

    srand((unsigned int)Pinpoint::utils::get_current_microsec_stamp64());

    for (uint32_t i = 0; i < len; ++i)
    {
        v.push_back(::rand() % 1000);
    }
}


static char random_char(bool readable)
{
    const char readables[] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    srand((unsigned int)Pinpoint::utils::get_current_microsec_stamp64());

    int r = ::rand();

    if(readable)
    {
        return readables[r % sizeof(readables)];
    }

    return (char)r;
}

std::string random_string(uint32_t len, bool readable)
{
    std::string res;
    for (int i = 0; i < len; ++i)
    {
        res += random_char(readable);
    }
    return res;
}

bool probability(uint32_t percent)
{
    assert (0 <= percent && percent <= 100);

    uint32_t dice = (uint32_t)::rand() % 100;

    return  dice < percent;
}

int64_t rand64()
{
    uint64_t num;

    srand((unsigned int)Pinpoint::utils::get_current_microsec_stamp64());

    num = (uint64_t)rand();
    num = (num << 32) | rand();

    return (int64_t)num;
}
