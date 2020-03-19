// AsyncLogging.h
// Created by Lixin on 2020.03.02
// AsyncLogging启动一个log线程，专门用来将log写入LogFile
// 应用了双buffer，写满后或到时后将缓冲区中的数据写入LogFile中。

#pragma once

#include "CountDownLatch.h"
#include "LogStream.h"
#include "MutexLock.h"
#include "Thread.h"
#include "noncopyable.h"

#include <functional>
#include <string>
#include <vector>
	
class AsyncLogging : noncopyable {
public:
	AsyncLogging(const std::string basename, int flushInterval = 2);
	~AsyncLogging() 
	{
		if (running_) 
		{	stop(); }
	}
	void append(const char* logline, int len);

	void start() 
	{
		running_ = true;
		thread_.start();
		latch_.wait();
	}

	void stop() 
	{
		running_ = false;
		cond_.notify();
		thread_.join();
	}

private:
	void threadFunc();

private:
	typedef FixedBuffer<kLargeBuffer> Buffer;
	typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
	typedef std::shared_ptr<Buffer> BufferPtr;

	const int flushInterval_; // 定时写入，保证最多只会丢失flushInterval_秒的信息
	bool running_;
	std::string basename_;
	Thread thread_;
	MutexLock mutex_;
	Condition cond_;
	BufferPtr currentBuffer_;
	BufferPtr nextBuffer_; 
	BufferVector buffers_;
	CountDownLatch latch_;
};