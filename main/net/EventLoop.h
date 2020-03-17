// EventLoop.h
// Created by Lixin on 2020.02.11

#pragma once 

#include "Channel.h"
#include "Epoll.h"
#include "Util.h"
#include "base/CurrentThread.h"
#include "base/Logging.h"
#include "base/Thread.h"
#include "base/noncopyable.h"

#include <functional>
#include <memory>
#include <vector>

#include <iostream>

class EventLoop : noncopyable
{
public:
	typedef std::function<void()> Functor; // 必须有确定的函数类型
  	
	EventLoop();
  	~EventLoop();
  	
	void loop();
  	void quit();
  	
	void runInLoop(Functor&& cb);
  	void queueInLoop(Functor&& cb);
  	
	bool isInLoopThread() const 
	{	return threadId_ == CurrentThread::tid(); } // 获取tid
  	void assertInLoopThread() 
	{	assert(isInLoopThread()); }
  	
	void shutdown(shared_ptr<Channel> channel) 
	{	shutDownWR(channel->getFd()); } // 仅关闭写端

	void removeFromPoller(shared_ptr<Channel> channel) 
	{	poller_->epoll_del(channel);}
  	void updatePoller(shared_ptr<Channel> channel, int timeout = 0) 
	{	poller_->epoll_mod(channel, timeout); }
  	void addToPoller(shared_ptr<Channel> channel, int timeout = 0) 
	{	poller_->epoll_add(channel, timeout); }

private:
 	void wakeup();
  	void handleRead();
  	void doPendingFunctors();
  	void handleConn();

private:
	// 状态参数方便GDB调试时保证loop的状态时正确的
  	bool looping_;
  	shared_ptr<Epoll> poller_;
  	int wakeupFd_;
  	bool quit_;
  	bool eventHandling_;
  	mutable MutexLock mutex_;
  	std::vector<Functor> pendingFunctors_;
  	bool callingPendingFunctors_;
  	const pid_t threadId_;
  	shared_ptr<Channel> pwakeupChannel_; // 注意channel都保持是shared_ptr
};


