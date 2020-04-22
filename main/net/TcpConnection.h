//
// Created by 黎鑫 on 2020/4/21.
//

#ifndef MYPROJECT_TCPCONNECTION_H
#define MYPROJECT_TCPCONNECTION_H


#include "base/noncopyable.h"
#include "Channel.h"
#include "EventLoop.h"

#include <netinet/in.h>
#include <string>

using std::unique_ptr;
using std::shared_ptr;
using std::string;
using std::enable_shared_from_this;

class TcpConnection : noncopyable,
                      enable_shared_from_this<TcpConnection>
{

public:
    typedef std::function<void(const shared_ptr<TcpConnection>&)> CallBack;
    typedef std::function<void(const shared_ptr<TcpConnection>&, string&)> MessageCallBack;

    TcpConnection(EventLoop* eventLoop, int fd, const struct sockaddr_in &clientaddr);
    ~TcpConnection();

    int GetFd() const { return fd_; }
    EventLoop* GetLoop() const { return loop_; }

    void AddChannelToLoop();
    void Send(const string &str);
    void SendInLoop();
    void ShutDown();
    void ShutDownInLoop();

    void HandleRead();
    void HandleWrite();
    void HandleError();
    void HandleClose();


    // void SetAsyncProcessing(const bool asyncprocessing)
    // {   asyncprocessing_ = asyncprocessing; }

    void SetMessageCallBack(const MessageCallBack& cb)
    {   message_callback_ = cb; }
    void SetSendCompleteCallBack(const CallBack& cb)
    {   sendcomplete_callback_ = cb; }
    void SetCloseCallBack(const CallBack& cb)
    {   close_callback_ = cb; }
    void SetErrorCallBack(const CallBack& cb)
    {   error_callback_ = cb; }
    void SetConnectionCleanUp(const CallBack& cb)
    {   connectioncleanup_ = cb; }

private:
    int fd_;
    EventLoop* loop_;
    unique_ptr<Channel> channel_ptr_;

    struct sockaddr_in client_addr_;
    bool halfclose_;
    bool disconnected_;
    // bool asyncprocessing_;

    string buffer_in_;
    string buffer_out_;

    MessageCallBack message_callback_;
    CallBack sendcomplete_callback_;
    CallBack close_callback_;
    CallBack error_callback_;
    CallBack connectioncleanup_;
};
#endif //MYPROJECT_TCPCONNECTION_H
