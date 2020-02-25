// TcpConnection.h
// Created by Lixin on 2020.02.23

#ifndef MAIN_NET_TCPCONNECTION_H
#define MAIN_NET_TCPCONNECTION_H

using namespace main
{
using namespace net
{

class TcpConnection : public boost::noncopyable
					  public boost::enable_shared_from_this<TcpConnection>
{
public:
	// only for server or client,not for user
	void setCloseCallback(const CLoseCallback& cb)
	{ closeCallback_ = cb; }
private:
	// called when TcpServer accepts a new connection, should be called only once
	void connectEstablished();
	// called when TcpServer has removed from map, should be called only once
	void connectDestroyed();
	
	// Thread safe
	void send(const void* messsage, size_t len);
	void send(const std::string& message);
	void send(Buffer* message);
	// void send(Buffer&& message);
	// void send(String&& message); // move instead of copy

	void shutdown();
private:
	enum StateE { kConnecting, kConnected,kDisconnecting,kDisconnected };

	void setState(StateE s) { state_ = s; }
	void handleRead();
	void handleWrite();
	void handleClose();
	void handleError();
	void sendInLoop();
	void shutdownInloop();

	EventLoop *loop_;
	std::string name_;
	StateE state_; // TODO use atomic variable
	// do not expose these classes to client
	boost::scoped_ptr<Socket> socket_;
	boost::scoped_ptr<Channel> channel_;
	InetAddress localAddr_;
	InetAddress peerAddr_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	CloseCallback closeCallback_;
	Buffer inputBuffer_;
	Buffer outputBuffer_;
};

} // namespace net
} // namespace main
#endif // MAIN_NET_TCPCONNECTION_H

