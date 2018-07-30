/*
 * File:   pinpoint_lock.h
 * Author: bluse
 *
 * Created on January 22, 2017, 11:37 AM
 */

#ifndef PINPOINT_LOCK_H
#define PINPOINT_LOCK_H

#include "pinpoint_api.h"
#include "os.h"
#include "errno.h"

#include <boost/log/detail/config.hpp>
namespace Pinpoint {
    using namespace log;
class PinPointLock
{
public:
    virtual int lock() = 0;
    virtual int try_lock(int64_t i64Msec)=0;
    virtual int unlock() = 0;
    virtual ~PinPointLock(){}
    public:
        char pLockName[128];
protected:
        PinPointLock()
        {
            strcpy(pLockName, "PinPointLock");
        }
protected:
    PinPointLock(const PinPointLock&);
    PinPointLock& operator=(const PinPointLock&);
};




class MutexLock : public PinPointLock
{
public:

    MutexLock()
    {


            pthread_mutex_init(&_mutex, NULL);
    }

    virtual ~MutexLock()
    {


        pthread_mutex_destroy(&_mutex);
    }

    int lock()
    {
        return pthread_mutex_lock(&_mutex);
    }

    int try_lock(int64_t i64Msec)
    {
        struct  timespec _timeval= {0,i64Msec*1000000};
        return  pthread_mutex_timedlock(&_mutex,&_timeval);
    }

    int unlock()
    {
        return pthread_mutex_unlock(&_mutex);
    }
private:
    pthread_mutex_t _mutex;
};

class ScopeLock
{
private:
    PinPointLock *_lock;
public:

    ScopeLock(PinPointLock* lock)
    {
        _lock = lock;
        assert(_lock);
        _lock->lock();
    }

    virtual ~ScopeLock()
    {
        _lock->unlock();
    }
};


#ifdef __GNUC__
#define DEBUG_MUTEXLOCK_PS
    struct pthread_mutex_attributes
    {
        pthread_mutexattr_t attrs;
        pthread_mutex_attributes()
        {
            int err = pthread_mutexattr_init(&this->attrs);
            if (BOOST_UNLIKELY(err != 0))
                LOGI("Failed to initialize pthread mutex attributes %s", strerror(err));
        }
        ~pthread_mutex_attributes()
        {
            BOOST_VERIFY(pthread_mutexattr_destroy(&this->attrs) == 0);
        }
        BOOST_DELETED_FUNCTION(pthread_mutex_attributes(pthread_mutex_attributes const&))
        BOOST_DELETED_FUNCTION(pthread_mutex_attributes& operator=(pthread_mutex_attributes const&))
    };
    class PSMutexLock : public PinPointLock
    {
    private:
        pthread_mutex_t mutex;
        volatile pid_t ownPid;
    private:
        int recover()
        {
            int err = pthread_mutex_consistent(&this->mutex);
            if (BOOST_UNLIKELY(err != 0)) {
                LOGI("Failed to recover pthread mutex from a crashed thread  %d -> %s", errno, strerror(err));
            } else {
                LOGI("pid %d own this mutex", getpid());
                ownPid = getpid();
            }
            return err;
        }
    public:
        PSMutexLock()
        {
            strcpy(pLockName, "MutexLock_PS");
            struct pthread_mutex_attributes attrs;
            int err = pthread_mutexattr_init(&attrs.attrs);
            if (BOOST_UNLIKELY(err != 0)) {
                LOGI("Failed to set pthread_mutexattr_init  %d -> %s", errno, strerror(err));
            }
            err = pthread_mutexattr_setpshared(&attrs.attrs, PTHREAD_PROCESS_SHARED);
            if (BOOST_UNLIKELY(err != 0)) {
                LOGI("Failed to make pthread mutex process-shared  %d -> %s", errno, strerror(err));
            }
            err = pthread_mutexattr_settype(&attrs.attrs, PTHREAD_MUTEX_RECURSIVE /*PTHREAD_MUTEX_NORMAL*/);
            if (BOOST_UNLIKELY(err != 0)) {
                LOGI("Failed to set pthread mutex type  %d -> %s", errno, strerror(err));
            }
            err = pthread_mutexattr_setrobust(&attrs.attrs, PTHREAD_MUTEX_ROBUST);
            if (BOOST_UNLIKELY(err != 0)) {
                LOGI("Failed to make pthread mutex robust  %d -> %s", errno, strerror(err));
            }
            err = pthread_mutex_init(&mutex, &attrs.attrs);
            if (BOOST_UNLIKELY(err != 0)) {
                LOGI("Failed to initialize pthread mutex  %d -> %s", errno, strerror(err));
            }
            ownPid = getpid();
        }
        virtual ~PSMutexLock()
        {
            pthread_mutex_destroy(&this->mutex);
        }
        int lock()
        {
            int err = pthread_mutex_lock(&this->mutex);
            if (BOOST_UNLIKELY(err == EOWNERDEAD)) {
                LOGI("mutext owner dead");
                return recover();
            }
            if (BOOST_UNLIKELY(err != 0)) {
                LOGI("Failed to lock pthread mutex %d -> %s", errno, strerror(err));
            }
            return err;
        }
        int unlock()
        {
            return pthread_mutex_unlock(&this->mutex);
        }
        int try_lock(int64_t i64Msec)
        {
            struct timespec abs_time;
            clock_gettime(CLOCK_REALTIME, &abs_time);
            abs_time.tv_nsec += (i64Msec * 1000000) % (1000000000);
            abs_time.tv_sec += (i64Msec * 1000000) / (1000000000);
            int err = pthread_mutex_timedlock(&mutex, &abs_time);
            if (BOOST_UNLIKELY(err == EOWNERDEAD)) {
                LOGI("mutext owner dead");
                return recover();
            } else if (err != ETIMEDOUT && err != 0) {
                LOGI("Failed to lock pthread mutex %d -> %s", errno, strerror(err));
            }
            return err;
        }
        pid_t getCurrentOwner() const
        {
            return ownPid;
        }
    };
#endif
}
#endif /* PINPOINT_LOCK_H */

