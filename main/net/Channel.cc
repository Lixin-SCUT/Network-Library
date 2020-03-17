// Channel.cc
// Created by Lixin on 2020.02.14

#include "Channel.h"

#include "Epoll.h"
#include "EventLoop.h"
#include "Util.h"

#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <queue>

Channel::Channel(EventLoop *loop)
	: loop_(loop), 
	  events_(0), 
	  lastEvents_(0), 
	  fd_(0) 
{ }

Channel::Channel(EventLoop *loop, int fd)
	: loop_(loop), 
	  fd_(fd), 
	  events_(0), 
	  lastEvents_(0) 
{ }

Channel::~Channel() 
{ }

int Channel::getFd() { return fd_; }
void Channel::setFd(int fd) { fd_ = fd; }

void Channel::handleEvents() 
{
    	events_ = 0;
	if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) // 
	{
      		events_ = 0;
      		return;
    	}
    	if (revents_ & EPOLLERR) 
	{
      		if (errorHandler_) 
		{
			errorHandler_();
      		}
		events_ = 0;
      		return;
    	}
    	if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) // 任一可读事件
	{
      		handleRead();
    	}
    	if (revents_ & EPOLLOUT) // 可写事件，buffer未输出完毕需要主动设置
	{
      		handleWrite();
    	}
    	handleConn(); // 维护链接，主要为keepAlive长连接
}


void Channel::handleRead() 
{
  	if (readHandler_) 
	{
    		readHandler_();
	}
}

void Channel::handleWrite() 
{
	if (writeHandler_) 
	{
		writeHandler_();
	}
}

void Channel::handleConn() 
{
	if (connHandler_) 
	{
    		connHandler_();
  	}
}