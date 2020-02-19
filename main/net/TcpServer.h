// TcpServer.h
// Created by Lixin on 2020.02.19

#ifndef MAIN_NET_TCPSERVER_H
#define MAIN_NET_TCPSERVER_H

#include "main/base/Atomic.h"
#include "main/net/TcpConnection.h"
#include "main/base/Types.h"

#include <map>

namespace main
{
namespace net
{

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

// TCP server,supports single-threaded and thread-pool models.
// This is an interface class,so don't expose too much detail
class TcpServer : noncopyable
{
public:
	typedef std::function<void(EventLoop*)> ThreadInitCallback;
	enum Option
	{
		kNoReusePort;
		kReusePort;
	};

	TcpServer(EventLoop* loop,
			  const InetAddress& listenAddr,
			  const String& nameArg,
			  Option option = kNoReusePort);
	~TcpServer(); // force out-line dtor,for std::unique_ptr members.

	const string& ipPort() const { return ipPort_; }
	const string& name() const { return name_; }
	EventLoop* getLoop() cosnt { return loop_; }

	// Set the number of threads for handling input.
  	//
  	// Always accepts new connection in loop's thread.
  	// Must be called before @c start
  	// @param numThreads
  	// - 0 means all I/O in loop's thread, no thread will created.
  	//   this is the default value.
  	// - 1 means all I/O in another thread.
  	// - N means a thread pool with N threads, new connections
  	//   are assigned on a round-robin basis.
	void setThreadNum(int numThreads);
	void setThreadInitCallback(const ThreadInitCallback& cb)
		{ theadInitCallback_ = cb; }
	// valid after calling start()
	std::shared_ptr<EventLoopThreadPool> threadPool()
		{ return threadPool_; }

	 // Starts the server if it's not listenning.
  	 //
  	 // It's harmless to call it multiple times.
  	 // Thread safe.
	void start();

	// Set Connection callback
	// Not thread safe
	void setConnectionCallback(const ConnectionCallback& cb)
	 	{ ConnectionCallback_ = cb ;}
	
	// Set Message callback
	// Not thread safe
	void setMessageCallback(const MessageCallback& cb)
	 	{ ConnectionCallback_ = cb ;}

	// Set Write complete callback
	// Not thread safe
	void setWriteCompleteCallback(const WriteCompleteCallback& cb)
	 	{ WriteCompleteCallback_ = cb ;}
	
private:
	// Not thread safe, but in loop
	void newConnection(int sockfd,const InetAddress& peerAddr);
	// Thread safe
	void removeConnetcion(const TcpConnectionPtr& conn);
	// Not thread safe, but in loop
	void removeConnectionInLoop(const TcpConnection);

	typedef std::map<string,TcpConnectionPtr> ConnectionMap;

	EventLoop* loop_; // the acceptor loop
	const string ipPort_;
	const string name_;
	std::unique_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
	std::shared_ptr<EventLoopThreadPool> threadPool_;
	ConnectioncCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	ThreadInitCallback threadInitCallback_;
	AtomicInt32 started_;
	// always in loop thread
	int nextConnId_;
	ConnectionMap connections_;
};

} // namespace net
} // namespace main

#endif // MAIN_NET_TCPSERVER_H
