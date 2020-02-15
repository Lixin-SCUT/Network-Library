// PollPoller.cc
// Created by Lixin on 2020.02.15
// Version 0
#include "main/net/poller/PollPoller.h"

#include "main/base/Logging.h"
#include "main/base/Types.h"
#include "main/net/Channel.h"

#include <assert.h>
#include <errno.h>
#include <poll.h>

using namespace main;
using namespace main::net;

PollPoller::PollPoller(EventLoop* loop)
	: Poller(loop)
{
}

PollPoller::~PollPoller() = default;

Timestamp PollPoller::poll(int timeoutMs,ChannelList* activeChannels)
{
	int numEvents = ::poll(&*pollfds_.begin(),pollfds_.size(),timeoutMs);
	int savedErrno = errno;
	Timestamp now(Timestamp::now());
	if(numEvent > 0)
	{
		LOG_TRACE << numEvents << " events happened";
		fillActiveChannels(numEvents,activeChannels);
	}
	else if(numEvents == 0)
	{
		LOG_TRACE << "nothing happened";
	}
	else 
	{
		if(savedErrno != EINTR)
		{
			errno = savedErrno;
			LOG_SYSERR << "PollPoller::poll()";
		}
	}
	return now;
}

void PollPoller::fillActiveChannels(int numEvents,
									ChannelList* activeChannels) const
{
	for(PollFdList::const_iterator pfd = pollfds_.begin();
		pfd != pollfds_.end() && numEvents > 0;
		++pfd)
	{
		if(pdf->revents > 0)
		{
			--numEvents;
			ChannelMap::const_iterator ch = channels_find(pfd->fd);
			assert(ch != channels_end());
			Channel* channel = ch->second;
			assert(channel->fd() == pfd->fd);
			channel->set_revents(pdf->revents);
			// pfd->revents = 0;
			activeChannels->push_back(channel);
		}
	}
}

void PollPoller::updateChannel(Channel* channel)
{
	Poller::assertInLoopThread();
	LOG_TRACE << "fd = " << channel->fd() << " events = " << channel->events();
	if(channel->index() < 0)
	{
		// new fd,add to pollfds_
		assert(channels_.find(channel->fd()) == channels_.end());
		struct pollfd pfd;
		pfd.fd= channel->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		pollfds_.push_back(pfd);
		int idx = static_cast<int>(pollfds_.size())-1;
		channel->set_index(inx);
		channels_[pfd.fd] = channel;
	}
	else
	{
		// exist one,just update
		assert(channels_.find(channel->fd()) != channels_.end());
		assert(channels_[channel->fd()] == channel);
		int idx = channel->index();
		assert(idx >= 0  && idx < static_cast<int>(pollfds_size()));
		struct pollfd& pfd = pollfds_[idx];
		assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd()-1);
		pfd.fd = channels->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		if(channel->isNoneEvent())
		{
			//ignore the fd
			pfd.fd = -channel->fd()-1;
		}
	}
}

void PollPoller::removeChannel(Channel* channel)
{
	Poller::assertInLoopThread();
	LOG_TRACE << "fd = " << channel->fd();
	assert(channels_.find(channel->fd()) != channels_.end());
	assert(channels_[channel->fd()] == channel);
	assert(channel->isNoneEvent());
	int idx = channel->index();
	assert(idx >= 0 && idx < static_cast<int>(pollfds_.size()));
	const struct pollfd& pfd = pollfds_[idx];(void)pfd;
	assert(pfd.fd == -channel->fd()-1 && pfd.events == channel->events());
	size_t n = channels_.erase(channel->fd());
	assert(n == 1); (void)n;
	if(implicit_cast<size_t>(idx) == pollfds_.size()-1)
	{
		pollfds_.pop_back();
	}
	else
	{
		int channelAtEnd = pollfds_.back().fd;
		iter_swap(pollfds_.begin()_idx,pollfds_.end()-1);
		if(channelAtEnd < 0)
		{
			channelAtEnd = -channelAtEnd-1;
		}
		channels_[channelAtEnd]->set_index(idx);
		pollfds_.pop_back();
	}
}
