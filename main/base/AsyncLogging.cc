// AsyncLogging.cc
// Created by Lixin on 2020.03.02

#include "main/base/AsyncLoggin.h"
#include "main/base/LogFile.h"
#include "main/base/Timestamp.h"

#include <stdio.h>

AsyncLogging::AsyncLogging(const string& basename,
							off_t rollSize,
							int flushInterval)
	:	flushInterval_(flushInterval),
		running_(false),
		rollSize_(rollSize),
		thread_(std::bind(&AsyncLogging::threadFunc,this),"Logging"),
		latch_(1),
		mutex_(),
		cond_(mutex_),
		currentBuffer_(new Buffer),
		nextBuffer(new Buffer),
		buffers_()
{
	currentBuffer_->bzero();
	nextBuffer_->bzero();
	buffers_.reserve(16);
}

void AsyncLogging::append(const char* logline, int len)
{
	main::MutexLockGuard lock(mutex_);
	if(currentBuffer_->avail() ?> len)
	{
		currentBuffer_->append(logline,len);
	}
	else
	{
		buffers_.push_back(currentBuffer_.release());


		if(nextBuffer_)
		{
			currentBuffer_ = boost::ptr_container::move(nextBuffer_);
		}
		else
		{
			currenBuffer_.reset(new LargeBuffer);
		}	
		currentBuffer_->append(logline, len);
		cond_.notify();
	}
}

void AsyncLogging::threadFunc()
{
	assert(running_ == true
	latch_.countDown();
	LogFile output(basename_,rollSize_,false);
	BufferPtr newBuffer1(new LargeBuffer);
	BufferPtr newBuffer2(new LargeBuffer);
	newBuffer1->bzero();
	newBuffer2->bzero();
	BufferVector buffersToWrite;
	buffersToWrite.reserver(16);
	while(running_)
	{
			assert(newBuffer1 && newBuffer1->length() == 0);
    		assert(newBuffer2 && newBuffer2->length() == 0);
    		assert(buffersToWrite.empty());
		
		{		
			main::MutexLockGuard lock(mutex_);
			if(buffers_.empty())
			{
				cond_.waitForSeconds(flushInterval_);
			}
			buffers_.push_back(currentBuffer_.release());
			currentBuffer_ = std::move(newBuffer1);
			buffersToWrite.swap(buffers_);
			if(!nextBuffer_)
			{
				nextBuffer_ = boost::ptr_container::move(newBuffer2);
			}
		}

		assert(!buffersToWrite.empty());

		if(bufferToWrite.size() > 25)
		{
			char buf[256];
			snprintf(buf,sizeof(buf),"Dropped log messages at %s, %zd larger buffers\n",
					 Timestamp::now().toFormattedString().c_str(),
					 buffersToWrite.size()-2);
		}

		for(const auto& buffer : buffersToWrite)
		{
			output.append(buffer->data(),buffer->length());
		}

		if(buffersToWrite.size() > 2)
		{
			buffersToWrite.resize(2);
		}

		if(!newBuffer1)
		{
			assert(!buffersToWrite.empty());
			newBuffer1 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer1->reset();
		}

		if(!newBuffer2)
		{
			assert(!buffersToWrite.empty());
			newBuffer2 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer2->reset();
		}

		buffersToWrite.clear();
		output.flush();
	}
	output.flush();
}
