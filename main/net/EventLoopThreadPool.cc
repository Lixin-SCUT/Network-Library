// EventLoopThreadPool.cc
// Created by Lixin on 2020.03.07

#include "/main/net/EventLoopThreadPool.h"

#include "muduo/net/EventLoop.h"
#include "muduo/net/EventLoopThread.h"

using namespace main;
using namespace main::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const string& nameArg)
  : baseLoop_(baseLoop),
    name_(nameArg),
    started_(false),
    numThreads_(0),
    next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
  // Don't delete loop, it's stack variable
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
  assert(!started_);
  baseLoop_->assertInLoopThread();

  started_ = true;

  for (int i = 0; i < numThreads_; ++i)
  {
    char buf[name_.size() + 32];
    snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);
    EventLoopThread* t = new EventLoopThread(cb, buf);
    threads_.push_back(std::unique_ptr<EventLoopThread>(t));
    loops_.push_back(t->startLoop());
  }
  if (numThreads_ == 0 && cb)
  {
    cb(baseLoop_);
  }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
  baseLoop_->assertInLoopThread();
  assert(started_);
  EventLoop* loop = baseLoop_;

  if (!loops_.empty())
  {
    // round-robin
    loop = loops_[next_];
    ++next_;
    if (implicit_cast<size_t>(next_) >= loops_.size())
    {
      next_ = 0;
    }
  }
  return loop;
}

EventLoop* EventLoopThreadPool::getLoopForHash(size_t hashCode)
{
  baseLoop_->assertInLoopThread();
  EventLoop* loop = baseLoop_;

  if (!loops_.empty())
  {
    loop = loops_[hashCode % loops_.size()];
  }
  return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
  baseLoop_->assertInLoopThread();
  assert(started_);
  if (loops_.empty())
  {
    return std::vector<EventLoop*>(1, baseLoop_);
  }
  else
  {
    return loops_;
  }
}
/*
EventLoopThreadPool::EventLoopThreadPool(EventLop *baseLoop, int numThreads)
	:	baseLoop_(baseLoop),
		started_(false),
		numThreads_(numThread),
		next_(0)
{
	if(numThreads_ <= 0)
	{
		LOG_INFO << "numThreads_ <= 0";
		abort();
	}
}

void EventLoopThreadPool::start()
{
	baseLoop_->assertInLoopThread();
	started_ = true;
	for(int i = 0; i < numThreads_; ++i)
	{
		std::shared_ptr<EventLoopThread> t(new EventLoopThread());
		threads_.push_back(t);
		loops_.push_back(t->startLoop());
	}
}

EventLoop *EventLoopThreadPool::getNextLoop()
{
	baseLoop_->assertInLoopThread();
	assert(started_);
	EventLoop *loop = baseLoop_;
	if(!loop_.empty())
	{
		loop = loop_[next_];
		next_ = (next_ + 1) % numThreads_;
	}
	return loop;
}
*/
