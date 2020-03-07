// EventLoopThreadPool.h
// Created by Lixin on 2020.03.07

#pragma once

#include "/main/net/EventLoopThread.h"
#include "/main/base/Logging.h"
#include "/mian/base/noncopyable.h"

#include <memory>
#include <vector>

namespace main
{
namespace net
{

class EventLoopThreadPool : noncopyable
{
public:
	EventLoopThreadPool（EventLoop* baseLoop, int numThreads）;

	～EventLoopThreadPool（）{ LOG_INFO << "~EventLoopThreadPool()"; }
	void start();

	EventLoop* getNextLoop();

private:
	EventLoop *loop;
	bool started_;
	int numThreads_;
	int next_;
	std::vector<std::shared_ptr<EventLoopThread>> threads_;
	std::vector<EventLoop*> loops_;
};

} // namespace net
} // namespace main


