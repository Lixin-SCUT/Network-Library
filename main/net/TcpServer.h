//
// Created by 黎鑫 on 2020/4/21.
//

#ifndef MYPROJECT_TCPSERVER_H
#define MYPROJECT_TCPSERVER_H

#include "base/Socket.h"
#include "base/MutexLock.h"
#include "base/noncopyable.h"
#include "Channel.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "TcpConnection.h"

#include <functional>
#include <map>

using std::function;
using std::shared_ptr;
using std::string;

const int kSoftLimit = 100000;

class TcpServer : noncopyable
{
public:
    typedef function<void(shared_ptr<TcpConnection>&, string&)> MessageCallBack;
    typedef function<void(shared_ptr<TcpConnection>&)> CallBack;

    TcpServer(EventLoop *loop, const int port, const int threadnum = 0);
    ~TcpServer();

    void Start();

    void SetNewConnectionCallBack(CallBack& cb)
    {   newconnection_callback_ = cb; }

    void SetMessageCallback(MessageCallBack& cb)
    {   message_callback_ = cb; }
    void SetSendCompleteCallBack(CallBack& cb)
    {   sendcomplete_callback_ = cb; }
    void SetErrorCompleteCallBack(CallBack& cb)
    {   error_callback_ = cb; }
    void SetCloseCallBack(CallBack& cb)
    {   close_callback_ = cb; }

private:
    void NewConnection();
    void RemoveConnection(shared_ptr<TcpConnection> connection);
    void ErrorConnection();
private:
    Socket socket_;
    int fd_;
    EventLoop* main_loop_;
    Channel serv_channnel_;

    int connect_count_;
    map<int, shared_ptr<TcpConnection>> connect_list_;

    MutexLock mutex_;
    EventLoopThreadPool thread_pool_;

    CallBack newconnection_callback_;

    MessageCallBack message_callback_;
    CallBack sendcomplete_callback_;
    CallBack error_callback_;
    CallBack close_callback_;



};
#endif //MYPROJECT_TCPSERVER_H
