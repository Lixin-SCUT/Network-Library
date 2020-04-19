//
// Created by 黎鑫 on 2020/4/16.
//

#ifndef MYPROJECT_MUTEXLOCK_H
#define MYPROJECT_MUTEXLOCK_H

#include <sys/_pthread/_pthread_mutex_t.h>
#include <pthread.h>
#include "noncopyable.h"

class MutexLock : noncopyable
{
public:
    MutexLock()
    { pthread_mutex_init(&mutex_, nullptr); }

    ~MutexLock()
    {
        pthread_mutex_lock(&mutex_);
        pthread_mutex_destroy(&mutex_);
    }

    void Lock()
    { pthread_mutex_lock(&mutex_); }

    void Unlock()
    { pthread_mutex_unlock(&mutex_); }

    pthread_mutex_t* get()
    { return &mutex_;}

private:
    pthread_mutex_t mutex_;
};

class MutexLockGuard : noncopyable
{
public:

    explicit MutexLockGuard(MutexLock& mutex)
        : mutex_(mutex)
    { mutex_.Lock(); }

    ~MutexLockGuard()
    { mutex_.Unlock(); }

private:
    MutexLock& mutex_;
};
#endif //MYPROJECT_MUTEXLOCK_H
