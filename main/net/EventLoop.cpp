//
// Created by 黎鑫 on 2020/4/21.
//

#include "EventLoop.h"
#include "log/Logging.h"

#include <sys/eventfd.h>
#include <unistd.h>

using std::this_thread::get_id;
using std::bind;

EventLoop::EventLoop()
    : state_(true),
      tid_(get_id()),
      wakefd_(-1),
      wake_channel_(),
      poller_(),
      mutex_(),
      activechannel_list_(),
      functor_list_()
{
    wakefd_ = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if(wakefd_ == -1)
    {
        LOG << "ERROR : eventfd() for EventLoop";
        exit(-1);
    }
    wake_channel_.SetFd(wakefd_);
    wake_channel_.SetEvent(EPOLLIN | EPOLLET);
    wake_channel_.SetReadCallBack(bind(&EventLoop::HandleRead, this));
    wake_channel_.SetErrorCallBack((bind(&EventLoop::HandleError, this)));
    AddToPoller(&wake_channel_);
}

EventLoop::~EventLoop()
{
    close(wakefd_);
}

void EventLoop::Loop()
{
    Channel* channel = nullptr;
    while(state_)
    {
        poller_.EpollWait(activechannel_list_);
        for(int i = 0; i < activechannel_list_.size(); ++i)
        {
            channel = activechannel_list_[i];
            channel->HandleEvents();
        }
        HandleFunctor();
        activechannel_list_.clear();
    }
}

void EventLoop::HandleRead()
{
    uint64_t one = 1;
    ssize_t n = read(wakefd_, static_cast<void*>(&one), sizeof(one));
}

void EventLoop::HandleError()
{
    LOG << "ERROR : HandleError() for EventLoop";
    exit(-1);
}

void EventLoop::QueueInLoop(Functor cb)
{
    MutexLockGuard lock(mutex_);
    functor_list_.push_back(cb);
}

void EventLoop::WakeUp()
{
    uint64_t one = 1;
    ssize_t n = write(wakefd_, static_cast<void*>(&one), sizeof(one));
}

void EventLoop::HandleFunctor()
{
    vector<Functor> functor_list;
    {
        MutexLockGuard lock(mutex_);
        functor_list.swap(functor_list_);
    }

    for(int i = 0; i < functor_list.size(); ++i) {
        functor_list[i]();
    }
}
