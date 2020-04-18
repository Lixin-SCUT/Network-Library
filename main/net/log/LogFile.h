//
// Created by 黎鑫 on 2020/4/16.
//

#ifndef MYPROJECT_LOGFILE_H
#define MYPROJECT_LOGFILE_H

#include "../base/noncopyable.h"
#include "FileUtil.h"
#include "../base/MutexLock.h"

#include <memory>


#define FLUSHTIMES 64*1024

class LogFile : noncopyable
{
public:
    LogFile(const char* filename);
    ~LogFile();

    void Append(const char* logline, size_t len);
    void Flush();

private:
    std::unique_ptr<AppendFile> file_ptr;
    int append_count_;
    std::unique_ptr<MutexLock> mutex_;
};

#endif //MYPROJECT_LOGFILE_H
