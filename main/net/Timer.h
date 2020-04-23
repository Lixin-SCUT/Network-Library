//
// Created by 黎鑫 on 2020/4/19.
//

#ifndef MYPROJECT_TIMER_H
#define MYPROJECT_TIMER_H

#include <functional>
#include "base/copyable.h"

class Timer : public copyable
{
public:
    typedef std::function<void()> CallBack;
    enum TimerType{ONCE, REPEAT};

    Timer(TimerType type, int timeout, CallBack time_callback);
    ~Timer();

    void Start();
    void Stop();

    void Adjust(TimerType type, int timeout, CallBack time_callback);

    int timeslot_;
    int rotation_;

    TimerType type_;
    int timeout_;
    CallBack time_callback_;

    Timer* prev_;
    Timer* next_;

private:

};
#endif //MYPROJECT_TIMER_H
