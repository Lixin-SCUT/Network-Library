// Thread.h
// Created by Lixin on 2020.03.07
// 新建线程，同时供EventLoopThread和日志线程AsyncLogging使用

#pragma once

#include "CountDownLatch.h"
#include "noncopyable.h"
	
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <functional>
#include <memory>
#include <string>

class Thread : noncopyable 
{
public:
	typedef std::function<void()> ThreadFunc;
	explicit Thread(const ThreadFunc&, const std::string& name = std::string());
	~Thread();
	void start();
	int join();
	bool started() const { return started_; }
	pid_t tid() const { return tid_; }
	const std::string& name() const { return name_; }

private:
	void setDefaultName();

private:
	bool started_;
	bool joined_;
	pthread_t pthreadId_;
	pid_t tid_;
	ThreadFunc func_;
	std::string name_;
	CountDownLatch latch_;
};