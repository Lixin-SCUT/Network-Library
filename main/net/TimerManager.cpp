//
// Created by 黎鑫 on 2020/4/19.
//

#include "TimerManager.h"
#include <sys/time.h>


TimerManager* TimerManager::instance_manager_ = nullptr;
MutexLock TimerManager::mutex_manager_;

TimerManager* TimerManager::GetTimerManagerInstance()
{
    if(instance_manager_ == nullptr)
    {
        MutexLockGuard lock(mutex_manager_);
        if(instance_manager_ == nullptr)
        {
            instance_manager_ = new TimerManager();
        }
    }
    return instance_manager_;
}

void TimerManager::StartManager()
{
    state_ = true;
    thread_ = std::thread(&TimerManager::TimeThread,this);
}

void TimerManager::StopManager()
{
    state_ = false;
    if(thread_.joinable())
    {
        thread_.join();
    }
}

TimerManager::TimerManager()
    : thread_(),
    state_(false),
    slot_interval_(1),
    slot_num_(1024),
    timer_wheel_(slot_num_, nullptr),
    cur_slot(0),
    mutex_timer_()
{}

TimerManager::~TimerManager()
{
    StopManager();
}

void TimerManager::TimeThread()
{
    struct timeval tp;
    gettimeofday(&tp, nullptr);
    int pre_time = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    int cur_time;
    int slot_count;
    while(state_)
    {
        gettimeofday(&tp, nullptr);
        cur_time = tp.tv_sec * 1000 + tp.tv_usec / 1000;
        slot_count = (cur_time - pre_time) / slot_interval_;
        pre_time = cur_time;
 

        for(int i = 0; i < slot_count; ++i)
        {
            GetTimerManagerInstance()->CheckTimer();
        }
        std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
}

void TimerManager::CheckTimer()
{
    MutexLockGuard lock(mutex_timer_);
    Timer* cur_timer = timer_wheel_[cur_slot];

    while(cur_timer != nullptr)
    {

        if(cur_timer->rotation_ > 0)
        {
            --cur_timer->rotation_;
            cur_timer = cur_timer->next_;
        }
        else
        {
            cur_timer->time_callback_();

            if(cur_timer->type_ == Timer::TimerType::ONCE)
            {
                Timer* temp = cur_timer;
                cur_timer = cur_timer->next_;
                RemoveTimerUnlock(temp);
            }
            else
            {
                Timer* temp = cur_timer;
                cur_timer = cur_timer->next_;
                AdjustTimerUnlock(temp);
                if(cur_slot == temp->timeslot_ && temp->rotation_ >0)
                {
                    --temp->rotation_;
                }
            }

        }

    }

    cur_slot = (cur_slot + 1 ) % slot_num_;
}

void TimerManager::AddTimer(Timer *timer)
{
    if(timer == nullptr)
    {   return; }

    MutexLockGuard lock(mutex_timer_);
    SetTimerUnlock(timer);
    AddTimerUnlock(timer);
}

void TimerManager::RemoveTimer(Timer *timer)
{
    if(timer == nullptr)
    {   return; }
    MutexLockGuard lock(mutex_timer_);
    RemoveTimerUnlock(timer);
}

void TimerManager::AdjustTimer(Timer *timer)
{
    if(timer == nullptr)
    {   return; }
    MutexLockGuard lock(mutex_timer_);
    AdjustTimerUnlock(timer);
}


void TimerManager::AddTimerUnlock(Timer *timer)
{
    if(timer == nullptr)
    {   return; }


    if(timer_wheel_[timer->timeslot_] != nullptr)
    {
        timer_wheel_[timer->timeslot_]->prev_ = timer;
        timer->next_ = timer_wheel_[timer->timeslot_];
        timer_wheel_[timer->timeslot_] = timer;
    }
    else
    {
        timer_wheel_[timer->timeslot_] = timer;
    }
}

void TimerManager::RemoveTimerUnlock(Timer *timer)
{
    if(timer == nullptr)
    {   return; }

    if(timer == timer_wheel_[timer->timeslot_])
    {
        timer_wheel_[timer->timeslot_]= timer->next_;
        if(timer->next_ != nullptr)
        {
            timer->next_->prev_ = nullptr;
        }
    }
    else if(timer->prev_ != nullptr)
    {
        timer->prev_->next_ = timer->next_;
        if(timer->next_ != nullptr)
        {
            timer->next_->prev_ = timer->prev_;
        }
    }
    else
    {
        return;
    }
    timer->next_ = timer->prev_ = nullptr;
}

void TimerManager::AdjustTimerUnlock(Timer *timer)
{
    if(timer == nullptr)
    {   return; }


    RemoveTimerUnlock(timer);
    SetTimerUnlock(timer);
    AddTimerUnlock(timer);
}

void TimerManager::SetTimerUnlock(Timer *timer)
{
    if(timer == nullptr)
    {   return; }

    int slot = 0;
    if(timer->timeout_ < slot_interval_)
    {
        slot = 1;
    }
    else
    {
        slot = timer->timeout_ / slot_interval_;
    }

    timer->rotation_ = slot / slot_num_;
    timer->timeslot_ = (cur_slot + slot) % slot_num_;
}
