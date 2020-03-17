// TcpServer.cc
// Created by Lixin on 2020.02.19
// 用于建立服务器端

#include "TcpServer.h"

#include "Util.h"
#include "base/Logging.h"
	
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <functional>
	
Server::Server(EventLoop *loop, int threadNum, int port)
	: loop_(loop),
	  threadNum_(threadNum),
	  eventLoopThreadPool_(new EventLoopThreadPool(loop_, threadNum)),
	  started_(false),
	  acceptChannel_(new Channel(loop_)),
	  port_(port),
	  listenFd_(socket_bind_listen(port_)) 
{
	acceptChannel_->setFd(listenFd_); // 设置监听描述符
	handle_for_sigpipe(); // 忽略SIGPIPE信号，防止对方断开连接继续执行写操作导致服务进程意外退出
	if (setSocketNonBlocking(listenFd_) < 0) // 设置非阻塞I/O
	{
		perror("set socket non block failed");
		abort();
	}
}

void Server::start() 
{
	eventLoopThreadPool_->start(); // 启动线程池
	// acceptChannel_->setEvents(EPOLLIN | EPOLLET | EPOLLONESHOT);
	acceptChannel_->setEvents(EPOLLIN | EPOLLET);
	acceptChannel_->setReadHandler(bind(&Server::handNewConn, this)); // 新连接事件
	acceptChannel_->setConnHandler(bind(&Server::handThisConn, this)); // 更新监听事件（非必须，当使用了ONESHOT的时候必须要）
	loop_->addToPoller(acceptChannel_, 0);
	started_ = true;
}

void Server::handNewConn() 
{
	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(struct sockaddr_in));
	socklen_t client_addr_len = sizeof(client_addr);
	int accept_fd = 0;
	while ((accept_fd = accept(listenFd_, 
				(struct sockaddr *)&client_addr,
				&client_addr_len)) > 0) 
	{
		EventLoop *loop = eventLoopThreadPool_->getNextLoop(); // 从线程池获得loop
		LOG << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":"
			<< ntohs(client_addr.sin_port);
		
		// 当使用HTTP的keepAlive时，TCP的保活机制默认是关闭的，
		// int optval = 0;
		// socklen_t len_optval = 4;
		// getsockopt(accept_fd, SOL_SOCKET,	SO_KEEPALIVE, &optval, &len_optval);
		// cout << "optval ==" << optval << endl;
		
		// 限制服务器的最大并发连接数
		if (accept_fd >= MAXFDS) 
		{
			close(accept_fd);
			continue;
		}
		// 设为非阻塞模式
		if (setSocketNonBlocking(accept_fd) < 0) 
		{
			LOG << "Set non block failed!";
			// perror("Set non block failed!");
			return;
		}

		setSocketNodelay(accept_fd);
		// setSocketNoLinger(accept_fd);

		shared_ptr<HttpData> req_info(new HttpData(loop, accept_fd)); //新建HttpData对象
		req_info->getChannel()->setHolder(req_info); // 绑定Channel的HttpData对象
		loop->queueInLoop(std::bind(&HttpData::newEvent, req_info)); // 跨线程传输已连接套接字connfd
	}
	acceptChannel_->setEvents(EPOLLIN | EPOLLET); // 设置监听事件
}