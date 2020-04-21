//
// Created by 黎鑫 on 2020/4/21.
//

#include "EventLoopThreadPool.h"
#include "log/Logging.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *main_loop_, int thread_num)
    : main_loop_(main_loop_),
      thread_num_(thread_num),
      index_(0),
      thread_list_()
{
    if(thread_num_ > 0)
    {
        for (int i = 0; i < thread_num_; ++i)
        {
            thread_list_.push_back(new EventLoopThread);
        }
    }
}

EventLoopThreadPool::~EventLoopThreadPool()
{
    LOG << "INFO :: EventLoopThreadPool closed for ~EventLoopThreadPool()";
    for(auto thread : thread_list_)
    {
        delete thread;
    }
    thread_list_.clear();
}

EventLoop* EventLoopThreadPool::GetNextLoop()
{
    if(thread_num_ == 0)
    {
        return main_loop_;
    }
    EventLoop* loop = thread_list_[index_]->GetLoop();
    index_ = (index_ + 1) % thread_num_;
    return loop;
}

void EventLoopThreadPool::StartThread()
{
    if(thread_num_ > 0)
    {
        for(auto thread : thread_list_)
        thread->StartLoop();
    }
}
