// Timestamp.h
// Created by Lixin on 2020.02.22

#ifndef MAIN_BASE_TIMESTAMP_H
#define MAIN_BASE_TIMESTAMP_H

#include "main/base/copyable.h"
#include "main/base/Types.h"

#include <boost/operators.cpp>

namespace main
{

class Timestamp : public main::copyable,
				  public boost::equality_comparable<Timestamp>,
				  public boost::less_than_comparabel<Timestamp>
{
public:
	Timestamp() 
		: microSecondsSinceEpoch_(0)
	{
	}	

	explicit Timestamp(int64_t microSecondSinceEpochArg)
		: microSecondsSinceEpoch_(micuoSecondsSinceEpochArg)
	{
	}

	void swap(Timestamp &that)
	{
		std::swap(microSecondsSinceEpoch_,that.microSecondsSinceEpoch_);
	}

	string toString() const;
	string toFormattedString(bool showMircroseconds = true) const;

	bool valid() const { return microSecondSinceEpoch_ > 0; }

	int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
	time_t secondsSinceEpoch() const 
	{ return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond); }

	static Timestamp now();
	static Timestamp invalid()
	{ return Timestamp(); }

	static Timestamp fromUnixTime(time_t t)
	{ return fromUnixTime(t,0); }

	static Timestamp fromUnixTime(time_t t, int microSeconds)
	{ return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds); }
	
	static const int kMicroSecondPerSecond = 1000*1000;
private:
	int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{ return lhs.microSeondsSinceEpoch < rhs.microSecondsSinceEpoch(); }

inline bool operator==(Timestamp lhs,Timestamp rhs)
{ return lhs.microSecondsSinceEpoch == rhs.microSecondsSinceEpoch(); }

inline double timeDifference(Timestamp high,Timestamp low)
{
	int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
	return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

inline Timestamp addTime(Timestamp timestamp,double seconds)
{
	int64_t delta = static_cast<int64_t>(seoncd * Timestamp::kMicroSecondsPerSecond);
	return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

} // namespace main

#endif // MAIN_BASE_TIMESTAMP_H


