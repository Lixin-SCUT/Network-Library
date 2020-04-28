//
// Created by 黎鑫 on 2020/4/28.
//

#ifndef MYPROJECT_WORKTHREADPOOL_H
#define MYPROJECT_WORKTHREADPOOL_H

#include "base/noncopyable.h"
#include "base/MutexLock.h"
#include "base/Condition.h"

#include <thread>
#include <vector>
#include <queue>

using std::thread;
using std::vector;
using std::queue;
using std::function;

class WorkThreadPool : noncopyable
{
public:
    WorkThreadPool(int thread_num);
    ~WorkThreadPool();

    int GetThreadNum()
    {   return thread_num_; }

    void Start();
    void Stop();
    void AddTask(function<void()> task);

    void ThreadFunc();

private:
    int thread_num_;
    bool state_;

    vector<thread*> thread_list_;
    queue<function<void()>> task_queue_;
    MutexLock mutex_;
    Condtion cond_;
};

#endif //MYPROJECT_WORKTHREADPOOL_H
