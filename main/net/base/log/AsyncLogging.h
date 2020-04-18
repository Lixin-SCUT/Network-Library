//
// Created by 黎鑫 on 2020/4/17.
//

#ifndef MYPROJECT_ASYNCLOGGING_H
#define MYPROJECT_ASYNCLOGGING_H

#include <cstring>
#include <memory>
#include <vector>
#include "../base/noncopyable.h"
#include "LogStream.h"
#include "../base/MutexLock.h"
#include "../base/Condition.h"

const size_t kBufSize = 4000 * 1000;
const int kFlushPeriod = 2;



class AsyncLogging : noncopyable
{
    typedef  FixedBuffer<kBufSize> Buffer;
public:
    AsyncLogging(std::string filename);
    ~AsyncLogging();

    void Append(const char* logline, int len);

    pthread_t* GetPid() { return &pthread_id; }

    std::string GetFileName() { return filename_; }

    void LogLoop();

private:
    std::string filename_;
    bool state_;

    std::shared_ptr<Buffer> current_buf_;
    std::shared_ptr<Buffer> next_buf_;
    std::vector<std::shared_ptr<Buffer>> buf_vec_;
    MutexLock mutex_;
    Condtion cond_;

    pthread_t pthread_id;
};

#endif //MYPROJECT_ASYNCLOGGING_H
