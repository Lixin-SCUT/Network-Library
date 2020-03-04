// Socket.h
// Created by Lixin on 2020.03.05

#ifndef MAIN_NET_SOCKET_H
#define MAIN_NET_SOCKET_H

#include "main/base/noncopyable.h"

// struct tcp_info is in <netinet/tcp.h>
struct tcp_info;

namespace main
{
namespace net
{

class InetAddress;

// Wrapper of socket file descriptor.
// It closes the sockfd when desctructs.
// It's thread safe, all operations are delagated to OS.
class Socket : noncopyable
{
public:
	explicit Socket(int sockfd)
		:	sockfd_(sockfd)
		{}
	Socket(Socket&& socket)
		:	sockfd_(socket.sockfd_)
	{ }
	~Socket();

	int fd() const { return sockfd_; }

	bool getTcpInfo(struct tcp_info*) const;
	bool getTcpInfoString(char *buf, int len) const;

	void bindAddress(const InetAddress& localaddr);
	void listen();

	int accept(InetAddress* peeraddr);
	
	void shutdownWrite();
	
	// Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm).
	void setTcpNoDelay(bool on);

	// Enable/disable SO_REUSEADDR
	void setReuseAddr(bool on);

	// Enable/disable SO_REUSEPORT
	void setReusePort(bool on);

	// Enable/disable SO_KEEPALIVE
	void setKeepAlive(bool on);
private:
	const int sockfd_;
}
} // namespace net
} // namespace main

#endif // MAIN_NET_SOCKET_H
