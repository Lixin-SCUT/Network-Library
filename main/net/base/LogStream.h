// LogStream.h
// Created by Lixin on 2020.03.08
// LogStream主要用来格式化输出，重载了<<运算符
// 同时也有一块缓冲区，负责将多个<<的结果连成一块

#pragma once

#include <assert.h>
#include <string.h>
#include <string>

#include "noncopyable.h"

class AsyncLogging;
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;
	
template <int SIZE>
class FixedBuffer : noncopyable 
{
public:
	FixedBuffer() 
		: cur_(data_) 
	{ }
	~FixedBuffer() 
	{ }

	void append(const char* buf, size_t len) 
	{
		if (avail() > static_cast<int>(len)) 
		{
			memcpy(cur_, buf, len);
			cur_ += len;
		}
	}

	const char* data() const { return data_; }
	int length() const { return static_cast<int>(cur_ - data_); }

	char* current() { return cur_; }
	int avail() const { return static_cast<int>(end() - cur_); }
	void add(size_t len) { cur_ += len; }

	void reset() { cur_ = data_; }
	void bzero() { memset(data_, 0, sizeof(data_)); }

private:
	const char* end() const { return data_ + sizeof(data_); }

	char data_[SIZE];
	char* cur_;
};

class LogStream : noncopyable 
{
public:
	typedef FixedBuffer<kSmallBuffer> Buffer;

	LogStream& operator<<(bool v) 
	{
		buffer_.append(v ? "1" : "0", 1);
		return *this;
	}

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

	LogStream& operator<<(float f) 
	{
		*this << static_cast<double>(f);
		return *this;
	}

	LogStream& operator<<(char v) 
	{
		buffer_.append(&v, 1);
		return *this;
	}

	LogStream& operator<<(const char* str) 
	{
		if (str)
		{
			buffer_.append(str, strlen(str));
		}
		else
		{
			buffer_.append("(null)", 6);
		}
		return *this;
	}

	LogStream& operator<<(const void*)
	{
		*this << static_cast<const char*>(v);
		return *this;
	}

	LogStream& operator<<(const unsigned char* str) 
	{
		return operator<<(reinterpret_cast<const char*>(str));
	}

	LogStream& operator<<(const std::string& v) 
	{
		buffer_.append(v.c_str(), v.size());
		return *this;
	}

	void append(const char* data, int len) { buffer_.append(data, len); }
	const Buffer& buffer() const { return buffer_; }
	void resetBuffer() { buffer_.reset(); }

private:
	template <typename T>
	void formatInteger(T);

private:
	Buffer buffer_;

	static const int kMaxNumericSize = 32;
};