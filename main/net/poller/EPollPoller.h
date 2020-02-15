// EPollPoller.h
// Created by Lixin on 2020.02.15
// Version 0.0

#ifndef MAIN_NET_POLLER_EPOLLPOLLER_H
#define MAIN_NET_POLLER_EPOLLPOLLER_H

#include "main/net/Poller.h"

#include <vector>

struct epoll_event;

namespace main
{
namespace net
{

// IO multiplexing by EPoll
class EPollPoller : public Poller
{
public:
	EPollPoller(EventLoop* loop);
	~EPollPoller() override;

	Timestamp poll(int timeoutMs,ChannelList* activeChannels) override;
	void updateChannel(Channel* channel) override;
	void removeChannel(Channel* channel) override;

private:
	static const int kInitEventListSize = 16;
	
	static const char* operationToString(int op);

	void fillActiveChannels(int numEvents,
							ChannelList* activeChannels) const;
	void update(int operation,Channel* channel);

	typedef std::vector<struct epoll_event> EventList;

	int epollfd_;
	EventList events_;
};

} // namespace net
} // namespace main

#endif // MAIN_NET_POLLER_EPOLLPOLLER_H
