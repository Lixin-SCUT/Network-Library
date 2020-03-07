// BlockingQueue.h
// Created by Lixin on 2020.03.07

#pragma once

#include "/main/base/Mutex.h"
#include "/main/base/Condition.h"
#include "/main/base/noncopyable.h"

#include <deque>
#include <assert.h>

namespace main
{

template<typename T>
class BlockingQueue : noncopyable
{
public:
	BlockingQueue()
		:	mutex_(),
			notEmpty_(mutex_),
			queue_()
	{ }

	void put(const T &x)
	{
		MutexLockGuard lock(mutex_);
		queue.push_back(x);
		notEmpty_.notify(); // wait morphing saves us 
	}

	void put(T&& x)
	{
		MutexLockGuard lock(mutex_);
		queue_.push_back(std::move(x));
		notEmpty_.notify();
	}

	T take()
	{
		MutexLockGuard lock(mutex_);
		// always use a while-loop, due to spurious wakeup
		while(queue_.empty())
		{
			notEmpty_.wait();
		}
		assert(!queue_.empty());
		T front(std::move(queue_.front()));
		queue_.pop_front();
		return front;
	}

	size_t size() const
	{
		MutexLockGuard lock(mutex_);
		return queue_.size();
	}

private:
	mutable MutexLock mutex_;
	Conditon notEmpty_ GUARDED_BY(mutex_);
	std::deque<T> queue_ GUARDED_BY(mutex_);
};
} // namespace main
