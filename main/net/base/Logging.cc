// Logging.cc
// Created by Lixin on 2020.03.08

#include "Logging.h"

#include "CurrentThread.h"
#include "Thread.h"
#include "AsyncLogging.h"

#include <assert.h>
#include <iostream>
#include <time.h>  
#include <sys/time.h> 

	
static pthread_once_t once_control_ = PTHREAD_ONCE_INIT; // 配合pthread_once，达到执行一次的效果
static AsyncLogging *AsyncLogger_; 

std::string Logger::logFileName_ = "./NetWork-Library.log"; // 输出路径与写入文件

void once_init() // TODO 改为单例模式，只允许存在一个实例
{
	AsyncLogger_ = new AsyncLogging(Logger::getLogFileName());
	AsyncLogger_->start(); 
}

// 第一次LOG就会触发pthread_once，创建线程
void output(const char* msg, int len) 
{
	pthread_once(&once_control_, once_init);
	AsyncLogger_->append(msg, len);
}

Logger::Impl::Impl(const char *fileName, int line)
	: stream_(),
	  line_(line),
	  basename_(fileName)
{
	formatTime();
}

void Logger::Impl::formatTime() // 一定不能漏掉两个作用域限定符
{
	struct timeval tv;
	time_t time;
	char str_t[26] = {0};
	gettimeofday (&tv, nullptr);
	time = tv.tv_sec;
	struct tm* p_time = localtime(&time);   
	strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
	stream_ << str_t;
}

Logger::Logger(const char *fileName, int line)
	: impl_(fileName, line)
{ }

Logger::~Logger()
{
	// 每次LOG都会在末尾加上文件名和代码行号
	impl_.stream_ << " -- " << impl_.basename_ << ':' << impl_.line_ << '\n'; // 注意不要漏掉回车符，否则连成一片
	const LogStream::Buffer& buf(stream().buffer());
	output(buf.data(), buf.length()); // 输出到LogStream的buffer中
}