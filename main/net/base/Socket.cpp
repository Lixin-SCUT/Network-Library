//  Socket.cc
// Created by 黎鑫 on 2020/4/13.
//

#include "Socket.h"
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <netinet/tcp.h>
#include <fcntl.h> // for fcntl
#include <errno.h> // for errno
#include <stdlib.h> // for exit
#include <cstring> // for memset
// #include <sys/socket.h>
// #include <netinet/in.h>

#define BACKLOG 10000

Socket::Socket()
{
    serverFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == serverFd_)
    {
        perror("ERROR : socket() for serverFd_");
        exit(-1);

    }
    std::cout << "serverFd is " << serverFd_ << std::endl;
}

Socket::~Socket()
{
    CloseServerFd();
    std::cout << "serverFd is closing." << std::endl;
}

bool Socket::Bind(int port)
{
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr)); // replace bzero
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    if(-1 == bind(serverFd_, reinterpret_cast<const struct sockaddr*>(&serverAddr), sizeof(serverAddr)))
    {
        CloseServerFd();
        perror("ERROR : bind() for serverFd_");
        exit(-1);
    }
    std::cout << "serverFd binded a random address." << std::endl;
    return true;
}

bool Socket::Listen()
{
    if(-1 == listen(serverFd_, BACKLOG))
    {
        CloseServerFd();
        perror("ERROR : listen() for serverFd_");
        exit(-1);
    }
    std::cout << "serverFd is listening." << std::endl;
    return true;
}

int Socket::Accept(struct sockaddr_in& cliaddr)
{
    socklen_t clilen = sizeof(cliaddr);
    int connfd = accept(serverFd_, reinterpret_cast<struct sockaddr*>(&cliaddr), &clilen);
    if(connfd < 0)
    {
        int savedErrno = errno;
        switch (savedErrno)
        {
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
            case EPROTO:
            case EPERM:
            case EMFILE:
                errno = savedErrno;
                break;
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
                std::cout << "ERROR : unexpected error of accept() for serverFd_" << std::endl;
                break;
            default:
                std::cout << "ERROR : unknown error of accept() for serverFd_" << std::endl;
                break;
        }
    }
    // std::cout << "serverFd accept a new connect." << std::endl;
    return connfd;
}

bool Socket::CloseServerFd()
{
    if(-1 == close(serverFd_))
    {
        CloseServerFd();
        perror("ERROR : close() for serverFd_");
        exit(-1);
    }
    std::cout << "serverFd " << serverFd_ << "is closed now." << std::endl;
    return true;
};

bool Socket::SetReuseAddr()
{
    int on = 1;
    if(-1 == setsockopt(serverFd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(&on)))
    {
        CloseServerFd();
        perror("ERROR : SetReuseAddr() for serverFd_");
        exit(-1);
    }
    std::cout << "serverFd set SO_REUSEADDR ." << std::endl;
    return true;
}

bool Socket::SetNonBlock()
{
    int val;
    val = fcntl(serverFd_, F_GETEL, 0);
    val |= O_NONBLOCK;
    if(-1 == fcntl(serverFd_, F_SETEL, val))
    {
        CloseServerFd();
        perror("ERROR : SetNonBlock() for serverFd_");
        exit(-1);
    }
    std::cout << "serverFd set O_NONBLOCK ." << std::endl;
}

bool Socket::SetNoDelay()
{
    int on = 1;
    if(-1 == setsockopt(serverFd_, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(&on)))
    {
        CloseServerFd();
        perror("ERROR : SetNonBlock() for serverFd_");
        exit(-1);
    }
    std::cout << "serverFd set TCP_NODELAY ." << std::endl;
    return true;
}

