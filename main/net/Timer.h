// Timer.h
// Created by Lixin on 2020.03.05

#ifndef MAIN_NET_TIMER_H
#define MAIN_NET_TIMER_H

#include "main/base/Atomic.h"
#include "main/base/Timestamp.h"
#include "main/net/Callbacks.h"

namespace main
{
namespace net
{

class Timer : noncopyable
{
public:
	Timer(TimerCallback cb, Timestamp when, double interval)
		:	callback_(cb),
			expiration_(when),
			interval_(interval),
			repeat_(interval > 0.0),
			sequence_(s_numCreated_.incrementAndGet())
	{ }

	void run() const
	{ callback_; }

	Timestamp expiration() const { return expiration_; }
	bool repeat() cosnt { return repeat_; }
	int64_t sequence() const { return sequence_; }

	void restart(Timestamp now);

	static int64_t numCreated() { return s_numCreated_.get(); }

private:
	const TimerCallback callback_;
	Timestamp expiration_;
	const double interval_;
	const bool repeat_;
	const int64_t sequence;

	static AtomicInt64 s_numCreated_;
};

} // namespace net
} // namespace main
#endif // MAIN_NET_TIMER_H


