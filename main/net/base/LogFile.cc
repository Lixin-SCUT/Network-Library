// LogFile.cc
// Created by Lixin on 2020.03.08

#include "LogFile.h"

#include "FileUtil.h"

#include <assert.h>
#include <stdio.h>
#include <time.h>

using namespace std; // TODO delete
	
LogFile::LogFile(const string& basename, int flushEveryN)
	: basename_(basename),
	  flushEveryN_(flushEveryN),
	  count_(0),
	  mutex_(new MutexLock) 
{
	// assert(basename.find('/') >= 0);
	file_.reset(new AppendFile(basename)); // AppendFile负责打开文件并设置缓冲区
}

LogFile::~LogFile() 
{ }

void LogFile::append(const char* logline, int len) 
{
	MutexLockGuard lock(*mutex_);
	append_unlocked(logline, len);
}

void LogFile::flush() 
{
	MutexLockGuard lock(*mutex_);
	file_->flush();
}

void LogFile::append_unlocked(const char* logline, int len) 
{
	file_->append(logline, len);
	++count_;
	if (count_ >= flushEveryN_) 
	{
		count_ = 0;
		file_->flush(); // 满足写入次数后将buffer直接flush到file中
	}
}