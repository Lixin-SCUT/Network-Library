// TcpConnection.h
// Created by Lixin on 2020.02.23

#ifndef MAIN_NET_TCPCONNECTION_H
#define MAIN_NET_TCPCONNECTION_H

#include "main/base/noncopyable.h"
#include "main/base/Types.h"
#include "main/net/Callbacks.h"
#include "main/net/Buffer.h"
#include "main/net/InetAddress.h"

#include <memory>
#include <functional>
#include <boost/any.hpp>

// struct tcp_info is in <netinet/tcp.h>
struct tcp_info;

using namespace main
{
using namespace net
{

class Channel;
class EventLoop;
class Socket;

// TCP connection,for both client and server usage.
// This is an interface class, so don't expose too much details
class TcpConnection : public noncopyable
					  public std::enable_shared_from_this<TcpConnection>
{
public:
	// Constructs a TcpConnection with a connected sockfd
	// User should not create this object.
	TcpConnection(EventLoop* loop,
				  const string* name,
				  int sockfd,
				  const InetAddress& localAddr,
				  const InetAddress& peerAddr);
	~TcpConnection();

	EventLoop* getLoop() const { return loop_; }
	const string& name const { return name_; }
	const InetAddress& localAddress() const { return localAddr_; }
	const InetAddress& peerAddress() const { return peerAdder_; }
	bool connected() const { return state_ == kConnected; }
	bool disconnected() const { return state_ == kDisConnected; }
	bool getTcpInfo(struct tcp_info*) const;
	string getTcpInfoString() const;

	// Thread safe
	void send(const void* messsage, size_t len);
	void send(const std::string& message);
	void send(Buffer* message);
	// void send(Buffer&& message);
	// void send(String&& message); // move instead of copy
	void shutdown();
	void forceClose();
	void forceCloseWithDelay(double seconds);
	void setTcpNoDelay(bool on);
	void startRead();
	void stopRead();
	bool isReading() const {return reading_;}// NOT thread safe, may race with start/stopReadInLoop
	
	void setContext(const boost::any& context)
	{ context_ = context; }

	const boost::any& getContext() const
	{ return context_; }

	boost::any* getMutableContext()
	{ return &context_; }

	void setConnectionCallback(const ConnectionCallback& cb)
	{ connectionCallback_ = cb; }

	void setMessageCallback(const MessageCallback& cb)
	{ messageCallback_ = cb; }

	void setWriteCompleteCallback( const WriteCompleteCallback& cb)
	{ WriteCompleteCallback_ = cb; }


	void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark)
	{ highWaterMarkCallback_ = cb; highWaterMark_ = highWaterMark; }
	
	Buffer* inputBuffer()
	{ return &inputBuffer_; }

	Buffer* outputBuffer()
	{ return &outputBuffer_; }

	// only for server or client,not for user
	void setCloseCallback(const CLoseCallback& cb)
	{ closeCallback_ = cb; }
	
	// called when TcpServer accepts a new connection, should be called only once
	void connectEstablished();
	// called when TcpServer has removed from map, should be called only once
	void connectDestroyed();
	
private:
	enum StateE { kConnecting, kConnected,kDisconnecting,kDisconnected };

	void setState(StateE s) { state_ = s; }
	void handleRead(Timestamp receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();
	void sendInLoop(const string& message);
	void sendInLoop(const void* message, size_t len);
	void shutdownInloop();
	void forceCloseInLoop();
	const char* stateToString() const;
	void startReadInLoop();
	void stopReadInLoop();

private:
	EventLoop *loop_;
	const string name_;
	StateE state_; // TODO use atomic variable
	bool reading_;
	// do not expose these classes to client
	boost::unique_ptr<Socket> socket_;
	boost::unique_ptr<Channel> channel_;
	InetAddress localAddr_;
	InetAddress peerAddr_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	HighWaterMarkCallback highWaterMarkCallback_;
	CloseCallback closeCallback_;
	size_t higWaterMark_;
	Buffer inputBuffer_;
	Buffer outputBuffer_; //TODO use list<Buffer> as output buffer
	boost::any context_;
	// TODO: creationTime_,lastReceiveTime_
	//		 bytesReceived_,bytesSent_
};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

} // namespace net

template<typename CLASS, typename... ARGS>
class WeakCallback
{
 public:

  WeakCallback(const std::weak_ptr<CLASS>& object,
               const std::function<void (CLASS*, ARGS...)>& function)
    : object_(object), function_(function)
  {
  }

  // Default dtor, copy ctor and assignment are okay

  void operator()(ARGS&&... args) const
  {
    std::shared_ptr<CLASS> ptr(object_.lock());
    if (ptr)
    {
      function_(ptr.get(), std::forward<ARGS>(args)...);
    }
    // else
    // {
    //   LOG_TRACE << "expired";
    // }
  }

 private:

  std::weak_ptr<CLASS> object_;
  std::function<void (CLASS*, ARGS...)> function_;
};

template<typename CLASS, typename... ARGS>
WeakCallback<CLASS, ARGS...> makeWeakCallback(const std::shared_ptr<CLASS>& object,
                                              void (CLASS::*function)(ARGS...))
{
  return WeakCallback<CLASS, ARGS...>(object, function);
}

template<typename CLASS, typename... ARGS>
WeakCallback<CLASS, ARGS...> makeWeakCallback(const std::shared_ptr<CLASS>& object,
                                              void (CLASS::*function)(ARGS...) const)
{
  return WeakCallback<CLASS, ARGS...>(object, function);
}
} // namespace main
#endif // MAIN_NET_TCPCONNECTION_H

