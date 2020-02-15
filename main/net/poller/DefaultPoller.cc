// DefaultPoll.cc
// Created by Lixin on 2020.02.15
// Versin 0.0

#include "main/net/Poller.h"
#include "main/net/poller/PollPoller.h"
#include "main/net/poller/EPOLLPoller.h"

#include <stdlib.h>

using namespace main::net;

Poller* Poller::newDefualtPoller(EventLoop* loop)
{
	if(::getenv("MAIN_USE_POLL"))
	{
		return new PollPoller(loop);
	}
	else
	{
		return new EPollPoller(loop);
	}
}
