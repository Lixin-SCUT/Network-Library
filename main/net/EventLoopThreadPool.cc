// EventLoopThreadPool.cc
// Created by Lixin on 2020.03.07

#include "/main/net/EventLoopThreadPool.h"

using namespace main;
using namespace main::net;

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

