// CountDownLatch.h
// Created by Lixin on 2020.03.07

#pragma once

#include "Condition.h"
#include "Mutex.h"
#include "noncopyable.h"

// CountDownLatch的主要作用是确保Thread中传进去的func真的启动了以后
// 外层的start才返回
namespace main
{

class CountDownLatch : noncopyable
{
public:
	explicit CountDownLatch(int count);
	void wait();
	void CountDown();
	int getCount() const;
private:
	mutable MutexLock mutex_;
	Condition condition_;
	int count_;
}

} // namespace main
