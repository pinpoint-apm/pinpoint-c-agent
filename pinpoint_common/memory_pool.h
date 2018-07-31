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
#ifndef PINPOINT_MEMORY_POOL_H
#define PINPOINT_MEMORY_POOL_H

#include "pinpoint_lock.h"
#include "pinpoint_api.h"
#include <stdint.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/detail/os_thread_functions.hpp>
#include <boost/shared_ptr.hpp>
namespace Pinpoint {
    namespace memory {
        using boost::interprocess::shared_memory_object;
        using boost::interprocess::mapped_region;
        using boost::interprocess::open_or_create;
        using boost::interprocess::offset_t;
        using boost::interprocess::read_write;
        using boost::interprocess::ipcdetail::OS_process_id_t;

template <typename T>
class MemPool
{
public:

    explicit MemPool(uint32_t number) : number(number) { };
    virtual T* get() = 0;
    virtual T* try_get(int64_t i64Msec) = 0;
    virtual void free(T* ptr) = 0;
    virtual ~MemPool() { };
private:
    uint32_t number;
};

template <typename T>
class TestMemPool : public MemPool<T>
{
public:

    explicit TestMemPool(uint32_t number) : MemPool<T>(number) {number = 0; };

    T* get()
    {
        return (T*)::malloc(sizeof (T));
    }

    void free(T* ptr)
    {
        return ::free(ptr);
    }

    T* try_get(int64_t i64Msec) { }

    virtual ~TestMemPool() { };
private:
    uint32_t number;
};

#define DEBUG_SSM 

#ifdef DEBUG_SSM
#define SLOG_INFO LOGD
#else
#define SLOG_INFO
#endif
        enum ESHAREMEM
        {
            REMOVE_WHEN_EXIT = 0x1,
        };
        template <typename T>
        class SafeShareMemory
        {
        private:
            typedef struct
            {
                OS_process_id_t createrPid;
                int32_t refcount;
                T t;
                PSMutexLock writeLock;
            } PackType;
        private:
            // note mType place onto share memory ,DO NOT  delete 
            PackType* mType;
            std::string mSMName;
        public:
            T* getData()
            {
                return &mType->t;
            }
            PSMutexLock& getWriteLock()
            {
                return mType->writeLock;
            }
            explicit SafeShareMemory(const char* uniqName)
            {
                shm = boost::shared_ptr<shared_memory_object>(new shared_memory_object(open_or_create, uniqName, read_write));
                boost::interprocess::offset_t shmSize = 0;
                shm->get_size(shmSize);
                if (shmSize == 0) // must a new  shm 
                {
                    shm->truncate(sizeof (PackType));
                }
                SLOG_INFO("SafeShareMemory %s size %d", uniqName, shmSize);
                region = boost::shared_ptr<mapped_region>(new mapped_region(*shm, read_write));
                void * addr = region->get_address();
                if (shmSize == 0) {
                    mType = new (addr) PackType;
                    mType->createrPid = Pinpoint::utils::get_current_process_id(); //boost::interprocess::ipcdetail::get_current_process_id();
                    mType->refcount   = 1;
                    SLOG_INFO("%s expand %ld  createrPid %d ", uniqName, sizeof (PackType), mType->createrPid);
                } else {
                    mType = static_cast<PackType*> (addr);

                    mType->writeLock.lock();
                    mType->refcount++;
                    mType->writeLock.unlock();

                }
                mSMName = uniqName;
                LOGD("SafeShareMemory:%s pid:%d ", uniqName, mType->createrPid);
            }
            virtual ~SafeShareMemory()
            {
                mType->writeLock.lock();
                mType->refcount--;
                mType->writeLock.unlock();

                if (mType->refcount <= 0) {
                    int ret = shared_memory_object::remove(mSMName.c_str());
                    LOGD("share memory mSMName %s removed %d ", mSMName.c_str(),ret);
                }

            }
        private:
            boost::shared_ptr<shared_memory_object> shm;
            boost::shared_ptr<mapped_region> region;
        private:
            SafeShareMemory(const SafeShareMemory<T> &casMpool);
            SafeShareMemory<T>& operator=(const SafeShareMemory<T>&);
            SafeShareMemory();
        };
    }
}
#endif
