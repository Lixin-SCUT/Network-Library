//
// Created by 黎鑫 on 2020/4/28.
//


#include "WorkThreadPool.h"
#include "log/Logging.h"

#include <sstream>
#include <string>

using std::stringstream;
using std::string;
using std::this_thread::get_id;

WorkThreadPool::WorkThreadPool(int thread_num)
    : thread_num_(thread_num),
      state_(false),
      thread_list_(),
      task_queue_(),
      mutex_(),
      cond_(mutex_)
{

}

WorkThreadPool::~WorkThreadPool()
{
    Stop();
    for(int i = 0; i < thread_num_; ++i)
    {
        thread_list_[i]->join();
    }
    for(int i = 0; i < thread_num_; ++i)
    {
        delete thread_list_[i];
    }
}

void WorkThreadPool::Start()
{
    state_ = true;
    for(int i = 0; i < thread_num_; ++i)
    {
        thread_list_.push_back(new thread(&WorkThreadPool::ThreadFunc, this));
    }
    LOG << "INFO : " << thread_num_ << " WorkThreads are built now.";
}

void WorkThreadPool::Stop()
{
    state_ = false;
    cond_.NotifyAll();
}

void WorkThreadPool::AddTask(function<void()> task)
{
    MutexLockGuard lock(mutex_);
    task_queue_.push(task);
    cond_.Notify();
}

void WorkThreadPool::ThreadFunc()
{
    stringstream ss;
    ss << get_id();
    string tid = ss.str();
    function<void()> task;
    while(state_)
    {
        {
            MutexLockGuard lock(mutex_);
            while(state_ && task_queue_.empty())
            {   cond_.Wait(); }
            if(!state_)
            {   break; }
            task = task_queue_.front();
            task_queue_.pop();
        }
        LOG << "INFO : WorkThread " << tid << " is running task.";
        task();
    }
}


