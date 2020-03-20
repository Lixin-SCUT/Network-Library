// LogStream.cc
// Created by Lixin on 2020.03.08

#include "LogStream.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <limits>

const char digits[] = "9876543210123456789"; // 前面9～1是为了应对负数
const char* zero = digits + 9;
	
// 将数字转化为字符（itoa） 并返回size
template <typename T>
size_t convert(char buf[], T value) 
{
	T i = value;
	char* ptr = buf;

	do 
	{
		int lsd = static_cast<int>(i % 10);
		i /= 10;
		*ptr++ = zero[lsd];
	} while (i != 0);

	if (value < 0) 
	{
		*ptr++ = '-';
	}
	*ptr = '\0';
	std::reverse(buf, ptr);

	return ptr - buf; // 注意为什么不需要+1
}

template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;

template <typename T>
void LogStream::formatInteger(T v) 
{
	// buffer容不下kMaxNumericSize个字符的话会被直接丢弃
	if (buffer_.avail() >= kMaxNumericSize) 
	{
		size_t len = convert(buffer_.current(), v);
		buffer_.add(len);
	}
}

LogStream& LogStream::operator<<(short v) 
{
	*this << static_cast<int>(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned short v) 
{
	*this << static_cast<unsigned int>(v);
	return *this;
}

LogStream& LogStream::operator<<(int v) 
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned int v) 
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long v) 
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long v) 
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long long v) 
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long long v) 
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(double v) 
{
	if (buffer_.avail() >= kMaxNumericSize) 
	{
		int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
		buffer_.add(len);
	}
	return *this;
}

LogStream& LogStream::operator<<(long double v) 
{
	if (buffer_.avail() >= kMaxNumericSize) 
	{
		int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12Lg", v);
		buffer_.add(len);
	}
	return *this;
}