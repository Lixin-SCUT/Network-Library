// EventLoopThread.cc
// Created by Lixin on 2020.02.17

#include "EventLoopThread.h"
#include <functional>

EventLoopThread::EventLoopThread()
	:	loop_(nullptr),
		exiting_(false),
		thread_(bind(&EventLoopThread::threadFunc, this), "EventLoopThread"),
		mutex_(),
		cond_(mutex_) 
{ }

EventLoopThread::~EventLoopThread() 
{
	exiting_ = true;
	if (loop_ != nullptr) 
	{
		loop_->quit();
		thread_.join();
	}
}

EventLoop* EventLoopThread::startLoop() 
{
	assert(!thread_.started());
	thread_.start(); // 这里会调用传给thread_的threadFunc
	{
		MutexLockGuard lock(mutex_);
		// 一直等到threadFun在Thread里真正跑起来
		while (nullptr == loop_) 
		{
			cond_.wait();
		}
	}
	return loop_;
}

void EventLoopThread::threadFunc() 
{
	EventLoop loop;

	{
		MutexLockGuard lock(mutex_);
		loop_ = &loop;
		cond_.notify();
	}

	loop.loop(); // 注意此处就启动了线程的事件循环，epoll只注册了wakeupFd
	// assert(exiting_);
	loop_ = nullptr; // 记得重置loop_,因为当前loop已经返回给ThreadPool了，不需要保留
}