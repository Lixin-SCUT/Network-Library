//  Socket.h
// Created by 黎鑫 on 2020/4/13.
//

#pragma once

#include <sys/socket.h>
#include <netinet/in.h>

class Socket {
public:
    Socket();
    ~Socket();

    //
    int GetServerFd() const
    {   return serverFd_; }

    bool Bind(int port);

    bool Listen();

    int Accept(struct sockaddr_in& cliaddr);

    bool CloseServerFd();

    bool SetReuseAddr();

    bool SetNonBlock();

    bool SetNoDelay();


private:
    int serverFd_;
};
