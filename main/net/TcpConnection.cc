// TcpConnection.cc
// Created by Lixin on 2020.02.23

#include "main/net/TcpConnection.h"

using namespace main;
using namespace net;

void TcpConnection::shutdown()
{
	// TODO : use compare and swap
	if(state_ = kConnected)
	{
	 setState(kDisconnecting);
	 // FIXME: shared_from_this()?
	 loop_->runInloop(std::bind(&shutdownInLoop,this));
	}
}

void TcpConnection::shutdownInLoop()
{
	loop_->assertInLoopThread();
	if(!channel_->isWriting())
	{
		socket_->shutdownWrite();
	}
}

void TcpConnection::send(const std::stirng& message)
{
	if(state_ = kConnected)
	{
		if(loop_->isInLoopThread())
		{
			sendInLoop(message);
		}
		else
		{
			loop_->runInLoop(std::bind(&sendInLoop,this,message));
		}
	}
}

void TcpConnection::sendInLoop(const string& message)
{
	loop_->assertInLoopThread();
	ssize_t nwrote = 0;
	// if nothing in output queue,try writing directly
	if(!channel_->isWriting() && outputBuffer_.readbleBytes() == 0)
	{
		nwrote = ::write(channel_->fd(),message.data(),message.size());
		if(nwrote >= 0)
		{
			if(implicit_cast<size_t>(nwrote) < message.size())
			{
				LOG_TRACE << "I am going to write more data.";
			}
		}
		else
		{
			nwrote = 0;
			if(errno != EWOULDBLOCK)
			{
				LOG_SYSERR << "TcpConnection::sendInLoop";
			}
		}
	}

	assert(nwrote >= 0);
	if(implicit_cast<size_t>(nwrote) < messsage.size())
	{
		outputBuffer_.append(message.data() + nwrote,
							 message.size() - nwrote);
		if(!channel_->isWriting())
		{
			channel_->enbaleWriting();
		}
	}
}

void TcpConnection::handleWrite()
{
	loop_->assertInLoopThread();
	if(channel_->isWriting())
	{
		ssize_t n = ::write(channel_->fd(),
							outputBuffer_.peek(),
							outputBuffer_.readableBytes());
		if(n > 0)
		{
			outputBuffer_.retrieve(n);
			if(outputBuffer_.readableBytes() == 0)
			{
				channel_->disableWriting();
				if(state_ == kDisconnecting)
				{
					shutdownInLoop();
				}
			} 
			else 
			{
				LOG_TRACE << "I am going to writr more data.";
			}
		}
		else
		{
			LOG_SYSERR << "TcpConnection::handleWrite";
		}
	}
	else
	{
		LOG_TRACE << "Connection is down, no more writing";
	}
}

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
