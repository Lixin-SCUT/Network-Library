// FileUtil.h
// Created by Lixin on 2020.03.08

#pragma once

#include "main/base/noncopyable.h"
// #include "main/base/StringPiece.h"

#include <sys/types.h>

namespace main
{
namespace FileUtil
{

// read small file < 64KB
class ReadSmallFile : noncopyable
{
public:
	ReadSmallFile(string filename);
	~ReadSmallFile();

	// return errno
	template<typename String>
	int readToString(int maxSize,
					 String* content,
					 int64_t* fileSize,
					 int64_t* modifyTime,
					 int64_t* createTime);
	
	// Read maximum kBufferSize into buf_
  	// return errno
	int readToBuffer(int* size);
	
	const char* buffer() const 
	{	return buf_; }
	
	static const int kBufferSize = 64*1024;
	
private:
	int fd_;
	int err_;
	char buf_[kBufferSize];
};

template<typename String>
int readFile(string filename,
			 int maxSize,
			 String* content,
			 int64_t* fileSize = nullptr,
			 int64_t* modifyTime = nullptr,
			 int64_t* createTime = nullptr)
{
	ReadSmallFile file(filename);
	return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}

// not thread safe
class AppendFile : noncopyable
{
public:
	explicit AppendFile(string filename);

	~AppendFile();

	void append(const char* logline, size_t len);

	void flush();

	off_t writtenBytes() const 
	{	return writtenBytes_; }

private:
	size_t write(const char* logline, size_t len);

	FILE* fp_;
	char buffer_[64*1024];
	off_t writtenBytes_;
};

} // namespace FileUtil
} // namespace main
