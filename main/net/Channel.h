// Channel.h
// Created by Lixin on 2020.02.14
// Version 0.1

#ifndef MAIN_NET_CHANNEL_H
#define MAIN_NET_CHANNEL_H

#include "main/base/noncopyable.h"
#include "main/base/Timestamp.h"

#include <functional>
#include <memory>

namespace main
{

namespace net
{

class EventLoop;

class Channel:boost::noncopyable
{
public:
	typedef std::function<void()> EventCallback;
	typedef std::function<void(Timestamp)> ReadEventCallback;

	Channel(EventLoop* loop,int fd);
	~Channel();

	void handleEvent(Timestamp receiveTime);
	void setReadCallback(ReadEventCallback cb)
	{ readCallback_ = std::move(cb); }
	void setWriteCallback(EventCallback cb)
	{ writeCallback_ = std::move(cb); }
	void setCloseCallback(EventCallback cb)
	{ closeCallback_ = std::move(cb); }
	void setErrorCallback(EventCallback cb)
	{ errorCallback_ = std::move(cb); }

	void tie(const std::share_ptr<void>&);

	int fd() const { return fd_; }
	int events() const { return events_; }
	void set_revents(int revt) { revents_ = revt; }
	int revents() { return revents_; }
	bool isNoneEvent() const { return events_ == kNoneEvent; }

	void enableReading() { events_ |= kReadEvent; update(); }
	void disable() { events_ &= ~kReadEvent; update(); }
	void enbaleWriting() { events_ |= kWritingEvent; update(); }
	void disableWriting() { events_ &= ~kWritingEvent; update(); }
	void disableALLL() { events_ = kNoneEvent; update(); }
	bool isWriting() const { return events_ & kWriteEvent; }
	bool isReading() cosnt { return events_ & kReadEvent; }

	int index() const { return index_; }
	void set_index(int idx) { index_ = idx; }

	string reventsToString() const;
	string eventsToString() const;

	void doNotLogHup() { logHup_=false; }

	EventLoop* ownerLoop()  { return loop_; }
	voiud remove();

private:
	static string eventsToString(int fd,int ev);

	void update();
	void handleEventWithGuard(Timestamp receiveTime);
	
	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;

	EventLoop* loop_;
	const int fd_;
	int events_;
	int revents_;
	int index_;
	bool logHup_;

	std::weak_ptr<void> tie_;
	bool tied_;
	bool eventHandling_;
	bool addedToLoop_;
	ReadEventCallback readCallback_;
	EventCallback writeCallback_;
	EventCallback closeCallback_;
	EventCallback errorCallback_;
};

} // namespace net 
} // namespace main

#endif // MAIN_NET_CHANNEL_H
