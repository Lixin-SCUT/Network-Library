// CurrentThread.h
// Created by Lixin on 2020.03.06

#ifndef MAIN_BASE_CURRENTTHREAD_H
#define MAIN_BASE_CURRENTTHREAD_H

#include "main/base/Types.h"

namespace main
{
namespace CurrentThread
{
	// internal
	extern __thread int t_cachedTid;
	extern __thread char t_tidString[32];
	extern __thread int t_tidStringLength;
	extern __thread const char* t_threadName;
	
	void cacheTid();

	inline int tid()
	{
		if(__builtin_expect(t_cacheTid == 0, 0))
		{
			cacheTid();
		}
		return t_cachedTid;
	}
	
	inline const char* tidString() // for logging
  	{	return t_tidString; }

  	inline int tidStringLength() // for logging
  	{	return t_tidStringLength; }

  	inline const char* name()
  	{	return t_threadName; }


} // CurrentThread
} // namespace main

#endif // MAIN_BASE_CURRENTTHREAD_H
