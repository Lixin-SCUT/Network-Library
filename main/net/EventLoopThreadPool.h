//
// Created by 黎鑫 on 2020/4/21.
//

#ifndef MYPROJECT_EVENTLOOPTHREADPOOL_H
#define MYPROJECT_EVENTLOOPTHREADPOOL_H

#include "base/noncopyable.h"
#include "EventLoopThread.h"

const int kThreadNum = 8;
class EventLoopThreadPool : noncopyable
{
public:
    EventLoopThreadPool(EventLoop* main_loop_, int thread_num = 0);
    ~EventLoopThreadPool();

    EventLoop* GetNextLoop();

    void StartThread();

private:
    EventLoop* main_loop_;
    int thread_num_;
    int index_;

    vector<EventLoopThread*> thread_list_;
};
#endif //MYPROJECT_EVENTLOOPTHREADPOOL_H
