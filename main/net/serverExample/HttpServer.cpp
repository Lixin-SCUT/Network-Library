//
// Created by 黎鑫 on 2020/4/23.
//

#include "HttpServer.h"
#include "../log/Logging.h"
#include "../TimerManager.h"

using std::placeholders::_1;
using std::placeholders::_2;

HttpServer::HttpServer(EventLoop *loop, const int port, const int threadnum)
    : server_(loop, port, threadnum),
      mutex_(),
      connect_list_(),
      timer_list_(),
      workthreadpool_(threadnum)
{
    server_.SetNewConnectionCallBack(bind(&HttpServer::HandleNewConnection, this, _1));
    server_.SetMessageCallback(bind(&HttpServer::HandleMessageCallBack, this, _1, _2));
    server_.SetSendCompleteCallBack(bind(&HttpServer::HandleSendComplete, this, _1));
    server_.SetCloseCallBack(bind(&HttpServer::HandleClose, this, _1));
    server_.SetErrorCallBack(bind(&HttpServer::HandleError, this, _1));

    TimerManager::GetTimerManagerInstance()->StartManager();
}

HttpServer::~HttpServer()
{}

void HttpServer::Start()
{
    server_.Start();
    workthreadpool_.Start();
}

void HttpServer::HandleNewConnection(const shared_ptr<TcpConnection> connection)
{
    LOG << "INFO : New connetion connect successfully!";
    shared_ptr<HttpSession> http_session(new HttpSession());
    shared_ptr<Timer> timer(new Timer(Timer::TimerType::ONCE, 4096, bind(&TcpConnection::ShutDown, connection)));
    timer->Start();
    {
        MutexLockGuard lock(mutex_);
        connect_list_.insert(make_pair(connection, http_session));
        timer_list_.insert(make_pair(connection, timer));
    }
}

void HttpServer::HandleMessageCallBack(const shared_ptr<TcpConnection> connection, string &str)
{
    shared_ptr<HttpSession> http_session;
    shared_ptr<Timer> timer;

    {
        MutexLockGuard lock(mutex_);
        http_session = connect_list_[connection];
        timer = timer_list_[connection];
    }
    timer->Adjust(Timer::TimerType::ONCE, 4096,bind(&TcpConnection::ShutDown, connection));

    HttpRequestContext http_request_context;
    string response_context;
    bool result = http_session->PraseHttpRequest(str, http_request_context);

    if(!result)
    {
        http_session->HttpError(400, "Bad Request", http_request_context, response_context);
        connection->Send(response_context);
        return;
    }

    if (workthreadpool_.GetThreadNum() > 0)
    {
        workthreadpool_.AddTask([=]() mutable {
            //string response_context;
            http_session->HttpProcess(http_request_context, response_context);
            connection->Send(response_context);
        });
    }
    else
    {
        http_session->HttpProcess(http_request_context, response_context);
        connection->Send(response_context);
    }
    return;

}

void HttpServer::HandleSendComplete(const shared_ptr<TcpConnection> connection)
{

}

void HttpServer::HandleClose(const shared_ptr<TcpConnection> connection)
{
    {
        MutexLockGuard lock(mutex_);
        connect_list_.erase(connection);
        timer_list_.erase(connection);
    }
}

void HttpServer::HandleError(const shared_ptr<TcpConnection> connection)
{
    {
        MutexLockGuard lock(mutex_);
        connect_list_.erase(connection);
        timer_list_.erase(connection);
    }
    LOG << "ERROR : error occurs in TcpConnection " << connection->GetFd()
        << " and close the connection." ;
}


