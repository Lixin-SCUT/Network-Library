//
// Created by 黎鑫 on 2020/4/16.
//


#include <sys/time.h>
#include <pthread.h>
#include "Logging.h"
#include "AsyncLogging.h"

static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogging* async_logger_;

std::string Logger::logfilename_ = "./NetworkLibrary.log";
void* ThreadFunc(void *arg);

void once_init()
{
    async_logger_ = new AsyncLogging(Logger::GetLogFileName());
    pthread_create(async_logger_->GetPid(), nullptr, ThreadFunc, nullptr);
}

void output(const char* buffer, size_t len)
{
    pthread_once(&once_control_, once_init);
    async_logger_->Append(buffer, len);
}

Logger::Logger(const char *filename, int line)
    : impl_(filename, line)
{}

Logger::~Logger()
{
    impl_.stream_ << " -- " << impl_.filename_ << ":" << impl_.line_ << '\n' ;
    const FixedBuffer<kLineBufSize>& buf(stream().Buffer());
    output(buf.Buffer(), buf.Length());
}

Logger::Impl::Impl(const char *filename, int line)
    : stream_(),
      filename_(filename),
      line_(line)
{
    FormatTime();
}

void Logger::Impl::FormatTime()
{
    struct timeval tv;
    gettimeofday (&tv, nullptr);

    time_t time = tv.tv_sec;
    struct tm* p_time = localtime(&time);

    char str_t[26] = {0};
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    stream_ << str_t;
}



void* ThreadFunc(void *arg)
{
    async_logger_->LogLoop();
}