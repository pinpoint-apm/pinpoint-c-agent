////////////////////////////////////////////////////////////////////////////////
// Copyright 2020 NAVER Corp
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
/*
 * test_util.cc
 *
 *  Created on: Sep 15, 2020
 *      Author: eeliu
 */

#include <gtest/gtest.h>
#include <thread>
#include "common.h"
#include "Util/Helper.h"
#include "Cache/SafeSharedState.h"
#include "NodePool/PoolManager.h"
#include <stdarg.h>
using Cache::SafeSharedState;
using Helper::STT;
using std::chrono::seconds;
namespace Json = AliasJson;
using NodePool::PoolManager;
using NodePool::TraceNode;
TEST(util, sst)
{
    ADDTRACE();

    std::this_thread::sleep_for(seconds(1));
}

TEST(util, onOFFline)
{
    SafeSharedState &_state = SafeSharedState::instance();
    EXPECT_FALSE(_state.isReady());
    _state.updateStartTime(1316615272);
    // EXPECT_EQ(1316615272, _state.getStartTime());
    EXPECT_TRUE(_state.isReady());
}

TEST(util, time_in_msec)
{
    uint64_t time = Helper::get_current_msec_stamp();
    std::this_thread::sleep_for(seconds(1));
    EXPECT_GE(Helper::get_current_msec_stamp(), time + 1000);
}
// restrict typeva_start
TEST(util, test_node_to_string)
{
    Json::Value _value;

    _value["a"] = 1;
    _value["b"] = 1;
    _value["d"] = 1;

    std::string str = Helper::node_tree_to_string(_value);
    EXPECT_STREQ(str.c_str(), "{\"a\":1,\"b\":1,\"d\":1}");
}

int sum(int n_args, ...)
{
    va_list ap;
    va_start(ap, n_args);
    int var = n_args;

    int total = n_args;
    do
    {
        var = va_arg(ap, int);
        total += var;
    } while (var != 0);

    va_end(ap);

    return total;
}
//./bin/unittest --gtest_filter=util.variadic_func
TEST(util, variadic_func_int)
{
    EXPECT_EQ(sum(1, 2, 3, 4, 5, 6, 0), 21);
    EXPECT_EQ(sum(1, 2, 3, 5, 0), 11);
}

int opt(const char *start, ...)
{
    va_list ap;
    va_start(ap, start);
    const char *var = start;

    int total = 0;
    while (var != nullptr)
    {
        printf("%s\n", var);
        total++;
        var = va_arg(ap, const char *);
    }

    va_end(ap);

    return total;
}
//./bin/unittest --gtest_filter=util.variadic_func_str
TEST(util, variadic_func_str)
{
    EXPECT_EQ(opt("a", "b", "c", "d", nullptr), 4);
    EXPECT_EQ(opt("a", "b", "c", nullptr), 3);
}

TEST(util, mergeTraceNodeTree)
{
    TraceNode &n1 = PoolManager::getInstance().Take();
    TraceNode &n2 = PoolManager::getInstance().Take();
    TraceNode &n3 = PoolManager::getInstance().Take();
    TraceNode &n4 = PoolManager::getInstance().Take();

    n2.mParentIndex = n1.mPoolIndex;
    n1.mChildHeadIndex = n2.mPoolIndex;
    n3.mParentIndex = n2.mPoolIndex;
    n4.mParentIndex = n2.mPoolIndex;
    n3.mNextId = n4.mPoolIndex;
    n2.mChildHeadIndex = n3.mPoolIndex;

    Json::Value var = Helper::mergeTraceNodeTree(n1);
    std::cout << var.toStyledString();
}

std::string span;

void captureSpan(const char *s)
{
    span = s;
}

TEST(util, mergeTraceNodeTree_1)
{

    register_span_handler(captureSpan);

    NodeID id1,
        id2, id3, id4;
    Json::Value var;
    id1 = pinpoint_start_trace(E_ROOT_NODE);
    pinpoint_add_clue(id1, "name", "id1", E_LOC_CURRENT);
    id2 = pinpoint_start_trace(id1);
    pinpoint_add_clue(id2, "name", "id2", E_LOC_CURRENT);
    id3 = pinpoint_start_trace(id2);
    pinpoint_add_clue(id3, "name", "id3", E_LOC_CURRENT);
    id4 = pinpoint_start_trace(id2);
    pinpoint_add_clue(id4, "name", "id4", E_LOC_CURRENT);
    pinpoint_end_trace(id3);
    pinpoint_end_trace(id2);
    pinpoint_end_trace(id1);
    pinpoint_end_trace(id4);
    EXPECT_TRUE(span.find("id4") != span.npos);
    EXPECT_TRUE(span.find("id1") != span.npos);
    EXPECT_TRUE(span.find("id2") != span.npos);
    EXPECT_TRUE(span.find("id3") != span.npos);
    EXPECT_TRUE(span.find("calls") != span.npos);
}
