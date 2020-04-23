//
// Created by 黎鑫 on 2020/4/23.
//

#ifndef MYPROJECT_ECHOSERVER_H
#define MYPROJECT_ECHOSERVER_H

#include "../TcpConnection.h"
#include "../EventLoop.h"
#include "../TcpServer.h"

#include <string>
#include <memory>

using std::string;
using std::shared_ptr;

class EchoServer : noncopyable
{
public:
    EchoServer(EventLoop* loop, const int port, const int threadnum = 0);
    ~EchoServer();

    void Start();

private:
    void HandleNewConnection(const shared_ptr<TcpConnection>& connection);

    void HandleMessageCallBack(const shared_ptr<TcpConnection>& connection, string& str);
    void HandleSendComplete(const shared_ptr<TcpConnection>& connection);
    void HandleClose(const shared_ptr<TcpConnection>& connection);
    void HandleError(const shared_ptr<TcpConnection>& connection);

private:
    TcpServer server_;
};

#endif //MYPROJECT_ECHOSERVER_H
