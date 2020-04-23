//
// Created by 黎鑫 on 2020/4/23.
//

#include "EchoServer.h"
#include "../log/Logging.h"

#include <functional>

using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;


EchoServer::EchoServer(EventLoop *loop, const int port, const int threadnum)
    : server_(loop, port, threadnum)
{
    server_.SetNewConnectionCallBack(bind(&EchoServer::HandleNewConnection, this, _1));
    server_.SetMessageCallback(bind(&EchoServer::HandleMessageCallBack,this, _1, _2));
    server_.SetSendCompleteCallBack(bind(&EchoServer::HandleSendComplete, this, _1));
    server_.SetCloseCallBack(bind(&EchoServer::HandleClose, this, _1));
    server_.SetErrorCallBack(bind(&EchoServer::HandleError, this, _1));
}

EchoServer::~EchoServer()
{}

void EchoServer::Start()
{
    server_.Start();
}

void EchoServer::HandleNewConnection(const shared_ptr<TcpConnection> &connection)
{
    LOG << "INFO : New connetion connect successfully!";
}

void EchoServer::HandleMessageCallBack(const shared_ptr<TcpConnection> &cb, string &str)
{
    // string str_temp = str;
    string str_temp = "";
    str_temp.swap(str);
    cb->Send(str_temp);
}

void EchoServer::HandleSendComplete(const shared_ptr<TcpConnection> &connection)
{
}

void EchoServer::HandleClose(const shared_ptr<TcpConnection> &connection)
{
    LOG << "INFO :EchoServer connetion has closed. ";
}

void EchoServer::HandleError(const shared_ptr<TcpConnection> &connection)
{
    LOG << "ERROR : error occurs in EchoServer connetion . ";
}




