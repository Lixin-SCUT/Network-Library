// LogFile.h
// Created by Lixin on 2020.03.08

#pragma once 

#include "main/base/Mutex.h"
#include "main/base/Types.h"
#include "main/base/noncopyable.h"

#include <memory>

namespace main
{
namespace FileUtil
{
class AppendFile;
} // namespace FileUtil

class LogFile : noncopyable
{
public:
	LogFile(const string &basename,
			off_t rollSize,
			bool threadSafe = true,
			int flushInterval = 3,
			int checkEveryN = 1024);
	~LogFile();

	void append(const char* logline, int len);
	void flush();
	bool rollFile();

private:
	void append_unlocked(const char* logline, int len);

	static string getLogFileName(const string &basename,
								 time_t *now);

private:
	const string basename_;
	const off_t rollSize_;
	const int flushInterval_;
	const int checkEveryN_;

	int count;

	std::unique_ptr<MutexLock> mutex_;
	time_t startOfPeriod_;
	time_t lastRoll_;
	time_t lastFlush_;
	std::unique_ptr<FileUtil::AppendFile> file_;

	const static int kRollPerSeconds_ = 60*60*24;
};

} // namespace main

