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
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "test_env.h"
#include "executor.h"
#include <string>
#include "pinpoint_api.h"

using namespace Pinpoint::log;
using namespace Pinpoint::Agent;
using namespace Pinpoint;

class TestScheduleTask1 : public ExecutorTask
{
public:
    TestScheduleTask1(const std::string& taskName, uint32_t sequence, volatile uint32_t* pGlobalSeq)
            : ExecutorTask(taskName)
    {
        this->sequence = sequence;
        this->m_pGlobalSeq = pGlobalSeq;
    }
    int32_t run()
    {
        ASSERT_EX(*this->m_pGlobalSeq == sequence, *this->m_pGlobalSeq, this->sequence);
        (*this->m_pGlobalSeq)++;
        return SUCCESS;
    }
private:
    uint32_t sequence;
    volatile uint32_t* m_pGlobalSeq;
};

// call several times
class TestScheduleTask2 : public ExecutorTask
{
public:
    TestScheduleTask2(const std::string& taskName, uint32_t callTimes)
            : ExecutorTask(taskName)
    {
        m_callTimes = callTimes;
        m_count = 0;
    }
    ~TestScheduleTask2()
    {
        ASSERT_EX(m_callTimes == m_count, m_callTimes, m_count);
    }
    int32_t run()
    {
        m_count++;
        return SUCCESS;
    }
private:
    uint32_t m_callTimes;
    uint32_t m_count;
};

class TestScheduleTask3 : public ExecutorTask
{
public:
    TestScheduleTask3(const std::string& taskName)
            : ExecutorTask(taskName)
    {
        this->sequence = 0;
    }
    int32_t run()
    {
        LOGI("sequence=%d", this->sequence++);
        return SUCCESS;
    }
private:
    uint32_t sequence;
};

class ExitTask : public ExecutorTask
{
public:
    ExitTask(const std::string& taskName, volatile bool* pExit)
            : ExecutorTask(taskName)
    {
        m_pExit = pExit;
    }
    int32_t run()
    {
        *m_pExit = true;
        return SUCCESS;
    }

private:
    volatile bool* m_pExit;
};

class ExpiryTaskExecutor : public ThreadExecutor
{
public:
    ExpiryTaskExecutor(const std::string& taskName) :
            ThreadExecutor(taskName), io(), timer_(io, boost::posix_time::seconds(1)), count(0) {}
    virtual void executeTask()
    {
        LOGI("ExpiryTaskExecutor::executeTask start");

        try {
            io.run();
        } catch (boost::system::system_error &er) {
            LOGI("io.run() throw %s",er.what());
        }
        LOGI("ExpiryTaskExecutor::executeTask stop ...");
        // timer_.expires_at(boost::get_system_time() - boost::posix_time::seconds(3600));
    }
    virtual void stopTask()
    {
        while (this->count != 2)
        {
            LOGI("ExpiryTaskExecutor::stopTask: count != 1, sleep 1.");
            boost::this_thread::sleep(boost::posix_time::seconds(1));
        }
        io.stop();
    }

    void executeExpiryTask(int32_t sec)
    {
        if (sec < 0)
        {
            timer_.expires_at(boost::get_system_time() - boost::posix_time::seconds(abs(sec)));
        }
        else
        {
            timer_.expires_at(boost::get_system_time() + boost::posix_time::seconds(abs(sec)));
        }

        timer_.async_wait(boost::bind(&ExpiryTaskExecutor::addCount, this, sec));
    }

    void addCount(int32_t sec)
    {
        LOGI("ExpiryTaskExecutor::addCount! waitSec=%d", sec);
        count++;
    }
private:
    boost::asio::io_service io;
    boost::asio::deadline_timer timer_;
    volatile uint32_t count;
};

class AddScheduledTaskExecutor: public ThreadExecutor
{
public:
    AddScheduledTaskExecutor(const std::string& taskName, const ScheduledExecutorPtr& scheduledExecutorPtr) :
            ThreadExecutor(taskName), scheduledExecutorPtr(scheduledExecutorPtr), _stop(false) {}

protected:

    class PrintTask : public ExecutorTask
    {
    public:
        explicit PrintTask(const std::string &taskName, const std::string &printStr)
                : ExecutorTask(taskName), printStr(printStr), executeCount(0) {}
        virtual int32_t run()
        {
            LOGI("%s, executeCount=%d", printStr.c_str(), executeCount);
            executeCount++;
            return SUCCESS;
        }
    private:
        std::string printStr;
        uint32_t executeCount;
    };

    virtual void executeTask()
    {

        uint32_t taskCount = 0;

        while (!_stop)
        {
            std::string printStr;

            Pinpoint::utils::PStream pStream = Pinpoint::utils::FormatConverter::getPStream();

            *pStream << "tid=" << gettid() << ", taskCount=" << taskCount++;

            *pStream >> printStr;

            ExecutorTaskPtr executorTaskPtr(new PrintTask("printTask", printStr));

            scheduledExecutorPtr->addTask(executorTaskPtr, 100, -1);
        }
    }
    virtual void stopTask()
    {
        _stop = true;
        LOGI("tid=%ld stop", gettid());
    }

private:
    ScheduledExecutorPtr scheduledExecutorPtr;
    volatile bool _stop;
};


TEST(schedule_executor_test, schedule_executor_basic_test)
{
    ScheduledExecutor executor("schedule_test");
    boost::shared_ptr<ExecutorTask> taskPtr;
    volatile uint32_t globalSeq = 0;
    volatile bool exit = false;

    executor.start();

    // basic test
    LOGI("schedule_executor_test: basic test.")
    taskPtr.reset(new TestScheduleTask1("s0", 0, &globalSeq));
    executor.addTask(taskPtr, 1000, 1);

    boost::this_thread::sleep(boost::posix_time::seconds(1));

    taskPtr.reset(new TestScheduleTask1("s1", 1, &globalSeq));
    executor.addTask(taskPtr, 2000, 1);

    taskPtr.reset(new ExitTask("exitTask", &exit));
    executor.addTask(taskPtr, 10000, 1);

    while(!exit)
    {
        boost::this_thread::sleep(boost::posix_time::seconds(1));
    }
    executor.stop();
}


TEST(schedule_executor_test, schedule_executor_disorder_cancel_test)
{
    ScheduledExecutor executor("schedule_test");
    boost::shared_ptr<ExecutorTask> taskPtr;
    uint32_t globalSeq = 0;
    volatile bool exit = false;

    // disorder
    LOGI("schedule_executor_test: disorder test.")
    globalSeq = 0;
    taskPtr.reset(new TestScheduleTask1("s1", 1, &globalSeq));
    executor.addTask(taskPtr, 2000, 1);
    taskPtr.reset(new TestScheduleTask1("s0", 0, &globalSeq));
    executor.addTask(taskPtr, 1000, 1);

    taskPtr.reset(new ExitTask("exitTask", &exit));
    executor.addTask(taskPtr, 5000, 1);

    executor.start();
    while(!exit)
    {
        boost::this_thread::sleep(boost::posix_time::seconds(1));
    }
    executor.stop();
}


TEST(schedule_executor_test, schedule_executor_disorder_test)
{
    ScheduledExecutor executor("schedule_test");
    boost::shared_ptr<ExecutorTask> taskPtr;
    uint32_t globalSeq = 0;
    volatile bool exit = false;

    // disorder
    LOGI("schedule_executor_test: disorder test.")
    globalSeq = 0;
    taskPtr.reset(new TestScheduleTask1("s1", 1, &globalSeq));
    executor.addTask(taskPtr, 2000, 1);
    taskPtr.reset(new TestScheduleTask1("s0", 0, &globalSeq));
    executor.addTask(taskPtr, 1000, 1);
    taskPtr.reset(new TestScheduleTask1("s2", 2, &globalSeq));
    executor.addTask(taskPtr, 3000, 1);


    taskPtr.reset(new TestScheduleTask1("s5", 5, &globalSeq));
    executor.addTask(taskPtr, 6000, 1);
    taskPtr.reset(new TestScheduleTask1("s3", 3, &globalSeq));
    executor.addTask(taskPtr, 4000, 1);
    taskPtr.reset(new TestScheduleTask1("s4", 4, &globalSeq));
    executor.addTask(taskPtr, 5000, 1);


    // callTimes
    taskPtr.reset(new TestScheduleTask2("repeat5",5));
    executor.addTask(taskPtr, 1000, 5);

    taskPtr.reset(new ExitTask("exitTask", &exit));
    executor.addTask(taskPtr, 10000, 1);
    executor.start();

    while(!exit)
    {
        boost::this_thread::sleep(boost::posix_time::seconds(1));
    }
    executor.stop();
}

TEST(schedule_executor_test, schedule_executor_infinite_test)
{
    ScheduledExecutor executor("schedule_test");
    boost::shared_ptr<ExecutorTask> taskPtr;
    volatile bool exit = false;

    // disorder
    LOGI("schedule_executor_test: disorder test.");
    taskPtr.reset(new TestScheduleTask3("infiniteTask"));
    executor.addTask(taskPtr, 1000, -1);


    taskPtr.reset(new ExitTask("exitTask", &exit));
    executor.addTask(taskPtr, 10000, 1);
    executor.start();

    while(!exit)
    {
        boost::this_thread::sleep(boost::posix_time::seconds(1));
    }
    executor.stop();
}


TEST(schedule_executor_test, boost_timer_expiry_task)
{
    ExpiryTaskExecutor executor("expiry_task_test");
    executor.executeExpiryTask(10);
    executor.executeExpiryTask(-3600);

    executor.start();
    executor.stop();
}

TEST(schedule_executor_test, multi_thread_post_test)
{
    typedef boost::shared_ptr<AddScheduledTaskExecutor> AddScheduledTaskExecutorPtr;
    typedef std::vector<AddScheduledTaskExecutorPtr> AddScheduledTaskExecutorPtrVector;

    ScheduledExecutorPtr scheduledExecutorPtr(new ScheduledExecutor("schedule_test"));
    AddScheduledTaskExecutorPtrVector v;

    for (int i = 0; i < 20; ++i)
    {
        AddScheduledTaskExecutorPtr addScheduledTaskExecutorPtr(
                new AddScheduledTaskExecutor("addTaskEx", scheduledExecutorPtr));
        addScheduledTaskExecutorPtr->start();
        v.push_back(addScheduledTaskExecutorPtr);
    }

    boost::this_thread::sleep(boost::posix_time::seconds(30));

    for(AddScheduledTaskExecutorPtrVector::iterator ip = v.begin(); ip != v.end(); ++ip)
    {
        (*ip)->stop();
        (*ip)->join();
    }
}


#define QUEUE_PUT_TEST(queue, data, expected_result) do {\
    int32_t a = data; \
    int32_t err = queue.put(a); \
    ASSERT_EX((err == expected_result), err, expected_result); \
} while(0);

#define QUEUE_GET_TEST(queue, expected_data) do {\
    int32_t a = queue.get(); \
    ASSERT_EX((a == expected_data), a, expected_data); \
} while(0);

#define QUEUE_PUT_TEST_TIMEOUT(queue, data, expected_result) do {\
    int32_t a = data; \
    int32_t err = queue.put(a, 100); \
    ASSERT_EX((err == expected_result), err, expected_result); \
} while(0);

#define QUEUE_GET_TEST_TIMEOUT(queue, expected_data, expected_result) do {\
    int32_t a; \
    int32_t err = queue.get(a, 100); \
    ASSERT_EX((err == expected_result), err, expected_result); \
    if (err == Pinpoint::SUCCESS) {ASSERT_EX((a == expected_data), a, expected_data);} \
} while(0);


TEST(SynchronizedFixedLengthQueue_test, basic_test)
{
    {
        SynchronizedFixedLengthQueue<int32_t> queue(1);

        QUEUE_PUT_TEST(queue, 1, Pinpoint::SUCCESS);
        QUEUE_PUT_TEST_TIMEOUT(queue, 2, Pinpoint::WAIT_TIMEOUT);

        QUEUE_GET_TEST(queue, 1);
        QUEUE_GET_TEST_TIMEOUT(queue, 2, Pinpoint::WAIT_TIMEOUT);

        QUEUE_PUT_TEST_TIMEOUT(queue, 1, Pinpoint::SUCCESS);
        QUEUE_PUT_TEST_TIMEOUT(queue, 2, Pinpoint::WAIT_TIMEOUT);

        QUEUE_GET_TEST_TIMEOUT(queue, 1, Pinpoint::SUCCESS);
        QUEUE_GET_TEST_TIMEOUT(queue, 2, Pinpoint::WAIT_TIMEOUT);
    }

    {
        int32_t start = 1;
        int32_t end = 10;

        SynchronizedFixedLengthQueue<int32_t> queue(end);

        std::vector<int32_t> vt;
        random_vector(vt, end);

        for (int32_t i = 0; i < end; ++i)
        {
            QUEUE_PUT_TEST_TIMEOUT(queue, vt[i], Pinpoint::SUCCESS);
        }

        std::vector<int32_t> rvt;
        int32_t err = queue.getAll(rvt, 100);
        ASSERT_EX((err == Pinpoint::SUCCESS), err, Pinpoint::SUCCESS);

        check_vector(vt, rvt);
    }
}

class LockForeverExecutor : public ThreadExecutor
{
public:
    LockForeverExecutor(const std::string &executorName, boost::timed_mutex* mutex_p)
            : ThreadExecutor(executorName), mutex_p(mutex_p)
    {

    }
protected:
    virtual void executeTask()
    {
        mutex_p->lock();
    }

    virtual void stopTask()
    {
        mutex_p->unlock();
    }

private:
    boost::timed_mutex* mutex_p;
};

TEST(SynchronizedFixedLengthQueue_test, lock_timeout)
{
    SynchronizedFixedLengthQueue<int32_t> queue(5);

    QUEUE_PUT_TEST(queue, 1, Pinpoint::SUCCESS);
    QUEUE_PUT_TEST(queue, 2, Pinpoint::SUCCESS);

    LockForeverExecutor lockForeverExecutor("lockForeverExecutor", &queue.monitor);
    lockForeverExecutor.start();

    boost::this_thread::sleep(boost::posix_time::milliseconds(100));

    QUEUE_PUT_TEST_TIMEOUT(queue, 3, Pinpoint::WAIT_TIMEOUT);
    QUEUE_GET_TEST_TIMEOUT(queue, 2, Pinpoint::WAIT_TIMEOUT);

    std::vector<int32_t> rvt;
    int32_t err = queue.getAll(rvt, 100);
    ASSERT_EX((err == Pinpoint::WAIT_TIMEOUT), err, Pinpoint::WAIT_TIMEOUT);

    lockForeverExecutor.stop();
    lockForeverExecutor.join();

    err = queue.getAll(rvt, 100);
    ASSERT_EX((err == Pinpoint::SUCCESS), err, Pinpoint::SUCCESS);
    ASSERT_EX((rvt[0] == 1), rvt[0], 1);
    ASSERT_EX((rvt[1] == 2), rvt[1], 2);
}


const int32_t STOP_MSG = 90000;

uint32_t rand_between(uint32_t a, uint32_t b)
{
    assert (a < b);
    uint32_t x = (uint32_t)::rand();
    return x % (b + 1 - a) + a;
}

class RandomReaderExecutor : public ThreadExecutor
{
public:
    RandomReaderExecutor(const std::string &executorName,
                         SynchronizedFixedLengthQueue<int32_t>* queue_p)
            : ThreadExecutor(executorName), queue_p(queue_p)
    {

    }

    std::vector<int32_t>& getData()
    {
        return data;
    }

protected:
    virtual void executeTask()
    {
        while(true)
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(rand_between(1, 100)));

            int32_t d;
            if(probability(50))
            {
                d = queue_p->get();
            }
            else
            {
                int32_t err = queue_p->get(d, rand_between(1, 100));
                if (err != Pinpoint::SUCCESS)
                {
                    continue;
                }
            }

            if (d == STOP_MSG)
            {
                break;
            }

            data.push_back(d);
        }
    }

    virtual void stopTask()
    {

    }

private:
    std::vector<int32_t> data;
    SynchronizedFixedLengthQueue<int32_t>* queue_p;
};


class RandomWriterExecutor : public ThreadExecutor
{
public:
    RandomWriterExecutor(const std::string &executorName,
                         std::vector<int32_t>& data,
                         SynchronizedFixedLengthQueue<int32_t>* queue_p)
            : ThreadExecutor(executorName), data(data), queue_p(queue_p)
    {

    }


protected:
    virtual void executeTask()
    {
        std::reverse(data.begin(), data.end());
        while(!data.empty())
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(rand_between(1, 100)));

            int32_t d = data.back();
            data.pop_back();

            if(probability(50))
            {
                queue_p->put(d);
            }
            else
            {
                int32_t err = queue_p->put(d, rand_between(1, 100));
                if (err != Pinpoint::SUCCESS)
                {
                    data.push_back(d);
                    continue;
                }
            }
        }

        int32_t stop_msg = STOP_MSG;
        queue_p->put(stop_msg);
    }

    virtual void stopTask()
    {

    }

private:
    std::vector<int32_t> data;
    SynchronizedFixedLengthQueue<int32_t>* queue_p;
};


TEST(SynchronizedFixedLengthQueue_test, random_test)
{
    SynchronizedFixedLengthQueue<int32_t> queue(50);
    std::vector<int32_t> vt;

    random_vector(vt, 1000);

    RandomReaderExecutor randomReaderExecutor("randomReaderExecutor", &queue);
    RandomWriterExecutor randomWriterExecutor("randomWriterExecutor", vt, &queue);

    randomReaderExecutor.start();
    randomWriterExecutor.start();

    randomReaderExecutor.join();
    randomWriterExecutor.join();

    check_vector(vt, randomReaderExecutor.getData());
}
