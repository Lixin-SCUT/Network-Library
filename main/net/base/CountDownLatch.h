// CountDownLatch.h
// Created by Lixin on 2020.03.07
// 一个同步计数器，底层为mutex和condition
// 通过设置while的条件达到计数器的功能。

#pragma once

#include "Condition.h"
#include "MutexLock.h"
#include "noncopyable.h"

// CountDownLatch的主要作用是确保Thread中传进去的func真的启动了以后
// 外层的start才返回
class CountDownLatch : noncopyable 
{
public:
	explicit CountDownLatch(int count);
	void wait();
	void countDown();

private:
	mutable MutexLock mutex_;
	Condition condition_;
	int count_;
};
