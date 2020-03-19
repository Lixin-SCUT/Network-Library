// FileUtil.h
// Created by Lixin on 2020.03.08
// FileUtil是日志哭最底层的文件类，封装了Log文件的打开、写入并在类析构的时候关闭文件
// 底层使用了标准I/O写入文件

#pragma once

#include "noncopyable.h"
	
#include <string>

class AppendFile : noncopyable {
public:
	explicit AppendFile(std::string filename);
	~AppendFile();
	// append首先写入buffer，然后再flush写入文件
	void append(const char *logline, const size_t len);
	void flush();

private:
	size_t write(const char *logline, size_t len);
	FILE *fp_;
	char buffer_[64 * 1024];
};