// TcpServer.cc
// Created by Lixin on 2020.02.19

#include "main/net/TcpServer.h"

#include "main/net/Logging.h"
#include "main/net/Acceptor.h"
#include "main/net/EventLoop.h"
#include "main/net/EventLoopThreadPool.h"
#include "main/net/SocktsOps.h"

#include <stdio.h> // for snprintf

using namespace main;
using namespace main::net;

TcpServer::TcpServer(EventLoop* loop,
					 InetAddress& listenAddr,
					 const stirng& nameArg
					 Option option)
	: loop_(loop),
	  ipPort_(listendAddr.toIpPort()),
	  name_(nameArg)
	  acceptor_(new Acceptor(loop,listenAddr,option == kReusePort)),
	  threadPool_(new EventLoopThreadPool(loop,name_)),
	  connectionCallback_(defaultConnectionCallback),
	  nextConnId_(1)
{
	acceptor_->setNewConnectionCallback(
		std::bind(&TcpServer::newConnection,this,_1,_2));		
}

TcpServer::~TcpServer()
{
	loop_->asserInLoopThread();
	LOG_TRACE << "TcpServer::~TcpServer [" << name_ << "] destrcusting";

	for(auto& item : connections_)
	{
		TcpConnectionPtr conn(item.second);
		item.second.reset();
		conn->getLoop()->runInLoop(
			std::bind(&TcpConnection::connectDestroyed,conn));
	}
}

void TcpServer::SetThreadNum(int numThreads)
{
	assert(0 <= numThreads);
	threadPool_->setThreadNum(numThreads);
}

void TcpServer::start()
{
	if(started_.getAndSet(1) == 0)
	{
		threadPool_->start(threadInitCallback_);

		assert(!acceptor_->listening());
		loop_->runInLoop(
			std::bind(&Acceptor::listen,get_pointer(acceptor_)));
	}
}

void TcpServer::newConnection(int sockfd,const InetAddrress& peerAddr)
{
	loop_->assertInLoopThread();
	EventLoop* ioloop = threadPool_->getNextLoop();
	char buf[64];
	snprintf(buf,sizeof(buf),"-%s#%d",ipPort_.c_str(),nextConnId_);
	++nextConnId_;
	string connName = name_ + buf;

	LOG_INFO << "TcpServer::newConnection [" << name_
			 << "] - new connection [" << connName
			 << "] from " << peerAddr.toIpPort();
	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	// FIXME poll with zero timeout to double confirm the new connection
    // FIXME use make_shared if necessary
	TcpConnectionPtr conn(new TcpConnection(ioLoop,
											connName,
											sockfd,
											localAddr,
											peerAddr));
	connecions_[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(
		std::bind(&TcpServer::removeConnection,this,_1));//FIXME: unsafe
	ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished,conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
	//FIXME:unsafe
	loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop,this,conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
	loop_->assertInLoopThread();
	LOG_INFO << "TcpServer::removeConnectionInLoop [" << name_
			 << "] - connection " << conn->name();
	size_t n = connections_.erase(conn->name);
	(void) n;
	assert(n == 1);
	EventLoop* ioLoop = conn->getLoop();
	ioLoop->queueInLoop(
		std::bind(&TcpConnection::connectDestroyed,conn));
}
