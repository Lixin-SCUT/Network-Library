// Condition.h
// Created by Lixin on 2020.03.06

#pragma once

#include "main/base/Mutex.h"
#include "main/base/noncopyable.h"

#include <pthread.h>

namespace main
{

class Condition : noncopyable
{
public:
	Condition(MutexLock &mutex)
		:	mutex_(mutex)
	{	pthread_cond_init(&cond_, nullptr); }

	~Condtion()
	{	pthread_cond_destroy(&cond_); }

	void wait()
	{	pthread_cond_wait(&cond_, mutex.get()); }

	void signal()
	{	pthread_cond_signal(&cond_); }

	void broadcast()
	{	pthread_cond_broadcast(&cond_); }

	void waitForSeconds(int seconds)
	{
		struct timspec abstime;
		clock_gettime(CLOCK_REALTIME, &abstime);
  		abstime.tv_sec += static_cast<time_t>(seconds); 

  		return ETIMEDOUT == pthread_cond_timedwait(&cond_, 
													mutex_.get(), 
													&abstime);
	}
private:
	MutexLock &mutex_;
	pthread_cond_t cond_;
}
} // namespace main
