//
// Created by 黎鑫 on 2020/4/21.
//

#ifndef MYPROJECT_EVENTLOOP_H
#define MYPROJECT_EVENTLOOP_H


#include "Poller.h"
#include "Channel.h"
#include "base/MutexLock.h"

#include<thread>

using std::thread;
class EventLoop : noncopyable
{
public:
    typedef std::function<void()> Functor;

    EventLoop();
    ~EventLoop();

    void AddToPoller(Channel* channel)
    {   poller_.AddChannel(channel); }
    void RemovePoller(Channel* channel)
    {   poller_.RemoveChannel(channel); }
    void UpdatePoller(Channel* channel)
    {   poller_.UpdateChannel(channel); }

    void Loop();
    void HandleRead();
    void HandleError();

    void QueueInLoop(Functor cb);
    void WakeUp();
    void HandleFunctor();

    thread::id GetTid() const { return tid_; }
    void ShutDown() { state_ = false; }

private:
    bool state_;
    thread::id tid_;
    int wakefd_;
    Channel wake_channel_;
    Poller poller_;
    MutexLock mutex_;

    vector<Channel*> activechannel_list_;
    vector<Functor> functor_list_;


};
#endif //MYPROJECT_EVENTLOOP_H
