//
// Created by 黎鑫 on 2020/4/19.
//

#include "Timer.h"
#include "TimerManager.h"

Timer::Timer(Timer::TimerType type, int timeout, Timer::CallBack time_callback)
    : type_(type),
      timeout_(timeout),
      time_callback_(time_callback),
      prev_(nullptr),
      next_(nullptr),
      rotation_(0),
      timeslot_(0)
{
    assert(timeout_ > 0);
}

Timer::~Timer()
{
    Stop();
}

void Timer::Start()
{
    TimerManager::GetTimerManagerInstance()->AddTimer(this);
}

void Timer::Stop()
{
    TimerManager::GetTimerManagerInstance()->RemoveTimer(this);
}

void Timer::Adjust(Timer::TimerType type, int timeout, Timer::CallBack time_callback)
{
    type_ = type;
    timeout_ = timeout;
    time_callback_ = time_callback;
    TimerManager::GetTimerManagerInstance()->AdjustTimer(this);
}