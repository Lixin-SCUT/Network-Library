// TcpClient.h
// Created by Lixin on 2020.03.01

#ifndef MAIN_NET_TCPCLIENT_H
#define MAIN_NET_TCPCLIENT_H

#include "main/base/Mutex.h"
#include "main/net/TcpConnection.h"

namespace main
{
namespace net
{

class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;

class TcpClient : noncopyable
{
public:

	TcpClient(EventLoop *loop,
				const InetAddress &serverAddr,
				const String &nameArg)
	~TcpClient();

	void connect();
	void disconnect();
	void stop();

	TcpConnectionPtr connection() const
	{
		MutexLockGuard lock(mutex_);
		return connection_;
	}

	EventLoop* getLoop() const { return loop_; }
	bool retry() const { return retry_; }
	void enableRetry() { retry_ = true; }
	const string& name() const { return name_; }
 
	void setConnectionCallback(ConnectionCallback cb)
	{ connectionCallback_ = std::move(cb); }

	void setMessageCallback(MessageCalback cb)
	{ messageCallback_ = std::move(cb); }

	void setWriteCompleteCallback(WriteCompleteCallback cb)
	{ writeCompleteCallback_ = std::move(cb); }

private:
	void newConnection(int sockfd);
	void removeConnection(const TcpConnectionPtr& conn);

private:
	EventLoop* loop_;
	ConnectorPtr connector_;
	const string name_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	bool retry_;
	bool connect_;
	// always in loop thread
	int netxConnId_;
	mutable MutexLock mutex_;
	TcpConnectionPtr connection_ GUARDED_BY(mutex_);
};

} // namespace main
} // namespace net

#endif // MAIN_NET_TCPCLIENT_H
