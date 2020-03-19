// MutexLock.h
// Created by Lixin on 2020.03.06

#pragma once

#include "noncopyable.h"

#include <pthread.h>
#include <cstdio>
	
class MutexLock : noncopyable 
{
public:
	MutexLock() 
	{ 
		pthread_mutex_init(&mutex, nullptr); 
	}
	~MutexLock() 
	{
		pthread_mutex_lock(&mutex);
		pthread_mutex_destroy(&mutex);
	}

	void lock() { pthread_mutex_lock(&mutex); }
	void unlock() { pthread_mutex_unlock(&mutex); }
	pthread_mutex_t *get() { return &mutex; }

private:
	pthread_mutex_t mutex;

	// 注意：友元不受访问权限影响
private:
	friend class Condition;
};

// 利用RAII思想，把mutex包装为类对象
class MutexLockGuard : noncopyable 
{
public:
	explicit MutexLockGuard(MutexLock&_mutex) 
		: mutex(_mutex) 
	{	mutex.lock(); }

	~MutexLockGuard() 
	{	mutex.unlock(); }

private:
	MutexLock &mutex;
};