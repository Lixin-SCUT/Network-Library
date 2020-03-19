// CurrentThread.h
// Created by Lixin on 2020.03.06

#pragma once

#include <stdint.h>

namespace CurrentThread 
{

// 定义在Thread.cc中
extern __thread int t_cachedTid;
extern __thread char t_tidString[32];
extern __thread int t_tidStringLength;
extern __thread const char* t_threadName;

void cacheTid();

inline int tid() 
{
	if (t_cachedTid == 0) 
	{
		cacheTid();
	}
	return t_cachedTid;
}

inline const char* tidString()  // for logging
{
	return t_tidString;
}

inline int tidStringLength()  // for logging
{
	return t_tidStringLength;
}

inline const char* name() 
{	return t_threadName; }

} // namespace CurrentThread
