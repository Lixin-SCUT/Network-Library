// Connector.h
// Created by Lixin on 2020.02.29

#ifndef MAIN_NET_CONNECTOR_H
#define MAIN_NET_CONNECTOR_H

#include "main/base/noncopyable.h"
#include "main/net/InetAddress.h"

#include <fuctional>
#include <memory>

namespace main
{
namespace net
{

class Channel;
class EventLoop;

class Connector : noncopyable,
					public std::enable_shared_from_this<Connector>
{
public:
	typedef function<void (int sockfd)> NewConnectionCallback;

	Connector(EventLoop *loop, const InetAddress &serverAddr);
	~Connector();

	void setNewConnectionCallback(const NewConnectionCalback &cb)
	{ newConnectionCallback_ = cb; }

	void start(); // can be called in any thread
	void restart(); // must be called in loop thread
	void stop(); // can be called in any thread

	const InetAddress& serverAddress const
	{ return serverAddr_; }

private:
	enum States { kDisconnected, kConnecting, kConnected };
	static const int kMaxRetryDelatMs = 30*1000;
	static const int kInitRetryDelayMs = 500;

	void setState(States s) 
		{ state_ = s };
	void startInLoop();
	void stopInLoop();
	void connect();
	void connecting(int sockfd);
	void handleWrite();
	void handleError();
	void retry(int sockfd);
	int removeAndResetChannel();
	void resetChannel();

private:
	EventLoop* loop_;
	InetAddress serverAddr_;
	bool connect_;
	States state_;
	std::unique_ptr<Channel> channel_;
	NewConnectionCallback newConnectionCallback_;
	int retryDelayMs_;
};

} // namespace net
} // namespace main
#endif // MAIN_NET_CONNECTOR_H
