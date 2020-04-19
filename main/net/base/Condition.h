//
// Created by 黎鑫 on 2020/4/16.
//

#ifndef MYPROJECT_CONDITION_H
#define MYPROJECT_CONDITION_H

#include "noncopyable.h"
#include "MutexLock.h"

#include <sys/_pthread/_pthread_cond_t.h>
#include <errno.h>



class Condtion : noncopyable
{
public:
    explicit Condtion(MutexLock& mutex)
        : mutex_(mutex)
    { pthread_cond_init(&cond_, nullptr); }

    ~Condtion() { pthread_cond_destroy(&cond_); }

    void Wait() { pthread_cond_wait(&cond_, mutex_.get()); }

    bool TimedWait(int seconds)
    {
        struct timespec tsp;
        clock_gettime(CLOCK_REALTIME, &tsp);
        tsp.tv_sec += static_cast<time_t>(seconds);
        int n = pthread_cond_timedwait(&cond_, mutex_.get(), &tsp);
        return n == ETIMEDOUT;
    }

    void Notify() { pthread_cond_signal(&cond_); }
    void NotifyAll() { pthread_cond_broadcast(&cond_); }


private:
    pthread_cond_t cond_;
    MutexLock& mutex_;
};

#endif //MYPROJECT_CONDITION_H
