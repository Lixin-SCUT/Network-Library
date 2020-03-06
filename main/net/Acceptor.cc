// Acceptor.cc
// Created by Lixin on 2020.03.07

#include "Acceptor.h"

#include "main/net/EventLoop.h"
#include "main/net/Channel.h"
#include "main/base/Logging.h"
#include "main/net/SocketsOps.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

using namespace main;
using namespace net;

Acceptor::Acceptor(EventLoop *loop, 
					const InetAddress &listenAddr, 
					bool reuseport)
	:	loop_(loop),
		acceptSocket_(Socket::createNonblockingOrDie(listenAddr.family())),
		acceptChannel_(loop, acceptSocket_.fd()),
		listening_(false),
		idleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
	assert(idleFd_ >= 0);
	acceptSocket_.setReuseAddr(true);
	acceptSocket_.setReusePort(reuseport);
	acceptSocket_.setReadCallback(
		std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
	acceptChannel_.disableAll();
	acceptChannel_.remove();
	::close(idleFd_);
}

void Acceptor::listen()
{
	loop_->asseetInLoopThread();
	listenning_ = true;
	acceptSocket_.listen();
	acceptChannel_.enableReading();
}
	
void Acceptor::handleRead()
{
	loop_->assertInLoopThread();
	InetAddress peerAddr;
	int connfd = acceptSocket_.accept(&peerAddr);
	if(connfd >= 0)
	{
		if(newConnectionCallback_)
		{
			newConnectionCallback_(connfd, peerAddr);
		}
		else
		{
			sockets::close(connfd);
		}
	}
	else
	{
		LOG_SYSERR << "in Acceptor::handleRead";
		if (errno == EMFILE)
    	{
      		::close(idleFd_);
      		idleFd_ = ::accept(acceptSocket_.fd(), NULL, NULL);
      		::close(idleFd_);
      		idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    	}
	}
}
