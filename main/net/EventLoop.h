// EventLoop.h
// Created by Lixin on 2020.02.11

#pragma once 

#include "Channel.h"
#include "Epoll.h"
#include "Util.h"
#include "base/CurrentThread.h"
#include "base/Logging.h"
#include "base/Thread.h"

#include <functional>
#include <memory>
#include <vector>

#include <iostream>

class EventLoop {
public:
	typedef std::function<void()> Functor;
  	
	EventLoop();
  	~EventLoop();
  	
	void loop();
  	void quit();
  	
	void runInLoop(Functor&& cb);
  	void queueInLoop(Functor&& cb);
  	
	bool isInLoopThread() const 
	{	return threadId_ == CurrentThread::tid(); }
  	void assertInLoopThread() 
	{	assert(isInLoopThread()); }
  	
	void shutdown(shared_ptr<Channel> channel) 
	{	shutDownWR(channel->getFd()); }
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
  	bool looping_;
  	shared_ptr<Epoll> poller_;
  	int wakeupFd_;
  	bool quit_;
  	bool eventHandling_;
  	mutable MutexLock mutex_;
  	std::vector<Functor> pendingFunctors_;
  	bool callingPendingFunctors_;
  	const pid_t threadId_;
  	shared_ptr<Channel> pwakeupChannel_;
};


