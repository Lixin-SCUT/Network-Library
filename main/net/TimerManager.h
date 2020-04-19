//
// Created by 黎鑫 on 2020/4/19.
//

#ifndef MYPROJECT_TIMERMANAGER_H
#define MYPROJECT_TIMERMANAGER_H

#include "base/noncopyable.h"
#include "base/MutexLock.h"
#include "Timer.h"
#include <vector>
#include <thread>

class TimerManager : noncopyable
{
public:
    static TimerManager* GetTimerManagerInstance();

    void StartManager();
    void StopManager();

    void AddTimer(Timer* timer);
    void RemoveTimer(Timer* timer);
    void AdjustTimer(Timer* timer);


private:
    TimerManager();
    ~TimerManager();

    void TimeThread();

    void CheckTimer();

    void AddTimerUnlock(Timer* timer);
    void RemoveTimerUnlock(Timer* timer);
    void AdjustTimerUnlock(Timer* timer);
    void SetTimerUnlock(Timer* timer);

private:
    static TimerManager* instance_manager_;
    static MutexLock mutex_manager_;
    std::thread thread_;

    bool state_;
    const int slot_interval_;
    const int slot_num_;
    std::vector<Timer*> timer_wheel_;
    int cur_slot;
    MutexLock mutex_timer_;

};
#endif //MYPROJECT_TIMERMANAGER_H
