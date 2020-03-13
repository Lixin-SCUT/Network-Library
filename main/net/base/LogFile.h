// LogFile.h
// Created by Lixin on 2020.03.08

#pragma once

#include "FileUtil.h"
#include "MutexLock.h"
#include "noncopyable.h"

#include <memory>
#include <string>
	
// TODO 提供自动归档功能
class LogFile : noncopyable 
{
public:
	// 每当执行flushEveryN次的添加操作，flush会往文件写，同时文件也是带缓冲区的
	LogFile(const std::string& basename, int flushEveryN = 1024);
	~LogFile();

	void append(const char* logline, int len);
	void flush();
	bool rollFile();

private:
	void append_unlocked(const char* logline, int len);

private:
	const std::string basename_;
	const int flushEveryN_;

	int count_;
	std::unique_ptr<MutexLock> mutex_;
	std::unique_ptr<AppendFile> file_;
};