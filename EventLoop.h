//EventLoop.h
//Created by Lixin on 2020.02.11
//Version 0.1

#include <boost/utility.hpp>
#include <pthread.h>

#include "Channel.h"
#include "Poller.h"
#include "TimerQueue.h"

class EventLoop{
public:
	EventLoop();
	~EventLoop();
	void loop();
	static EventLoop* getEventLoopOfCurrentThread();
private:
	const pid_t threadId_;
	bool isInLoopThread();
	bool assertInLoopThread();
}
