// EventLoopThreadPool.cc
// Created by Lixin on 2020.03.07
	
#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, int numThreads)
	: baseLoop_(baseLoop),  // 这里很关键，如果numThreads == 0，那么主线程作为单线程服务器
	  started_(false), 
	  numThreads_(numThreads), 
	  next_(0) 
{
	if (numThreads_ <= 0) 
	{
		LOG << "numThreads_ <= 0";
		abort();
	}
}

void EventLoopThreadPool::start() // 和TcpServer一起初始化，直到TcpServer启动后再开始
{
	baseLoop_->assertInLoopThread();
	started_ = true; // 直到此处才改变状态
	for (int i = 0; i < numThreads_; ++i) // 注意下标从0开始，不要造成多余一个线程
	{
		std::shared_ptr<EventLoopThread> ptr(new EventLoopThread());
		threads_.push_back(ptr);
		loops_.push_back(ptr->startLoop()); // 注意此处先启动了loop
	}
}

EventLoop *EventLoopThreadPool::getNextLoop() 
{
	baseLoop_->assertInLoopThread();
	assert(started_);
	EventLoop *loop = baseLoop_;
	if (!loops_.empty()) 
	{
		loop = loops_[next_];
		next_ = (next_ + 1) % numThreads_; // round-robin轮询调度，所有任务优先级相同
		// TODO 可以制造高优先级线程，仅为高优先级任务服务。
	}
	return loop;
}