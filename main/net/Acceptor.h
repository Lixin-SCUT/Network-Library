// Acceptor.h
// Created by Lixin on 2020.03.07

#pragma once

#include "main/net/Channel.h"
#include "main/net/Socket.h"
#include "main/net/noncopyable.h"
#include <functional>

namespace main
{
namespace net
{
	
class EventLoop;
class InetAddress;

// Acceptor of incoming TCP connections.
class Acceptor : noncopyable
{
public:
	typedef std::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;

	Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
	~Acceptor();

	void setNewConnectionCallback(NewConnectionCallback& cb)
	{	newConnectionCallback_ = cb; }

	bool listening() const { return listening_; }
	void listen();

private:
	void handleRead();

private:
	EventLoop *loop_;
	Socket acceptSocket_;
	Channel acceptChannel_;
	NewConnectionCallback newConnectionCallback_;
	bool listening_;
	int idleFd_;
};
} // namespace net
} // namespace main
