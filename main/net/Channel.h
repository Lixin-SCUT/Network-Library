//
// Created by 黎鑫 on 2020/4/20.
//

#ifndef MYPROJECT_CHANNEL_H
#define MYPROJECT_CHANNEL_H

#include <cstdint>
#include <functional>
#include "base/noncopyable.h"

class Channel : noncopyable
{
public:
    typedef std::function<void()> CallBack;

    Channel();


    int GetFd() const { return fd_; }
    void SetFd(int fd) {  fd_ = fd; }

    uint32_t GetEvents() const { return events_; }
    void SetEvent(uint32_t events) { events_ = events; }

    void SetReadCallBack(CallBack cb) { readable_callback_ = cb; }
    void SetWritableCallBack(CallBack cb) { writable_callback_ = cb; }
    void SetErrorCallBack(CallBack cb) { error_callback_ = cb; }
    void SetClosedCallBack(CallBack cb) { closed_callback_ = cb; }

    void HandleEvents();

private:
    int fd_;
    uint32_t events_;
    CallBack readable_callback_;
    CallBack writable_callback_;
    CallBack error_callback_;
    CallBack closed_callback_;

};

#endif //MYPROJECT_CHANNEL_H
