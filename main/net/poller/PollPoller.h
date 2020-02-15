// PollPoller.h
// Created by Lixin on 2020.02.15
// Version 0.1

#ifndef MAIN_NET_POLLER_POLLPOLLER_H
#define MAIN_NET_POLLER_POLLPOLLER_H

#include "main/net/Poller.h"

#include <vector>

struct pollfd;

namespace main
{
namespace net
{
// IO multiplexing by poll

class PollPoller : public Poller
{
public:
	
	PollPoller(Eventloop* loop);
	~PollPoller() override;

	Timestamp poll(int timeoutMs,ChannelList* activeChannels)override;
	void updateChannel(Channel* channel) override;
	void removeChannel(Channel* channel) override;

private:
	void fillActiveChannels(int numEvents,
							ChannelList* activeChannels) const;
	typedef std::vector<struct pollfd> PollFdllist;
	PollFdList pollfds_;
};

} // namespace net
} // namespace main
#endif // MAIN_NET_POLLER_POLLPOLLER_H
