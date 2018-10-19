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
#include <cstdio>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include "executor.h"
#include "pinpoint_error.h"
#include "pinpoint_api.h"
#include "pinpoint_agent_context.h"

using namespace Pinpoint::log;

namespace Pinpoint
{
    namespace Agent
    {

        //<editor-fold desc="Executor">

        Executor::Executor(const std::string &executorName) : m_executorName(executorName)
        {

        }


        std::string Executor::getName()
        {
            return this->m_executorName;
        }
        //</editor-fold>

        //<editor-fold desc="ScheduledExecutor">

        ScheduledExecutor::ScheduledExecutor(boost::asio::io_service& agentIo,const std::string &executorName)
                : io(agentIo),
				  _status(E_RUNNING)
        {

        }

        ScheduledExecutor::~ScheduledExecutor()
        {

        }


        void ScheduledExecutor::stopScheduleExecutor()
        {
        	std::vector<boost::shared_ptr<boost::asio::deadline_timer> >::const_iterator cit = timerSet.begin();
        	while(cit !=timerSet.end())
        	{
        		LOGD("cancel a ScheduledExecutor");
        		(*cit)->cancel();
        		cit++;
        	}
        	_status = E_STOPPED;
        }

        int32_t ScheduledExecutor::addTask(const ExecutorTaskPtr& pTask, uint32_t interval, int32_t callTimes)
        {
        	if(_status == E_STOPPED)
        	{
        		LOGI("ScheduledExecutor stopped");
        		return SUCCESS;
        	}

            LOGD("ScheduledExecutor add task: name=%s, interval=%d, callTimes=%d", pTask->getTaskName().c_str(), interval, callTimes);

            try
            {
                RepeatedTask *repeatedTask = new RepeatedTask(pTask, interval, callTimes);

                boost::shared_ptr<RepeatedTask> repeatedTaskPtr = boost::shared_ptr<RepeatedTask>(repeatedTask);

                io.post(boost::bind(&ScheduledExecutor::addIoTask_, this, repeatedTaskPtr));
            }
            catch (std::exception& exception)
            {
                LOGE("ScheduledExecutor::addTask throw exception=%s", exception.what());
                return FAILED;
            }

            return SUCCESS;
        }

        int32_t ScheduledExecutor::addIoTask_(const boost::shared_ptr<RepeatedTask> &repeatedTaskPtr)
        {
            LOGD("run addTask_ in io.run() thread: add %s", repeatedTaskPtr->getTaskName().c_str());

            try
            {
                boost::asio::deadline_timer *pTimer = new boost::asio::deadline_timer(io);

                boost::shared_ptr<boost::asio::deadline_timer> timerPtr =
                        boost::shared_ptr<boost::asio::deadline_timer>(pTimer);
                // store this timer, cancel it when ScheduledExecutor exit;
                timerSet.push_back(timerPtr);

                timerPtr->expires_from_now(boost::posix_time::milliseconds(repeatedTaskPtr->getInterval()));
                timerPtr->async_wait(boost::bind(&RepeatedTask::run,_1, repeatedTaskPtr, timerPtr));

            }
            catch (std::exception& exception)
            {
                LOGE("ScheduledExecutor::addTask_ throw exception=%s", exception.what());
                return FAILED;
            }

            return SUCCESS;

        }


        ScheduledExecutor::RepeatedTask::RepeatedTask(const ExecutorTaskPtr &pTask, uint32_t interval,
                                                      int32_t callTimes)
                : m_pTask(pTask)
        {
            assert (pTask != NULL);
            assert (callTimes > 0 || callTimes == -1);
            assert (interval > 0);
            this->callTimes = callTimes;
            this->interval = interval;
        }

        int32_t ScheduledExecutor::RepeatedTask::run(const boost::system::error_code& e,
        											 boost::shared_ptr<RepeatedTask>& repeatedTaskPtr,
                                                     boost::shared_ptr<boost::asio::deadline_timer>& timerPtr)
        {

            int32_t err = SUCCESS;

			if(e ==  boost::asio::error::operation_aborted)
			{
				// timer had been canceled
				LOGD("timer canceled");
				return FAILED;
			}

            try
            {

                err = repeatedTaskPtr->m_pTask->run();

                if (err != SUCCESS)
                {
                    LOGE("ScheduledExecutor::RepeatedTask::run: task %s failed: err=%d.",
                         repeatedTaskPtr->m_pTask->getTaskName().c_str(), err);
                }
            }
            catch (std::exception& exception)
            {
                LOGE("ScheduledExecutor::RepeatedTask::run: task %s throw exception=%s: err=%d.",
                     repeatedTaskPtr->m_pTask->getTaskName().c_str(), exception.what());
                assert(false);
                err = FAILED;
            }

            if (repeatedTaskPtr->callTimes != -1)
            {
                repeatedTaskPtr->callTimes--;
            }

            if(timerPtr->expires_from_now() == boost::posix_time::milliseconds(0))
            {
            	return err;
            }

            if (repeatedTaskPtr->callTimes == -1 || repeatedTaskPtr->callTimes > 0)
            {

                timerPtr->expires_from_now(boost::posix_time::milliseconds(repeatedTaskPtr->interval));
                timerPtr->async_wait(boost::bind(&RepeatedTask::run,_1,repeatedTaskPtr, timerPtr));
            }

            return err;
        }
        //</editor-fold>

        //<editor-fold desc="ThreadExecutor">

        ThreadExecutor::ThreadExecutor(const std::string &executorName)
                : Executor(executorName)
        {

        }

        ThreadExecutor::~ThreadExecutor()
        {

        }

        void ThreadExecutor::start()
        {
            runThread();
        }

        void ThreadExecutor::stop()
        {
            stopTask();

            if (m_pThread != NULL)
            {
                m_pThread->interrupt();
                m_pThread->join();
            }
            LOGD("ThreadExecutor::stop(): executorName=%s", this->getName().c_str());
        }

        void ThreadExecutor::join()
        {
            m_pThread->join();
        }

        uint64_t ThreadExecutor::gettid()
        {
            uint64_t l;

            if (m_pThread == NULL)
            {
                return (uint64_t)-1;
            }

            std::stringstream ss;
            ss << m_pThread->get_id();
            ss >> l;

            return l;
        }

        void ThreadExecutor::runThread()
        {
            m_pThread.reset(new boost::thread(boost::bind(&ThreadExecutor::executeTask, this)));
        }
        //</editor-fold>

        //<editor-fold desc="SyncQueueExecutor">
#ifdef UNUSED_CODE

        SyncQueueExecutor::SyncQueueExecutor(std::string executorName, uint32_t queueLen)
                : ThreadExecutor(executorName), queue(queueLen), m_queueLen(queueLen), stop(false)
        {

        }

        void SyncQueueExecutor::executeTask()
        {
            while (!stop)
            {
                try
                {
                    boost::shared_ptr<ExecutorTask> taskPtr = queue.get();

                    assert (taskPtr != NULL);

                    taskPtr->bgRun();
                }
                catch (boost::thread_interrupted const &)
                {
                    LOGI("%s is interrupted!", this->getName().c_str());
                    return;
                }
                catch (...)
                {
                    LOGI("%s task get exception!", this->getName().c_str());
                }
            }
        }

        void SyncQueueExecutor::stopTask()
        {
            this->stop = true;
            LOGI("SyncQueueExecutor::stopTask(), name=%s", this->getName().c_str());
        }

        int32_t SyncQueueExecutor::putTask(boost::shared_ptr<ExecutorTask> pTask)
        {
            return queue.put(pTask);
        }

        int32_t SyncQueueExecutor::putTask(boost::shared_ptr<ExecutorTask> pTask, uint32_t timeout)
        {
            return queue.put(pTask, timeout);
        }
#endif
        //</editor-fold>

        //<editor-fold desc="MainProcessChecker">

        MainProcessChecker* MainProcessChecker::createChecker()
        {
            // todo
            static MainProcessChecker instance;
            return &instance;
        }

        MainProcessChecker::MainProcessChecker()
        {
            using namespace Pinpoint::memory;
            using Pinpoint::Agent::PinpointAgentContext;

            std::string shmname = "";
#ifdef UNITTEST
            shmname = shmname + "unittest" + "-MainProcessChecker-" + boost::lexical_cast < std::string> (getpid());
#else
            shmname = shmname + "MainProcessChecker_"+ boost::lexical_cast < std::string> (PinpointAgentContext::getContextPtr()->startTimestamp);
#endif
            pSShmFlag = boost::shared_ptr < SafeShareMemory<ShmMainProcFlag> > (
                    new SafeShareMemory<ShmMainProcFlag>(shmname.c_str()));

        }

        bool MainProcessChecker::isMainProcess(OS_process_id_t & mainPid)
        {
            static OS_process_id_t curPid = get_current_process_id();
            ShmMainProcFlag* pShmFlag = pSShmFlag->getData();
            mainPid = pShmFlag->mainPid;

#define CHECK_TIMER(SFlg) (SFlg->curUTCSec + SFlg->expireTime  < (int32_t)get_current_unixsec_stamp())


            if (CHECK_TIMER(pShmFlag)) {
                if (pSShmFlag->getWriteLock().try_lock(0) == 0) // get lock
                {
                    if (CHECK_TIMER(pShmFlag)) {
                        LOGD(" pre main is %d timeout with %d from %d ", pShmFlag->mainPid,pShmFlag->curUTCSec,pShmFlag->expireTime);
                        pShmFlag->mainPid = get_current_process_id();
                        keepSelfMain();
                        mainPid = pShmFlag->mainPid;
                        pSShmFlag->getWriteLock().unlock();
                        return true;
                    }
                    pSShmFlag->getWriteLock().unlock();
                    return false;
                }
                return false;
            }
            if (curPid == pShmFlag->mainPid) {
                keepSelfMain();
                return true;
            }
            return false;
        }
        //</editor-fold>

        void TaskDispatcher::start()
        {
        	boost::mutex::scoped_lock lock(thrMutex);
        	// 1. start the background thread
        	pth = new boost::thread(&TaskDispatcher::bgRun,this);
        	// 2. wait thread running success
        	while(thrStatus != E_Running ){
        		thrCon.wait(lock);
        	}

        }

        void TaskDispatcher::stop()
	    {
        	// 1. stop io

//        	io.stop();

        	// 2. wait thread stop
        	pth->join();
        	thrStatus = E_Stop;
	    }

    	void TaskDispatcher::bgRun()
		{
    		boost::mutex::scoped_lock lock(thrMutex);
    		thrStatus = E_Running;
    		thrCon.notify_one(); //thread is working
    	    lock.unlock();
			try
			{
				io.run();
			}
			catch (std::exception& e)
			{
				LOGE("io_service throw %s", e.what());
			}

			LOGW("background task finished");

		}

    	void TaskDispatcher::postEvent(const boost::function<void(void)> fun)
    	{
    		io.post(fun);
    	}

    }
}

