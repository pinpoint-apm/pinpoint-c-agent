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
#ifndef PINPOINT_EXECUTOR_H
#define PINPOINT_EXECUTOR_H

#include "stdint.h"
#include "pinpoint_error.h"
#include "utility.h"
#include "memory_pool.h"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/interprocess/detail/os_thread_functions.hpp>
#include <set>
#include <string>
#include <queue>
#include <functional>


namespace Pinpoint
{
    namespace Agent
    {

        class Future
        {
        public:
            virtual void onSuccess()
            { };

            virtual void onError()
            { };

            virtual void onException()
            { };
        };

        class ExecutorTask : public Future
        {
        public:
            virtual int32_t run() = 0;

            explicit ExecutorTask(const std::string &taskName) : m_taskName(taskName)
            { }

            virtual ~ExecutorTask()
            { }

            std::string getTaskName()
            { return m_taskName; }

        private:
            std::string m_taskName;
        };

        typedef boost::shared_ptr<ExecutorTask> ExecutorTaskPtr;

        class Executor
        {
        public:
            explicit Executor(const std::string &executorName);

            virtual ~Executor()
            { }

            virtual void start() = 0;

            virtual void stop() = 0;

            virtual std::string getName();

        protected:
            std::string m_executorName;

        private:
            explicit Executor(const Executor&);
            Executor& operator=(const Executor&);
        };

        class ThreadExecutor : public Executor
        {
        public:
            explicit ThreadExecutor(const std::string &executorName);

            void join();

            uint64_t gettid();

            virtual void start();

            virtual void stop();

            virtual ~ThreadExecutor();

        protected:
            virtual void executeTask() = 0;

            virtual void stopTask() = 0;

        private:
            void runThread();

            boost::shared_ptr<boost::thread> m_pThread;
        };


        class TaskDispatcher
        {

        private:
        	TaskDispatcher() {
        		pth = NULL;
        	}

        	virtual ~TaskDispatcher()
        	{
        		stop();

        		if(pth){
        			delete pth;
        		}
        	}

        public:

        	/// Note: use try { } catch { }
        	void  start();

        	void  stop();

        	boost::asio::io_service& getAsio()
        	{
        		return io;
        	}

        public:

        	static TaskDispatcher& getInstance()
        	{
        		static TaskDispatcher _task;
        		return _task;
        	}

        private:

        	void run();

        private:
        	boost::thread* pth;
        	boost::condition thrCon;
        	boost::mutex     thrMutex;
        	boost::asio::io_service io;
        };


        template<typename T>
        class ThreadSafeQueue
        {
        public:
            virtual int32_t put(T &item) = 0;

            virtual int32_t put(T &item, uint32_t timeout) = 0; // timeout msec
            virtual T get() = 0;

            virtual int32_t get(T &item, uint32_t timeout) = 0;

            virtual int32_t getAll(std::vector<T> &vt, uint32_t timeout) = 0;

            virtual ~ThreadSafeQueue()
            { };
        };

        template<typename T>
        class ThreadSafeAndTryQueue : public ThreadSafeQueue<T>
        {
        public:
            virtual int32_t put_try(T &item) = 0;

            virtual int32_t get_try(T &item, uint32_t timeout) = 0;

        };


        template<typename T>
        class SynchronizedFixedLengthQueue : public ThreadSafeQueue<T>
        {
        public:
            explicit  SynchronizedFixedLengthQueue(uint32_t queueLen) : m_queue(queueLen)
            {
                this->m_queueLen = queueLen;
            }

            virtual int32_t put(T &item)
            {
                boost::timed_mutex::scoped_lock sl(this->monitor);

                while (this->m_queue.full())
                {
                    this->buffer_not_full.wait(this->monitor);
                }

                this->m_queue.push_back(item);

                this->buffer_not_empty.notify_one();

                return SUCCESS;
            }

            virtual int32_t put(T &item, uint32_t timeout)
            {
                assert (timeout > 0);

                boost::posix_time::ptime start = boost::get_system_time();
                boost::posix_time::ptime end = start + boost::posix_time::milliseconds(timeout);

                boost::timed_mutex::scoped_lock sl(this->monitor, boost::posix_time::milliseconds(timeout));

                if (sl.owns_lock())
                {
                    while (this->m_queue.full())
                    {
                        if (!this->buffer_not_full.timed_wait(this->monitor, end))
                        {
                            return WAIT_TIMEOUT;
                        }
                    }

                    this->m_queue.push_back(item);

                    this->buffer_not_empty.notify_one();
                }
                else
                {
                    return WAIT_TIMEOUT;
                }


                return SUCCESS;
            }

            virtual T get()
            {
                T item;

                boost::timed_mutex::scoped_lock sl(this->monitor);

                while (this->m_queue.empty())
                {
                    this->buffer_not_empty.wait(this->monitor);
                }

                item = this->m_queue.front();
                this->m_queue.pop_front();

                this->buffer_not_full.notify_one();

                return item;
            }

            virtual int32_t get(T &item, uint32_t timeout)
            {
                boost::posix_time::ptime start = boost::get_system_time();
                boost::posix_time::ptime end = start + boost::posix_time::milliseconds(timeout);

                boost::timed_mutex::scoped_lock sl(this->monitor, boost::posix_time::milliseconds(timeout));

                if (sl.owns_lock())
                {
                    while (this->m_queue.empty())
                    {
                        if (!this->buffer_not_empty.timed_wait(this->monitor, end))
                        {
                            return WAIT_TIMEOUT;
                        }
                    }

                    item = this->m_queue.front();
                    this->m_queue.pop_front();

                    this->buffer_not_full.notify_one();

                    return SUCCESS;
                }

                return WAIT_TIMEOUT;
            }

            virtual int32_t getAll(std::vector<T> &vt, uint32_t timeout)
            {
                vt.clear();
                if (this->m_queue.empty())
                {
                    return SUCCESS;
                }

                boost::timed_mutex::scoped_lock sl(this->monitor, boost::posix_time::milliseconds(timeout));

                if (sl.owns_lock())
                {
                    if (this->m_queue.empty())
                    {
                        return SUCCESS;
                    }

                    vt.reserve(this->m_queue.size());

//                    while (!this->m_queue.empty())
//                    {
//                        T item = this->m_queue.front();
//                        this->m_queue.pop_front();
//                        vt.push_back(item);
//                    }
                    std::copy(this->m_queue.begin(), this->m_queue.end(), std::back_insert_iterator<std::vector<T> >(vt));
                    this->m_queue.clear();

                    this->buffer_not_full.notify_one();

                    return SUCCESS;
                }

                return WAIT_TIMEOUT;
            }

#ifndef RUN_TEST
        private:
#endif
            boost::circular_buffer<T> m_queue;
            uint32_t m_queueLen;
            boost::timed_mutex monitor;
            boost::condition buffer_not_full, buffer_not_empty;
        };

        class ScheduledExecutor
        {
        public:
            explicit  ScheduledExecutor(boost::asio::io_service& agentIo, const std::string &executorName);

            virtual   ~ScheduledExecutor();

            int32_t addTask(const ExecutorTaskPtr& pTask, uint32_t interval, int32_t callTimes);

        private:

            class RepeatedTask
            {
            public:
                static int32_t run(boost::shared_ptr<RepeatedTask>& repeatedTask,
                                   boost::shared_ptr<boost::asio::deadline_timer>& timerPtr);

                RepeatedTask(const boost::shared_ptr<ExecutorTask> &pTask, uint32_t interval, int32_t callTimes);

                uint32_t getInterval()
                { return interval; }

                std::string getTaskName()
                {
                    if (m_pTask != NULL)
                    {
                        return m_pTask->getTaskName();
                    }

                    assert (false);
                    return "";
                }

            private:
                boost::shared_ptr<ExecutorTask> m_pTask;
                uint32_t interval;
                int32_t callTimes;
            };

            boost::asio::io_service& io;
//            boost::asio::io_service::work work;

            virtual void executeTask();

            virtual void stopTask();

            int32_t addTask_(const boost::shared_ptr<RepeatedTask>& repeatedTaskPtr);

        };

        typedef boost::shared_ptr<ScheduledExecutor> ScheduledExecutorPtr;

#ifdef UNUSED_CODE
        class SyncQueueExecutor : public ThreadExecutor
        {
        public:
            SyncQueueExecutor(std::string executorName, uint32_t queueLen);

            int32_t putTask(boost::shared_ptr<ExecutorTask> pTask);

            int32_t putTask(boost::shared_ptr<ExecutorTask> pTask, uint32_t timeout);

            ~SyncQueueExecutor()
            { };
        protected:
            virtual void executeTask();

            virtual void stopTask();

        private:
            SynchronizedFixedLengthQueue<boost::shared_ptr<ExecutorTask> > queue;
            uint32_t m_queueLen;
            bool stop;
        };
#endif

        using boost::interprocess::ipcdetail::OS_process_id_t;
        using Pinpoint::utils::get_current_unixsec_stamp;
        using Pinpoint::utils::get_current_process_id;

        class MainProcessChecker //: public Pinpoint::Agent::ExecutorTask
        {
        private:
            typedef struct _SelfFlag
            {
                int64_t curUTCSec; // unix timestamp
                OS_process_id_t mainPid; //
                uint32_t expireTime; //
                _SelfFlag()
                {
                    using namespace Pinpoint::utils;
                    curUTCSec = get_current_unixsec_stamp();
                    mainPid = get_current_process_id();
                    expireTime = 0;
                }
            } ShmMainProcFlag;

#if 0
            int32_t run()
            {
                OS_process_id_t mainPid;
                isMainProcess(mainPid);
                return SUCCESS;
            }
#endif

        public:
            static MainProcessChecker* createChecker();

            void setMaxFreeTime(uint32_t iSec)
            {
                pSShmFlag->getData()->expireTime = iSec;
            }
            virtual bool isMainProcess(OS_process_id_t & mainPid);
        private:
            inline void keepSelfMain()
            {
                 ShmMainProcFlag* pShmFlag = pSShmFlag->getData();
                 pShmFlag->curUTCSec = get_current_unixsec_stamp();
            }
        private:
            MainProcessChecker();
            virtual ~MainProcessChecker() {}
            boost::shared_ptr<Pinpoint::memory::SafeShareMemory<ShmMainProcFlag> > pSShmFlag;
        };

    }
}


#endif
