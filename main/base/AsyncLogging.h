// AsyncLogging.h
// Created by Lixin on 2020.03.02
#ifndef MAIN_BASE_ASYNCLOGGING_H
#define MAIN_BASE_ASYNCLOGGING_H

#include "main/base/BlockingQueue.h"
#include "main/base/BoundedBlockingQueue.h"
#include "main/base/CountDownLatch.h"
#include "main/base/Mutex.h"
#include "main/base/Thread.h"
#include "main/base/LogStream.h"

#include <atomic>
#include <vector>

namespace main
{

class AsyncLoggin : noncopyable
{
public:

	AsyncLoggin(const string& basename,
				off_t rollSize,
				int flushInterval = 3);

	~AsyncLoggin()
	{
		if(running_)
		{
			stop();
		}
	}

	void append(const char* logline, int len);
	
	void start()
	{
		running_ = true;
		thread_.start();
		latch_.wait();
	}

	void stop() NO_THREAD_SAFETY_ANALYSIS
	{
		running_ = false;
		cond_.notify();
		thread_.join();
	}

private:

	void threadFunc();
	
	typedef main::detail::FixedBuffer<main::detail::kLargeBuffer> Buffer;
	typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
	typedef BufferVector::value_type BufferPtr;

	const int flushInterval_;
	std::atomic<bool> running;
	const off_t rollSize_;
	main::Thread thread_;
	main::CountDownLatch latch_;
	main::MutexLock mutex_;

	main::Condition cond_ GUAEDED_BY(mutex_);
	BufferPtr currentBuffer_ GUAEDED_BY(mutex_);
	BufferPtr nextBuffer_ GUAEDED_BY(mutex_);
	BufferVector buffers_ GUAEDED_BY(mutex_);
};

} // namespace main
#endif // MAIN_BASE_ASYNCLOGGING_H
