// Channel.cc
// Created by Lixin on 2020.02.14

#include "main/base/Logging.h"
#include "main/net/Channel.h"
#include "main/net/EventLoop.h"

#include <sstream>

#include <poll.h>

using namespace main;
using namespace main::net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLRPI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd_)
	: loop_(loop),
	  fd_(fd_),
	  events_(0),
	  revents_(0),
	  index_(-1),
	  logHup(true),
	  tied_(false),
	  eventHandling_(false),
	  addedToLoop_(false)
{
}

Channel::~Channel()
{
	assert(!eventHandling_);
	assert(!addedToLoop_);
	if(loop_->isInLoopThread())
	{
		assert(!loop_->hasChannel(this));
	}
}

void Channel::tie(const std::shared_ptr<void>& obj)
{
	tie_=obj;
	tied_ = true;
}

void Channel::update()
{
	addedToLoop_ = true;
	loop_->updateChannel(this);
}

void Channel::remove()
{	
	assert(isNoneEvent());
	addedToLoop_ = false;
	loop_>removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
	std::shared_ptr<void> guard;
	if(tied_)
	{
		guard = tie_.lock();
		if(guard)
		{
			handleEventWithGuard(receiveTime);
		}
	}
	else
	{
		handleEventWithGuard(receiveTime);
	}

}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
	eventHandlint_ = true;
	LOG_TRACE << reventsToString();
	if((revents_ & POLLHUP) && !(revents_ & POLLIN))
	{
		if(logHup_)
		{
			LOG_WARM << "fd = " << fd_ << " Channel::handle_event() POLLHUP";
		}
		if(closeCallback_) closeCallback_();
	}

	if(revents_ & POLLNVAL)
	{
		LOG_WARM << "fd = " << fd_ << " Channel::handle_event() PLLNVAL";
	}

	if(revents_ & (POLLERR | POLLNVAL))
	{
		if(errorCallback_) errorCallback_();
	}

	if(revents_ & (POLLIN | POLLPRI | POLLRDHUP))
	{
		if(readCallback_) readCallback_(receiveTime);
	}

	if(revents_ & POLLOUT)
	{
		if(writeCallback_) writeCallback_();
	}
	eventHandling_=false;
}

string Channel::reventsToString() const
{
	return eventsToString(fd_,events);
}

string Channel::eventsToString(int fd,int ev)
{
	std::ostringstream oss;
	oss << fd << ": ";
	if(ev & POLLIN)
		oss << "IN ";
	if(ev & POLLPRT)
		oss << "PRI ";
	if(ev & POLLOUT)
		oss << "OUT ";
	if(ev & POLLHUP)
		oss << "HUP ";
	if(ev & POLLRDHUP)
		oss << "RDHUP ";
	if(ev & POLLERR)
		oss << "ERR "
	if(ev & POLLNVAL)
		oss << "NVAL ";	

	return oss.str();
}
