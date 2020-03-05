// Timer.cc
// Created by Lixin on 2020.03.05

#include "main/net/Timer.h"

using namespace main;
using namespace main::net;

AtomicInt64 Timer::s_numCreated_;

void Timer::restart(Timestamp now)
{
	if(repeat_)
	{
		expiration_ = addTime(now, interval_);
	}
	else
	{
		expiration_ = Timestamp::invalid();
	}
