//
// Created by 黎鑫 on 2020/4/21.
//

#include "TcpServer.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

using std::function;
using std::bind;
using std::placeholders::_1;

TcpServer::TcpServer(EventLoop *loop, const int port, const int threadnum)
    : socket_(),
      fd_(socket_.GetServerFd()),
      main_loop_(loop),
      serv_channnel_(),
      connect_count_(0),
      connect_list_(),
      mutex_(),
      thread_pool_(main_loop_, threadnum)
{
    socket_.SetNoDelay();
    socket_.SetNonBlock();
    socket_.SetReuseAddr();
    socket_.Bind(port);
    socket_.Listen();

    serv_channnel_.SetFd(fd_);
    serv_channnel_.SetEvent(EPOLLIN | EPOLLET);
    serv_channnel_.SetReadCallBack(bind(&TcpServer::NewConnection, this));
    serv_channnel_.SetErrorCallBack(bind(&TcpServer::ErrorConnection, this));
}

TcpServer::~TcpServer()
{
    socket_.CloseServerFd();
}

void TcpServer::Start()
{
    thread_pool_.StartThread();
    main_loop_->AddToPoller(&serv_channnel_);
}

void TcpServer::NewConnection()
{
    struct sockaddr_in client_addr;
    int client_fd;
    while((client_fd = socket_.Accept(client_addr)) > 0)
    {
        if(connect_count_ > kSoftLimit)
        {
            close(client_fd);
            continue;
        }
        ++connect_count_;
        int val = fcntl(client_fd, F_GETEL, 0);
        val |= O_NONBLOCK;
        fcntl(client_fd, F_SETEL, val);

        EventLoop* loop = thread_pool_.GetNextLoop();
        shared_ptr<TcpConnection>  connection (new TcpConnection(loop, client_fd , client_addr));
        connection->SetMessageCallBack(bind(&TcpServer::message_callback_, this));
        connection->SetSendCompleteCallBack(bind(&TcpServer::sendcomplete_callback_, this));
        connection->SetErrorCallBack(bind(&TcpServer::error_callback_, this));
        connection->SetCloseCallBack(bind(&TcpServer::close_callback_, this));
        connection->SetConnectionCleanUp(bind(&TcpServer::RemoveConnection, this, _1));

        {
            MutexLockGuard lock(mutex_);
            connect_list_.insert(make_pair(client_fd, connection));
        }

        newconnection_callback_(connection);
        connection->AddChannelToLoop();
        loop->WakeUp();
    }

}

void TcpServer::RemoveConnection(shared_ptr<TcpConnection> connection)
{
    {
        MutexLockGuard lock(mutex_);
        connect_list_.erase(connection->GetFd());
    }
    --connect_count_;
}

void TcpServer::ErrorConnection()
{
    socket_.CloseServerFd();
}