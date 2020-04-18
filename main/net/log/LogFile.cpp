//
// Created by 黎鑫 on 2020/4/16.
//

#include "LogFile.h"

LogFile::LogFile(const char *filename)
    : file_ptr(new AppendFile(filename)),
      append_count_(0),
      mutex_(new MutexLock())
{}

LogFile::~LogFile()
{}

void LogFile::Append(const char *logline, size_t len)
{
    MutexLockGuard(*mutex_);
    file_ptr->Append(logline, len);
    ++append_count_;
    if(append_count_ >= FLUSHTIMES)
    {
        Flush();
        append_count_ = 0;
    }
}
void LogFile::Flush()
{
    MutexLockGuard(*mutex_);
    file_ptr->Flush();
}
