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
#include <Util/Trace.h>
#include <thread>

#include "Cache/SafeSharedState.h"
using Trace::STT;
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
    _state.markOFFline();
    EXPECT_TRUE(_state.isOFFLine());
    _state.markONLine();
    EXPECT_FALSE(_state.isOFFLine());
    _state.updateStartTime(1316615272 );
    EXPECT_EQ(1316615272, _state.getStartTime());
}
