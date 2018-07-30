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
/**
 * add main process test 03/16
 *
 * */


#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "log.h"
#include "memory_pool.h"
#include "utility.h"
#include "pp_test_utility.h"


#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread/condition.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <list>
#include <stack>
#include <stddef.h>
#include <iostream>
using std::list;
using std::stack;


typedef struct _M_PSTest
{
    Pinpoint::PSMutexLock gMutex;
    char buf[1024];
} M_PSTest;

typedef struct
{
  volatile   int a;
  volatile   int b;
} TT;


#define TTLOOPTIME 100000



TEST(utility, double_VS_long)
{
    using namespace Pinpoint::utils;
    srand(12);
    for (int i = 0; i < 10000; i++) {
        int64_t val = random();
        EXPECT_EQ(val, double_to_long_bits(long_bits_to_double(val)));
    }
}

//@deprecated
void process_main(TFlag iFlag)
{
    using namespace boost::interprocess;
    M_PSTest* pMtps;

//    if (iFlag.Flag.RmoveMem) {
//        LOGI("remove mem %d ", shared_memory_object::remove("MySharedMemory"));
//    }

    shared_memory_object shm
            (open_or_create //only create
            , "MySharedMemory" //name
            , read_write //read-write mode
            );

    offset_t size;
    shm.get_size(size);
    if (size == 0) // must a new  shm
    {
        shm.truncate(sizeof (M_PSTest));
    }

    LOGI("MySharedMemory size %d", size);

    mapped_region region
            (shm //What to map
            , read_write //Map it as read-write
            );
    void * addr = region.get_address();
    if (size == 0) { // new
        pMtps = new (addr) M_PSTest;
    } else { // old
        pMtps = static_cast<M_PSTest*> (addr);
    }
    int i = 100;
    while (i--) {
        int err = pMtps->gMutex.try_lock(1000);
        if (err == 0) {
            LOGI("get lock");
            //        sleep(1);
            if (iFlag.Flag.Seg) {
                *(int*) 0 = 10;
            }
            pMtps->gMutex.unlock();
            LOGI("free lock");
        } else {
            LOGI("get lock failed !!!");
        }
    }

    if (iFlag.Flag.RmoveMem)
        shared_memory_object::remove("MySharedMemory");
}


void process_test_share_memory(TFlag iFlag)
{
	Pinpoint::memory::SafeShareMemory<TT> sSm("test1");
    TT * tt = sSm.getData();
    int i = TTLOOPTIME;
    while (i--) {
        sSm.getWriteLock().lock();
        tt->a++;
        if (iFlag.Flag.Seg) {
            MAKE_SEG_ERROR();
        }
        tt->b++;
        sSm.getWriteLock().unlock();

        // test try_lock()
        if (sSm.getWriteLock().try_lock(10) == 0) {
            sSm.getWriteLock().unlock();
        }

    }
}


TEST(test, SafeShareMemory)
{
    using namespace boost::interprocess;

    TFlag tf = {0};
    tf.Flag.Seg = 0;
    // make sure the test1 is exist
    Pinpoint::memory::SafeShareMemory<TT> sSm("test1");
    sleep(1);
    fork_and_execute(process_test_share_memory, tf);

    tf.Flag.Seg = 1;
    fork_and_execute(process_test_share_memory, tf);
    tf.Flag.Seg = 0;
    fork_and_execute(process_test_share_memory, tf);

    WAIT_CHILDREN();


    TT* tt = sSm.getData();

    EXPECT_EQ(tt->a, TTLOOPTIME * 2 + 1);

    EXPECT_EQ(tt->b, TTLOOPTIME * 2);

    LOGI("remove mem %d ", shared_memory_object::remove("test1"));
}



