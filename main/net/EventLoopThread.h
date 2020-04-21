//
// Created by 黎鑫 on 2020/4/21.
//

#ifndef MYPROJECT_EVENTLOOPTHREAD_H
#define MYPROJECT_EVENTLOOPTHREAD_H

#include "base/noncopyable.h"
#include "EventLoop.h"
#include <thread>
#include <string>
#include <sstream>

using std::thread;
using std::string;
using std::this_thread::get_id;

class EventLoopThread : noncopyable
{
public:
    EventLoopThread(string thread_name = "I/O thread");
    ~EventLoopThread();

    void StartLoop();
    void ThreadFunc();

    EventLoop* GetLoop() const { return loop_;}
    string GetTid()
    {
        std::stringstream ss;
        ss << tid_;
        return ss.str();
    }

private:
    thread thread_;
    thread::id tid_;
    string thread_name_;
    EventLoop* loop_;


};

#endif //MYPROJECT_EVENTLOOPTHREAD_H
