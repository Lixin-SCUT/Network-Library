// InetAddress.h
// Created by Lixin on 2020.03.05

#ifndef MAIN_NET_INETADDRESS_H
#define MAIN_NET_INETADDRESS_H

#include "main/base/copyable.h"

#include <netinet/in.h>

namespace main
{
namespace net
{
namespace sockets
{
const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);
} // namespace sockets

// Wrapper of sockaddr_in
// This is an POD interface class.
class InetAddress : public copyable
{
public:
	// Constructs an endpoint with given port number.
  	// Mostly used in TcpServer listening.
	explicit InetAddress(uint16_t port = 0, 
						 bool loopbackOnly = false, 
						 bool ipv6 = false);
	explicit InetAddress(const struct sockaddr_in& addr)
		:	addr_(addr)
	{}

	explicit InetAddress(const struct sockaddr_in6& addr)
    	:	addr6_(addr)
  	{}

	sa_family_t family() const 
	{ return addr_.sin_family; }

	string toIp const;
	string toIpPort() comst;
	uint16_t toPort() const;

	// default copy/assignment is enough

	const struct sockaddr* getSockAddr() const 
	{ return sockets::sockaddr_cast(&addr6_);}

	void setSockAddrInet6(const struct sockaddr_in6& addr6) { addr6_ = addr6; }

	uint32_t ipNetEndian const;
	uint16_t portNetEndian() const 
	{ return addr.sin_port; }

	// resolve hostname to IP address, not changing port or sin_family
  	// return true on success.
  	// thread safe
	static bool resolve(StringArg hostname, InetAddress* result);

	// static std::vector<InetAddress> resolveAll(const char* hostname, uint16_t port = 0);

  	// set IPv6 ScopeID
  	void setScopeId(uint32_t scope_id);
private:
union
{
	struct sockaddr_in addr_;
	struct sockaddr_in6 addr_6;
};
};

} // namespace net
} // namespace main

#endif // MAIN_NET_INETADDRESS_H
