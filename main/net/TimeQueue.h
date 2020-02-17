// TimerQueue.h
// Created by Lixin on 2020.02.16
// Version 0.0

#ifndef MAIN_NET_TIMEQUEUE_H
#define MAIN_NET_TIMEQUEUE_H

#include <set>
#include <vector>

#include "main/base/Mutex.h"
#include "main/base/Timestamp.h"
#include "main/net/Callbacks.h"
#include "main/net/Channel,h"

namespace main
{
namespace net
{

class EventLoop;
class Timer;
class TimerId;

class TimeQueue : noncopyable
{
public:
private:
	typedef std::pair<Timestamp,Timer*> Entry;
	typedef std::set<Entry> TimerList;
	typedef std::pair<Timer*,int64_t> ActiveTimer;
	typedef std::set<ActiveTimer> ActiveTimerSet;

	void addTimeInLoop();
	void cancelInLoop(TimerId timerId);
	// called when timerfd alarms
	void handleRead();
	// delete all expired timers
	std::vector getExpired(Timestamp now);
	void reset(const std::vector<Entry>& expired,Timestamp now);

	bool insert(Timer* timer);

	EventLoop* loop_;
	const int timerfd_;
	Channel timerfdChannel_;
	// Timer list sorted by expiration
	TimerList timers_;

	// for cancel()
	ActiveTimerSet activeTimers_;
	bool callingExpiredTimers_;
	ActiveTimerSet cancelingTimers_;

};

} // namespace net
} // namespace main

#endif // MAIN_NET_TIMEQUEUE_H
