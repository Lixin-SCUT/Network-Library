// EventLoopThread.h
// Created by Lixin on 2020.02.17

#ifndef MAIN_NET_EVENTLOOPTHREAD_H
#define MAIN_NET_EVENTLOOPTHREAD_H

#include "main/base/Condition.h"
#include "main/base/Mutex.h"
#include "main/base/Thread.h"

namespace main
{
namespace net
{

class EventLoop;

class EventLoopThread : noncopyable
{
 public:
  typedef std::function<void(EventLoop*)> ThreadInitCallback;

  EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
                  const string& name = string());
  ~EventLoopThread();
  EventLoop* startLoop();

 private:
  void threadFunc();

  EventLoop* loop_ GUARDED_BY(mutex_);
  bool exiting_;
  Thread thread_;
  MutexLock mutex_;
  Condition cond_ GUARDED_BY(mutex_);
  ThreadInitCallback callback_;
};

}  // namespace net
}  // namespace main

#endif  // MAIN_NET_EVENTLOOPTHREAD_H
