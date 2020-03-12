// EventLoopThreadPool.h
// Created by Lixin on 2020.03.07

#pragma once

#include "/main/base/noncopyable.h"
#include "/main/base/Types.h"

#include <functional>
#include <memory>
#include <vector>

namespace main
{
namespace net
{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : noncopyable
{
public:
	typedef std::function<void (EventLoop*)> ThreadInitCallback;
	
	EventLoopThreadPool(EventLoop* baseLoop, const string& nameArg);
  	~EventLoopThreadPool();
  	void setThreadNum(int numThreads) { numThreads_ = numThreads; }
	// valid after calling start()
  	// round-robin
  	EventLoop* getNextLoop();

  	// with the same hash code, it will always return the same EventLoop
  	EventLoop* getLoopForHash(size_t hashCode);

	std::vector<EventLoop*> getAllLoops();
	
	void start(const ThreadInitCallback& cb = ThreadInitCallback());
	
	bool started() const
	{	return started_; }
	
	const string& name() const
	{	return name_; }	
	
	/*
	EventLoopThreadPool（EventLoop* baseLoop, int numThreads;

	~EventLoopThreadPool();
	void start();

	EventLoop* getNextLoop();
	*/
private:
	EventLoop *baseLoop_;
	//EventLoop *loop;
	string name_;
	bool started_;
	int numThreads_;
	int next_;
	std::vector<std::shared_ptr<EventLoopThread>> threads_;
	std::vector<EventLoop*> loops_;
};

} // namespace net
} // namespace main


