// TcpConnection.cc
// Created by Lixin on 2020.02.23

#include "main/net/TcpConnection.h"

using namespace main;
using namespace net;

void TcpConnection::handleRead(Timestamp receiveTime)
{
	int savedErrno = 0;
	ssize_t n = inputBuffer_.readFd(channel_->fd(),&saveErrno);
	if(n > 0)
	{
		messageCallback_(shared_from_this(),&inputBuffer_,receriveTime);
	}
	else if (n == 0)
	{
		handleClose();
	}
	else
	{
		errno = savedErrno;
		LOG_STSERR << "TcpConnection::handleRead";
		handleError();
	}
}

void TcpConnection::handleClose()
{
	loop_->assertInLoopThread();
	LOG_TRACE << "TcpConnection::handleClose state = " << state_;
	assert(state_ == kConnected);
	// don't need to close fd, leave it to dtor
	channel_->disableAll();
	// must be the last line
	closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
	int err = sockets::getSocketError(channel->fd());
	LOG_ERROR << "TcpConnection::handleError [" << name_
				<< "] - SO_ERROR = " << err << " " << strerror_tl(err);
}

void TcpConnection::connectDestroyed()
{
	loop_->assertInLoopThread();
	assert(state_ == kConnected);
	setState(kDisconnected);
	channel_->disableAll();
	connectionCallback_(shared_from_this());

	loop_->removeChannel(get_pointer(channel_));
}
