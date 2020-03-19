// EventLoop.cc
// Created by Lixin on 2020.02.13

// 兼顾了接受新连接的主事件循环和I/O操作的子事件循环

#include "EventLoop.h"

#include "Util.h"
#include "/base/Logging.h"

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <iostream>

using namespace std; // TODO delete
	
__thread EventLoop* t_loopInThisThread = nullptr;

int createEventfd() 
{
	int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC); // 注册wakeupEventFd
	if (evtfd < 0) 
	{
		LOG << "Failed in eventfd";
		abort();
	}
	return evtfd;
}

EventLoop::EventLoop()
	: looping_(false),
	  poller_(new Epoll()),
	  wakeupFd_(createEventfd()),
	  quit_(false),
	  eventHandling_(false),
	  callingPendingFunctors_(false),
	  threadId_(CurrentThread::tid()),
	  pwakeupChannel_(new Channel(this, wakeupFd_)) // 这个wakeupFd专门用于跨线程调用
{
	if (t_loopInThisThread != nullptr) 
	{
		t_loopInThisThread = this; // 先注册一下loop
	}
	pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
	pwakeupChannel_->setReadHandler(bind(&EventLoop::handleRead, this)); // 用于设置监听事件
	pwakeupChannel_->setConnHandler(bind(&EventLoop::handleConn, this)); // 用于执行跨线程调用请求
	poller_->epoll_add(pwakeupChannel_, 0); // 往epoll注册一下
}

void EventLoop::handleConn() 
{
	updatePoller(pwakeupChannel_, 0); // 日常在最后更新一下poller（如果不变的话不需要更新）
}

EventLoop::~EventLoop() 
{
	close(wakeupFd_);
	t_loopInThisThread = nullptr;
}

void EventLoop::wakeup() 
{
	uint64_t one = 1;
	ssize_t n = writen(wakeupFd_, (char*)(&one), sizeof(one)); // 仅写入一个1，引发可读事件
	if (n != sizeof(one)) 
	{
		LOG << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
	}
}

void EventLoop::handleRead() 
{
	uint64_t one = 1;
	ssize_t n = readn(wakeupFd_, &one, sizeof(one));
	if (n != sizeof(one)) 
	{
		LOG << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
	}
	pwakeupChannel_->setEvents(EPOLLIN | EPOLLET); // 重设events（记得重设为0了）
}

void EventLoop::runInLoop(Functor&& cb) // 跨线程调用函数
{
	if (isInLoopThread())
	{
		cb();
	}
	else
	{	
		queueInLoop(std::move(cb)); // 实际的跨线程调用函数，利用移动赋值
	}
}

void EventLoop::queueInLoop(Functor&& cb) 
{
	{
		MutexLockGuard lock(mutex_); // 一定要上锁，防止多线程同时读取
		pendingFunctors_.emplace_back(std::move(cb));
	}

	if (!isInLoopThread() || callingPendingFunctors_) 
	{	
		wakeup();
	}
}

void EventLoop::loop() 
{
	assert(!looping_);
	assert(isInLoopThread());
	looping_ = true;
	quit_ = false;
	// LOG_TRACE << "EventLoop " << this << " start looping";
	std::vector<SP_Channel> ret;
	while (!quit_) 
	{
		ret.clear();
		ret = poller_->poll();
		eventHandling_ = true;
		for (auto& it : ret) 
		{
			it->handleEvents();
		}
		eventHandling_ = false;
		doPendingFunctors(); // 日常执行跨线程调用请求
		poller_->handleExpired(); // 处理超时事件
	}
	looping_ = false;
}

void EventLoop::doPendingFunctors() 
{
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;
	
	{
		MutexLockGuard lock(mutex_); // 一定要上锁，防止多线程同时写入
		functors.swap(pendingFunctors_);
	}

	for (size_t i = 0; i < functors.size(); ++i) 
	{
		functors[i](); // 执行跨线程调用请求
	}
	callingPendingFunctors_ = false;
}

void EventLoop::quit() 
{
	quit_ = true;
	if (!isInLoopThread()) 
	{
		wakeup();
	}
}
