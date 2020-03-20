// Epoll.h
// Created by Lixin on 2020.02.14

#pragma once

#include "Channel.h"
#include "HttpData.h"
#include "Timer.h"
#include "base/noncopyable.h"

#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <vector>
  
class Epoll : noncopyable
{
public:
	Epoll();
	~Epoll();

	void epoll_add(SP_Channel request, int timeout);
	void epoll_mod(SP_Channel request, int timeout);
	void epoll_del(SP_Channel request);

	std::vector<std::shared_ptr<Channel>> poll();
	std::vector<std::shared_ptr<Channel>> getEventsRequest(int events_num);

	void add_timer(std::shared_ptr<Channel> request_data, int timeout);
	void handleExpired();

	int getEpollFd() 
	{	return epollFd_; }


 private:
	static const int MAXFDS = 100000;
	int epollFd_;
	std::vector<epoll_event> events_;
	std::shared_ptr<Channel> fd2chan_[MAXFDS];
	std::shared_ptr<HttpData> fd2http_[MAXFDS];
	TimerManager timerManager_;
};
