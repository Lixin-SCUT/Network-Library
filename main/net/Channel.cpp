//
// Created by 黎鑫 on 2020/4/20.
//

#include "Channel.h"
#include <sys/epoll.h>


Channel::Channel()
    : fd_(-1)
{}

void Channel::HandleEvents()
{
    if(events_ & （EPOLLIN | EPOLLPRI))
    {
        readable_callback_();
    }
    else if(events_ & EPOLLOUT)
    {
        writable_callback_();
    }
    else if(events_ & EPOLLERR)
    {
        error_callback_();
    }
    else if(events_ & (EPOLLHUP & EPOLLRDHUP))
    {
        closed_callback_();
    }

}


