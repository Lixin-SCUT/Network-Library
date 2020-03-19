// FileUtil.cc
// Created by Lixin on 2020.03.08
// 提供了日志文件的写入功能

#include "FileUtil.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

AppendFile::AppendFile(string filename) 
	: fp_(fopen(filename.c_str(), "ae")) 
{
	// 用户提供缓冲区
	setbuffer(fp_, buffer_, sizeof(buffer_)); // 先写入缓冲区，再fflush
}

AppendFile::~AppendFile() 
{ 
	fclose(fp_); 
}

void AppendFile::append(const char* logline, const size_t len) 
{
	size_t n = this->write(logline, len);

	size_t remain = len - n;
	while (remain > 0) 
	{
		size_t x = this->write(logline + n, remain);
		
		if (x == 0) 
		{
			int err = ferror(fp_);
			if (err) 
			{
				fprintf(stderr, "AppendFile::append() failed !\n");
			}
			break;
		}
		
		n += x;
		remain = len - n;
	}
}

void AppendFile::flush() 
{ 
	fflush(fp_); 
}

size_t AppendFile::write(const char* logline, size_t len) 
{
	return fwrite_unlocked(logline, 1, len, fp_); // 调用前会加锁，所以只需unlocked版本
}