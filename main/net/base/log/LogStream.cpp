//
// Created by 黎鑫 on 2020/4/16.
//


#include "LogStream.h"

const size_t  kMAXSIZE = 32;

const char nums[] = "9876543210123456789";
const char* zero = nums + 9;

template <typename T>
size_t itoa(char* buf, T value)
{
    T temp;
    char* cur = buf;

    do{
        temp = value % 10;
        value /= 10;
        *cur = zero[temp];
        ++cur;
    }while(0 != value);

    if(value < 0)
    {
        *cur = '-';
        ++cur;
    }

    std::reverse(buf, cur);
    *cur = '\0';

    return cur - buf;
}

template <typename T>
void LogStream::FormatInteger(T value)
{
    if(buf_.Avail() > kMAXSIZE)
    {
        size_t n = itoa(buf_.CurLoc(), value);
        buf_.AddLen(n);
    }
}


LogStream& LogStream::operator<<(bool value)
{
    buf_.Append((value ? "1" : "0") , 1);
    return *this;
}



LogStream& LogStream::operator<<(short value)
{
    *this << static_cast<int>(value);
    return *this;
}
LogStream& LogStream::operator<<(unsigned short value)
{
    *this << static_cast<int>(value);
    return *this;
}

LogStream& LogStream::operator<<(int value)
{
    FormatInteger(value);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int value)
{
    FormatInteger(value);
    return *this;
}
LogStream& LogStream::operator<<(long value)
{
    FormatInteger(value);
    return *this;
}
LogStream& LogStream::operator<<(unsigned long value)
{
    FormatInteger(value);
    return *this;
}

LogStream& LogStream::operator<<(long long value)
{
    FormatInteger(value);
    return *this;
}
LogStream& LogStream::operator<<(unsigned long long value)
{
    FormatInteger(value);
    return *this;
}

LogStream& LogStream::operator<<(double value)
{
    if (buf_.Avail() >= kMAXSIZE) {
        int n = snprintf(buf_.CurLoc(), kMAXSIZE, "%.12g", value);
        buf_.AddLen(n);
    }
    return *this;
}
LogStream& LogStream::operator<<(long double value)
{
    if (buf_.Avail() >= kMAXSIZE) {
        int n = snprintf(buf_.CurLoc(), kMAXSIZE, "%.12g", value);
        buf_.AddLen(n);
    }
    return *this;
}
LogStream& LogStream::operator<<(float value)
{
    *this << static_cast<double>(value);
    return *this;
}

LogStream& LogStream::operator<<(char value)
{
    buf_.Append(&value, 1);
    return *this;
}

LogStream& LogStream::operator<<(const char* value)
{
    if(value == nullptr)
    {
        buf_.Append("(null)", 6);
    }
    else
    {
        buf_.Append(value, strlen(value));
    }
    return *this;
}

LogStream& LogStream::operator<<(const void* value)
{
    operator<<(reinterpret_cast<const char*>(value));
}



LogStream& LogStream::operator<<(const unsigned char* value)
{
    operator<<(reinterpret_cast<const char*>(value));
}

LogStream& LogStream::operator<<(const std::string& value)
{
    buf_.Append(value.c_str(),value.size());
    return *this;
}
