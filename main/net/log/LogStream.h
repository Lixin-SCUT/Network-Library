//
// Created by 黎鑫 on 2020/4/16.
//

#ifndef MYPROJECT_LOGSTREAM_H
#define MYPROJECT_LOGSTREAM_H

#include <cstring>
#include <string>
#include "../base/noncopyable.h"

const size_t kLineBufSize = 4000;

template <unsigned SIZE>
class FixedBuffer : noncopyable
{
public:
    FixedBuffer() : cur_loc_(buf_) {}
    ~FixedBuffer() = default;

    void Append(const char* logline, size_t len)
    {
        if(len < Avail())
        {
            memcpy(cur_loc_, logline, len);
            cur_loc_ += len;
        }
    }

    size_t Avail() { return SIZE - Length(); }
    size_t Length() const { return cur_loc_ - buf_; }

    void AddLen(size_t n) { cur_loc_ += n; }

    char* CurLoc() const { return cur_loc_; }
    const char* Buffer() const { return buf_; }

private:

    char buf_[SIZE];
    char* cur_loc_;
};

class LogStream : noncopyable
{
public:
    LogStream& operator<<(bool value);

    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);

    LogStream& operator<<(double);
    LogStream& operator<<(long double);
    LogStream& operator<<(float);

    LogStream& operator<<(char v);
    LogStream& operator<<(const char* str);
    LogStream& operator<<(const void*);
    LogStream& operator<<(const unsigned char* str);
    LogStream& operator<<(const std::string& v);

    FixedBuffer<kLineBufSize>& Buffer() { return buf_; };

private:
    template <typename T>
    void FormatInteger(T);

private:

    FixedBuffer<kLineBufSize> buf_;
};

#endif //MYPROJECT_LOGSTREAM_H
