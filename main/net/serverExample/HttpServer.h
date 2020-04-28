//
// Created by 黎鑫 on 2020/4/23.
//

#ifndef MYPROJECT_HTTPSERVER_H
#define MYPROJECT_HTTPSERVER_H

#include "../base/noncopyable.h"
#include "../EventLoop.h"
#include "../TcpConnection.h"
#include "../TcpServer.h"
#include "../base/MutexLock.h"
#include "../Timer.h"
#include "../WorkThreadPool.h"
#include "HttpSession.h"

#include <functional>
#include <string>
#include <map>

using std::function;
using std::bind;
using std::string;

class HttpServer : noncopyable
{
public:
    HttpServer(EventLoop* loop, const int port, const int threadnum = 0);
    ~HttpServer();

    void Start();

private:
    void HandleNewConnection(const shared_ptr<TcpConnection> connection);
    void HandleMessageCallBack(const shared_ptr<TcpConnection> connection, string& str);
    void HandleSendComplete(const shared_ptr<TcpConnection> connection);
    void HandleClose(const shared_ptr<TcpConnection> connection);
    void HandleError(const shared_ptr<TcpConnection> connection);

private:
    TcpServer server_;

    MutexLock mutex_;
    map<shared_ptr<TcpConnection>, shared_ptr<HttpSession>> connect_list_;
    map<shared_ptr<TcpConnection>, shared_ptr<Timer>> timer_list_;

    WorkThreadPool workthreadpool_;
};

#endif //MYPROJECT_HTTPSERVER_H
