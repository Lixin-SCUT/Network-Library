//
// Created by 黎鑫 on 2020/4/20.
//

#ifndef MYPROJECT_POLLER_H
#define MYPROJECT_POLLER_H

#include "base/noncopyable.h"
#include "base/MutexLock.h"
#include "Channel.h"
#include <sys/epoll.h>
#include <vector>
#include <map>

using std::vector;
using std::map;

class Poller : noncopyable
{
public:
    Poller();
    ~Poller();

    void AddChannel(Channel *channel);
    void UpdateChannel(Channel *channel);
    void RemoveChannel(Channel *channel);

    void EpollWait(vector<Channel*> channel_list);
private:
    int epollfd_;

    map<int, Channel*> channel_map_;
    vector<struct epoll_event> revents_;

    MutexLock mutex_;
};

#endif //MYPROJECT_POLLER_H
