//
// Created by 黎鑫 on 2020/4/20.
//


#include "Poller.h"
#include "unistd.h"
#include "log/Logging.h"

const int kMaxEventNum  = 4096;
const int kTimeOut = 1000;

Poller::Poller()
    : epollfd_(-1),
      mutex_()
{
    epollfd_ = epoll_create1(EPOLL_CLOEXEC);
    LOG << "epoll_create" << epollfd_;
}

Poller::~Poller()
{
    close(epollfd_);
}


void Poller::AddChannel(Channel *channel)
{
    int fd = channel->GetFd();
    struct epoll_event event;
    event.events |= channel->GetEvents();
    event.data.ptr = channel;

    {
        MutexLockGuard lock(mutex_);
        channel_map_[fd] = channel;
    }

    int n = epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event);

    if(n == -1)
    {
        LOG << "ERROR: epoll_ctl() for AddChannel" ;
        exit(-1);
    }
}

void Poller::UpdateChannel(Channel *channel)
{
    int fd = channel->GetFd();
    struct epoll_event event;
    event.events |= channel->GetEvents();
    event.data.ptr = channel;

    int n = epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event);

    if(n == -1)
    {
        LOG << "ERROR: epoll_ctl() for UpdateChannel" ;
        exit(-1);
    }
}

void Poller::RemoveChannel(Channel *channel)
{
    int fd = channel->GetFd();
    struct epoll_event event;
    event.events |= channel->GetEvents();
    event.data.ptr = channel;

    {
        MutexLockGuard lock(mutex_);
        channel_map_.erase(fd);
    }

    int n = epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &event);

    if(n == -1)
    {
        LOG << "ERROR: epoll_ctl() for RemoveChannel" ;
        exit(-1);
    }
}

void Poller::EpollWait(vector<Channel *> activechannel_list)
{
    int nready = epoll_wait(epollfd_, &(*revents_.begin()), kMaxEventNum, kTimeOut);
    if(nready == -1)
    {
        LOG << "ERROR: epoll_wait() for EpollWait" ;
    }

    for(int i = 0; i < nready; ++i)
    {
        int events = revents_[i].events;
        Channel *channel = static_cast<Channel*>(revents_[i].data.ptr;
        int fd = channel->GetFd();

        map<int, Channel*>::const_iterator iter;
        {
            MutexLockGuard lock(mutex_);
            iter = channel_map_.find(fd);
        }

        if(iter != channel_map_.end())
        {
            channel->SetEvent(events);
            activechannel_list.push_back(channel);
        }
        else
        {
            LOG << "WARN: not find channel! EpollWait" ;
        }
    }

    if(nready == revents_.capacity())
    {
        LOG << "WARN: revents_ need to resize! EpollWait" ;
        revents_.resize(nready * 2);
    }
}