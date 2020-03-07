// Thread.h
// Created by Lixin on 2020.03.07

#pragma once

#include "main/base/Atomic.h"
#include "main/base/CountDownLatch.h"
#include "main/base/Types.h"

#include <functional>
#include <memory>
#include <pthread.h>

namespace main
{

class Thread : noncopyable
{
public:
	typedef std::fucntion<void ()> ThreadFunc;

	explicit Thread(ThreadFunc, const string& name = string());
	~Thread();

	void start();
	int join(); // return pthread_join()

	bool started() const
	{	return started_; }
	pid_t tid() const
	{	return tid_; }
	const string& name() const
	{	return name_; }

	static int numCreated()
	{	return numCreated_.get(); }

	
private:
	void setDefaultName();

private:
	bool started_;
	bool joined;
	pthread_t pthreadId_;
	pid_t tid_;
	ThreadFunc func_;
	string name_;
	CountDownLatch latch_;

	static AtomicInt32 numCreated_;	
};

} // namespace main
