// BoundedBlockingQueue.h
// Created by Lixin on 2020.03.07

#pragma once

#include "main/base/Mutex.h"
#include "main/base/Condition.h"

#include <boost/circular_buffer.hpp>
#include <assert.h>

namespace main
{

template<typename T>
class BoundedBlockingQueue : noncopyable
{
public:
	explicit BoundedBlockingQueue(int maxSize)
		:	mutex_(),
			notEmpty_(mutex_),
			notFull_(mutex_),
			queue_(maxSize)
	{ }

	void put(const T &x)
	{
		MutexLockGuard lock(mutex_);
		while(queue_.full())
		{
			notFull_.wait();
		}
		assert(!queue_.full());
		queue_.push_back(x);
		notEmpty_.notify();
	}

	void put(T&& x)
	{
		MutexLockGuard lock(mutex_);
		while(queue_.full())
		{
			notFull_.wait();
		}
		assert(!queue_.full());
    	queue_.push_back(x);
    	notEmpty_.notify();
	}

	void take()
	{
		MutexLockGuard lock(mutex_);
		while(queue_.empty())
		{
			notEmpty_.wait();
		}
		assert(!queue_.empty());
		T front(std::move(queue_.front()));
		queue_.pop_front();
		notFull_.notify();
		return front;
	}

	bool empty() const 
	{
		MutexLockGuard lock(mutex);
		return queue_.empty();
	}

	size_t size() const
	{
		MutexLockGuard lock(mutex_);
		return queue_.size();
	}

	size_t capacity() const
	{
		MutexLockGuard lock(mutex_);
		return queue_.capacity();
	}

private:
	mutable MutexLock mutex_;
	Condition notEmpty_ GUARDED_BY(mutex_);
	Condition notFull_ GUARDED_BY(mutex_);
	boost::circurlar_buffer<T> queue_ GUARDED_BY(mutex_);
};
} // namespace main
