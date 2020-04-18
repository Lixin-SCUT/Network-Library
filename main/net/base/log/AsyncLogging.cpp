//
// Created by 黎鑫 on 2020/4/17.
//

#include "AsyncLogging.h"
#include "LogFile.h"

AsyncLogging::AsyncLogging(std::string filename)
    : filename_(filename),
      state_(false),
      current_buf_(new Buffer),
      next_buf_(new Buffer),
      buf_vec_(),
      mutex_(),
      cond_(mutex_)
{
    buf_vec_.reserve(16);

}

AsyncLogging::~AsyncLogging()
{
    if(state_)
    {
        state_ = false;
        pthread_join(pthread_id, nullptr);
    }
}

void AsyncLogging::Append(const char *logline, int len)
{
    MutexLockGuard lock(mutex_);
    if(current_buf_->Avail() < len)
    {
        buf_vec_.push_back(current_buf_);
        if(next_buf_ != nullptr)
        {
            current_buf_.reset(next_buf_.get());
            next_buf_.reset();
        }
        else
        {
            current_buf_.reset(new Buffer);
        }
        cond_.Notify();
    }
    current_buf_->Append(logline, len);
}

void AsyncLogging::LogLoop()
{
    LogFile log_file_(filename_.c_str());
    std::shared_ptr<Buffer> new_buffer_first (new Buffer);
    std::shared_ptr<Buffer> new_buffer_second (new Buffer);
    std::vector<std::shared_ptr<Buffer>> buffer_to_write;

    while(state_ = true)
    {
        {
            MutexLockGuard lock(mutex_);
            while (buf_vec_.empty())
            {
                cond_.TimedWait(kFlushPeriod);
            }

            buf_vec_.push_back(current_buf_);
            current_buf_.reset(new_buffer_first.get());
            new_buffer_first.reset();

            if(new_buffer_first == nullptr)
            {
                next_buf_.reset(new_buffer_second.get());
                new_buffer_second.reset();
            }

            for(int i = 0; i < buf_vec_.size(); ++i)
            {
                buffer_to_write.push_back(buf_vec_[i]);
                buf_vec_[i].reset();
            }
            buf_vec_.clear();
        }

        if(buffer_to_write.size() > 4)
        {
            buffer_to_write.resize(4);
            buffer_to_write.shrink_to_fit();
        }

        for(int i = 0; i < buffer_to_write.size(); ++i)
        {
            log_file_.Append(buffer_to_write[i]->Buffer(), buffer_to_write[i]->Length());
        }

        if(new_buffer_first == nullptr)
        {
            new_buffer_first = buffer_to_write.back();
            buffer_to_write.pop_back();
        }

        if(new_buffer_second == nullptr)
        {
            new_buffer_second = buffer_to_write.back();
            buffer_to_write.pop_back();
        }

        buffer_to_write.clear();
        log_file_.Flush();
    }

    log_file_.Flush();

}
