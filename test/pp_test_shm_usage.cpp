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
 * pp_test_shm_usage.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: bluse
 */
#include "gtest/gtest.h"
#include "gmock/gmock.h"


#include "pp_test_utility.h"
#include "trace.h"
#include "executor.h"

using Pinpoint::Trace::ProcessShareDataManager;

#define PROCESS_COUNT 10
#define UNIT_COUNT 10000


void process_main_getT(TFlag )
{
    ProcessShareDataManager*ptemp = ProcessShareDataManager::instance();
    sleep(2);
    for (int i = 0; i < UNIT_COUNT; i++)
        ptemp->generateTranSequence();

}

TEST(shm,tran_sequence)
{
    TFlag _tflag;
    for(int i =0;i<PROCESS_COUNT ;i++)
        fork_and_execute(process_main_getT,_tflag);
    WAIT_CHILDREN();

}


void process_test_main_checker(TFlag iFlag)
{
    // @note
    // 1 test 5 Sec
    Pinpoint::Agent::MainProcessChecker* gPc = Pinpoint::Agent::MainProcessChecker::createChecker();
    pid_t mainPid;
    int i = 100;
    gPc->setMaxFreeTime(5);
    // @ test normal
    while (i--) {
        if (gPc->isMainProcess(mainPid)) {
            LOGI(" I'm main");
            sleep(1);
        } else {
            LOGI(" I'm not main but %d is", mainPid);
            sleep(1);
            EXPECT_NE(mainPid, getpid());
        }
    }

    i = 200;
    while (i--) {
        if (gPc->isMainProcess(mainPid)) {
            LOGI(" I'm main");
            MAKE_SEG_ERROR();
        } else {
            sleep(1);
            LOGI("I'm not main but %d is", mainPid);
            EXPECT_NE(mainPid, getpid());
        }
    }
}



TEST(MainProcessChecker, run_and_exit)
{
    (void)Pinpoint::Agent::MainProcessChecker::createChecker();
    TFlag tf = {0};
    for (int i = 0; i < 20; i++) {
        fork_and_execute(process_test_main_checker, tf);
    }
    sleep(1);
    WAIT_CHILDREN();
}
