// Poller.cc
// Created by Lixin on 2020.02.14
// Version 0.1

#include "main/net/Poller.h"

#include "main/net/Channel.h"

using namespace main;
using namespace main::net;

Poller::Poller(EventLoop* loop)
	: ownerLoop_(loop)
{	
}

Poller::~Poller()=default;

bool Poller::hasChannel(Channel* channel) const
{
	assertInLoopThread();
	ChannelMap::const_iterator it = channels_.find(channel->fd());
	return it != channels_.end() && it->second == channel;
}
