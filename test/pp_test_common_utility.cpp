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
/*
 * pp_test_common_utility.cpp
 *
 *  Created on: Mar 28, 2017
 *      Author: bluse
 */
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "log.h"

#include "utility.h"
#include "pp_test_utility.h"

#if 0

using Pinpoint::utils::ProcessLifeCycleManager;
using Pinpoint::utils::I_ForkWatcher;



class testSub :public I_ForkWatcher
{
public:
    testSub(){
        itest = 0;
    }
    void additest(){itest++;}

public:
    int32_t itest;
private:
    bool reInitAndKeepWatching(void)
    {
        LOGI("call after fork");
        itest = 0;
        return true;
    }
};


testSub tsub;

void sub_main_layer_2(TFlag)
{
    EXPECT_EQ(tsub.itest,0);
}
void sub_main_layer_1(TFlag)
{
    EXPECT_EQ(tsub.itest,0);
    TFlag t;
    tsub.additest();
    fork_and_execute(sub_main_layer_2,t);
    WAIT_CHILDREN();
}

TEST(test,ProcessLifeCycleManager){
    using Pinpoint::utils::ICAFITManger;
    //reg call
    boost::shared_ptr<ICAFITManger> picaf = ProcessLifeCycleManager::instance().regWatchCb(&tsub);
    // fork
    tsub.additest();
    TFlag t;
    fork_and_execute(sub_main_layer_1,t);
    WAIT_CHILDREN();
}


#endif
