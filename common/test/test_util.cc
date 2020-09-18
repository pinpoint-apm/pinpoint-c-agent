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

#include "../src/Util/Helper.h"
#include "Cache/SafeSharedState.h"
using Helper::STT;
using Cache::SafeSharedState;
using std::chrono::seconds;

TEST(util, sst)
{
    ADDTRACE();

    std::this_thread::sleep_for(seconds(1));

}


TEST(util,onOFFline)
{
    SafeSharedState& _state =  SafeSharedState::instance();
    EXPECT_FALSE(_state.isReady());
    _state.updateStartTime(1316615272 );
    EXPECT_EQ(1316615272, _state.getStartTime());
    EXPECT_TRUE(_state.isReady());
}

TEST(util,time_in_msec)
{
    uint64_t time = Helper::get_current_msec_stamp();
    std::this_thread::sleep_for(seconds(1));
    EXPECT_GE(Helper::get_current_msec_stamp(),time+1000);

}

TEST(util,test_node_to_string)
{
    Json::Value _value;

    _value["a"]=1;
    _value["b"]=1;
    _value["d"]=1;

    std::string str =  Helper::node_tree_to_string(_value);
    EXPECT_STREQ(str.c_str(),"{\"a\":1,\"b\":1,\"d\":1}");
}