//
// Created by 黎鑫 on 2020/4/21.
//

#include "EventLoopThread.h"
#include "log/Logging.h"

EventLoopThread::EventLoopThread(string thread_name)
    : thread_(),
      tid_(),
      thread_name_(thread_name),
      loop_(nullptr)
{

}

EventLoopThread::~EventLoopThread()
{
    LOG << "INFO :: Close EventLoopThread : " << GetTid() ;
    loop_->ShutDown();
    thread_.join();
}

void EventLoopThread::StartLoop()
{
    thread(&EventLoopThread::ThreadFunc, this);
}

void EventLoopThread::ThreadFunc()
{
    EventLoop eventLoop;
    loop_ = &eventLoop;
    tid_ = get_id();

    LOG << "INFO :: EventLoopThread  " << thread_name_ << " " << GetTid() << "start loop";
    loop_->Loop();
}


