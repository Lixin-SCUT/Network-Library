// FileUtil.h
// Created by Lixin on 2020.03.08

#pragma once

#include "noncopyable.h"
	
#include <string>

class AppendFile : noncopyable {
public:
	explicit AppendFile(std::string filename);
	~AppendFile();
	// append 写文件文件
	void append(const char *logline, const size_t len);
	void flush();

private:
	size_t write(const char *logline, size_t len);
	FILE *fp_;
	char buffer_[64 * 1024];
};