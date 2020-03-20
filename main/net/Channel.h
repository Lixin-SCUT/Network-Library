// Channel.h
// Created by Lixin on 2020.02.14

#pragma once

#include "base/noncopyable.h"

#include <sys/epoll.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

	
class EventLoop;
class HttpData;


class Channel : noncopyable
{

	typedef std::function<void()> CallBack;

public:
	Channel(EventLoop *loop);
	Channel(EventLoop *loop, int fd);
	~Channel();
	int getFd();
	void setFd(int fd);

	void setHolder(std::shared_ptr<HttpData> holder) 
	{	holder_ = holder; } // 注意这里是把shared_ptr赋值给weak_ptr,不会引发引用计数递增
	std::shared_ptr<HttpData> getHolder() 
	{
		if(holder_.lock() == nullptr) // 先进行提升，防止shared_ptr已经白给
		{	abort(); }
		std::shared_ptr<HttpData> ret(holder_.lock());
		return ret;	
	}

	void setReadHandler(CallBack && readHandler) 
	{	readHandler_ = readHandler; }
	void setWriteHandler(CallBack && writeHandler) 
	{	writeHandler_ = writeHandler; }
	void setErrorHandler(CallBack && errorHandler) 
	{	errorHandler_ = errorHandler; }
	void setConnHandler(CallBack &&	connHandler) 
	{	connHandler_ = connHandler; }

	void handleEvents(); 

	void handleRead();
	void handleWrite();
	void handleError(int fd, int err_num, std::string short_msg);
	void handleConn();

	void setRevents(__uint32_t ev) 
	{	revents_ = ev; }

	void setEvents(__uint32_t ev) 
	{	events_ = ev; }
	__uint32_t &getEvents() 
	{	return events_; }

	bool EqualAndUpdateLastEvents() 
	{
		bool ret = (lastEvents_ == events_);
		lastEvents_ = events_; // 注意这里的组合使得该函数可用于赋新值
		return ret;
	}

	__uint32_t getLastEvents() 
	{	return lastEvents_; }


private:
	CallBack readHandler_;
	CallBack writeHandler_;
	CallBack errorHandler_;
	CallBack connHandler_;

	EventLoop *loop_;
	int fd_;
	__uint32_t events_; // 记录监听事件
	__uint32_t revents_; // 记录响应事件
	__uint32_t lastEvents_; // 记录上一次监听事件

	// 方便找到上层持有该Channel的对象
	std::weak_ptr<HttpData> holder_;
};

typedef std::shared_ptr<Channel> SP_Channel;
