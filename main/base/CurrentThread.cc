// CurrentThread.cc
// Created by Lixin on 2020.03.06

#include "main/base/CurrentThread.cc"

namespace main
{
namespace CurrentThread
{
__thread int t_cachedTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char* t_threadName = "default";

void CurrentThread::cacheTid() 
{
  if (t_cachedTid == 0) 
  {
  	t_cachedTid = gettid();
  	t_tidStringLength = snprintf(t_tidString, 
									sizeof(t_tidString), 
									"%5d ", 
									t_cachedTid);
  }
}
}
} // namespace main
