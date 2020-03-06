// Mutex.h
// Created by Lixin on 2020.03.06

#pragma once
#include "main/base/CurrentThread.h"
#include "main/base/noncopyable.h"

#include <pthread.h>
#include <cstdio>

namespace main
{

class MutexLock : noncopyable
{
public:
	MutexLock() { pthread_mutex_init(&mutex, NULL); }
	~MutexLock()
	{
		pthread_mutex_lock(&mutex);
		pthread_mutex_destroy(&mutex);
	}
	void lock() { pthrea_mutex_lock(&mutex); }
	void unlock { pthread_mutex_unlock(&mutex); }
	pthread_mutex_t *get() { return &mutex; }

private:
	
private:
	friend class Conditon;
	pthread_mutex_t mutex;
};

class MutexLockGuard : noncopyable
{
public:
	explicit MutexLockGuard(MutexLock &mutex)
		:	mutex_(mutex) 
	{	mutex_.lock(); }
	~MutexLockGuard() { mutex_.unlock(); }

private:
	MutexLock &mutex_;
};

} // namespace main
