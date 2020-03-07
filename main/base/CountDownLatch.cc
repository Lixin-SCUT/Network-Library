// CountDownLatch.cc
// Created by Lixin on 2020.03.07

#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count)
	:	mutex_(),
		condition_(mutex_),
		count_(count)
{ }

void CountDownLatchL::wait()
{
	MutexLockGuard lock(mutex_);
	while(count_ > 0) 
	{
		condition_.wait();
	}
}

void CountDownLatch::countDown()
{
	MutexLockGuard lock(mutex_);
	--count;
	if(count_ == 0) 
	{
		condition_.notifyAll();
	}
}

int CountDownLatch::getCount() const
{
	MutexLockGuard lock(mutex_);
	return count_;
}

