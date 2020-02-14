// Poller.h
// Created by Lixin on 2020.02.14
// Version 0.1

#ifndef MAIN_NET_POLLER_H
#define MAIN_NET_POLLER_H

#include <map>
#include <vector>

#include "main/base/Timestamp.h"
#include "main/net/EventLoop.h"

namespace main
{
namespace net
{

class Channel;

class Poller : boost::noncopyable
{
public:
	typedef std::vector<Channel*> ChannelList;

	Poller(EventLoop* loop);
	virtual ~Poller();

	virtual Timestamp poll(int timeoutMS,ChannelList* activeChannels) = 0;

	virtual void updateChannel(Channel* channel) = 0;

	virtual bool removeChannel(Channel* channel) = 0;

	virtual bool hasChannel(Channel* channel) const;

	static Poller* newDefaultPoller(EventLoop* loop);

	void asserInLoopThread() const
	{
		ownerLoop_->assertInLoopThread();
	}

protected:
	typedef std::map<int,Channel*> ChannelMap;
	ChannelMap channels_;

private:
	EventLoop* ownerLoop_;
}
} // namespace net	
} // namespace main
#endif // MAIN_NET_POLLER_H
