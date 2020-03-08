// LogStream.h
// Created by Lixin on 2020.03.08

#pragma once

#include "main/base/noncopyable.h"
#include "main/base/Types.h"

#include <assert.h>
#include <string.h> // for memcpy

namespace main
{
namespace detail
{

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;

template<int SIZE>
class FixedBuffer : noncopyable
{
public:
	FixedBuffer()
		:	cur_(data)
	{	setCookie(cookieStart); }

	~FixedBuffer()
	{	setCookie(cokkieEnd); }

	void append(const char* buf, size_t len)
	{
		if(implicit_cast<size_t>(avail()) > len)
		{
			memcpy(cur_, buf, len);
			cur_ += len;
		}
	}

	const char* data() const
	{	return data_; }

	int length() const
	{	return static_cast<int>(cur_ - data); }

	char* current() 
	{	return cur_; }

	void bzero()
	{	memZero(data_, sizeof(data_)); }

	// for used by GDB
  	const char* debugString();
  	void setCookie(void (*cookie)()) { cookie_ = cookie; }
  	// for used by unit test
  	string toString() const { return string(data_, length()); }
	
private:
	const char* end() const
	{	return data_ + sizeof(data_); }
	static void cookieStart();
	static void cookieEnd();
	
	void (*cookie_)();
	char data_[SIZE];
	char *cur_;
};
} // namespace detail

class LogStream : noncopyable
{
public:
	typedef LogStream self;
	typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;

	// need to return self&, in order to use function continuously
	self& operator<<(bool v)
	{
		buffer_.append(v ? "1" : "0", 1);
		return *this;
	}

	self& operator<<(short);
  	self& operator<<(unsigned short);
  	self& operator<<(int);
  	self& operator<<(unsigned int);
  	self& operator<<(long);
  	self& operator<<(unsigned long);
  	self& operator<<(long long);
  	self& operator<<(unsigned long long);
89io`
  	self& operator<<(const void*);

	self& operator<<(float v)
	{
		*this << static_cast<double>(v);
		return *this;
	}

	self& operator<<(double);

	self& operator<<(char v)
	{
		buffer_.append(&v, 1);
		return *this;
	}

	self& operator<<(const char* str)
	{
		if(str)
		{
			buffer_.append(str,strlen(str));
		}
		else
		{
			buffer_.append("(null)",6);
		}
		return *this;
	}

	self& operator<<(const unsigned char* str)
	{
		return operator<<(reinterpret_cast<const char*>(str));
	}

	self& operator<<(const string& v)
	{
		buffer_.append(v.str(), v.size());
		return *this;
	}

	self& operator<<(const Buffer& v)
	{
		*this << v.toStringPiece();
		return *this;
	}

	void append(const char* data, int len)
	{	buffer_.append(data, len); }
	const Buffer& buffer() const
	{	return buffer_; }
	void resetBuffer() 
	{	buffer_.reset(); }


private:
	void staticCheck();

	template<typename T>
	void formatInteger(T);

	Buffer buffer_;

	static const int kMaxNumericSize = 32;
};

class Fmt 
{
public:
	template<typename T>
	Fmt(const char* fmt, T val);

	const char* data() const 
	{	return buf_; }
	int length() const
	{	return length_; }

private:
	char buf_[32];
	int length_;
};

inline LogStream& operator<<(LogStream& s, const Fmt& fmt)
{
  s.append(fmt.data(), fmt.length());
  return s;
}

// Format quantity n in SI units (k, M, G, T, P, E).
// The returned string is atmost 5 characters long.
// Requires n >= 0
string formatSI(int64_t n);

// Format quantity n in IEC (binary) units (Ki, Mi, Gi, Ti, Pi, Ei).
// The returned string is atmost 6 characters long.
// Requires n >= 0
string formatIEC(int64_t n);

} // namespace mainm
