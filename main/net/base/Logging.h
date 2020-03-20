// Logging.h
// Created by Lixin on 2020.03.08
// Logging是对外接口，主题还是AsyncLogging
// Logging类包含一个Impl对象个LogStream对象，每次构造都会加上固定的格式化的信息

#pragma once

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "LogStream.h"

class AsyncLogging;
	
class Logger {
public:
	Logger(const char *fileName, int line);
	~Logger();
	LogStream& stream() { return impl_.stream_; }
	
	static void setLogFileName(std::string fileName) { logFileName_ = fileName; }
	static std::string getLogFileName() { return logFileName_; }

private:
	class Impl 
	{
	public:
		Impl(const char *fileName, int line);
		void formatTime();

		LogStream stream_;
		int line_;
		std::string basename_;
	};
	Impl impl_;
	static std::string logFileName_;
};

// __LINE__：在源代码中插入当前源代码行号；
// __FILE__：在源文件中插入当前源文件名；
// 注意每次LOG都会新建logger对象
#define LOG Logger(__FILE__, __LINE__).stream()