//
// Created by 黎鑫 on 2020/4/16.
//

#ifndef MYPROJECT_LOGGING_H
#define MYPROJECT_LOGGING_H

#include "LogStream.h"

class Logger :noncopyable
{
public:
    Logger(const char* filename, int line);
    ~Logger();
    LogStream& stream() { return impl_.stream_; }

    static void SetLogFileName(std::string logfilename) { logfilename_ = logfilename; }
    static std::string GetLogFileName() { return logfilename_; }

private:
    class Impl
    {
    public:
        Impl(const char *filename, int line);
        void FormatTime();

        LogStream stream_;
        std::string filename_;
        int line_;
    };

private:
    Impl impl_;
    static std::string logfilename_;

};

#define LOG Logger(__FILE__, __LINE__).stream()

#endif //MYPROJECT_LOGGING_H
