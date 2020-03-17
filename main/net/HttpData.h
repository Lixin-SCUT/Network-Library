// HttpData.h
// Created by Lixin on 2020.03.10
// 这个类主要供于I/O线程，可类比于TcpServer线程之于接受新连接的主线程

#pragma once

#include "Timer.h"
#include "base/noncopyable.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

// 前向声明，简化头文件之间的依赖关系
class EventLoop;
class TimerNode;
class Channel;

enum ProcessState 
{
	STATE_PARSE_URI = 1,
	STATE_PARSE_HEADERS,
	STATE_RECV_BODY,
	STATE_ANALYSIS,
	STATE_FINISH
};

enum URIState 
{
	PARSE_URI_AGAIN = 1,
	PARSE_URI_ERROR,
	PARSE_URI_SUCCESS,
};

enum HeaderState 
{
	PARSE_HEADER_SUCCESS = 1,
	PARSE_HEADER_AGAIN,
	PARSE_HEADER_ERROR
};

enum AnalysisState { ANALYSIS_SUCCESS = 1, ANALYSIS_ERROR };

enum ParseState 
{
	H_START = 0,
	H_KEY,
	H_COLON,
	H_SPACES_AFTER_COLON,
	H_VALUE,
	H_CR,
	H_LF,
	H_END_CR,
	H_END_LF
};

enum ConnectionState { H_CONNECTED = 0, H_DISCONNECTING, H_DISCONNECTED };

enum HttpMethod { METHOD_POST = 1, METHOD_GET, METHOD_HEAD };

enum HttpVersion { HTTP_10 = 1, HTTP_11 };
	
class MimeType 
{
public:	
	static std::string getMime(const std::string &suffix);
private:
	static void init();
	static std::unordered_map<std::string, std::string> mime;
	MimeType();
	MimeType(const MimeType &m);

private:
	static pthread_once_t once_control;
};

class HttpData : noncopyable,
		 public std::enable_shared_from_this<HttpData>  // 将this指针变为shared_ptr,主要用于回调函数
{
public:
	HttpData(EventLoop *loop, int connfd);
	~HttpData() { close(fd_); }
	void reset();
	void SeparateTimer();
	void linkTimer(std::shared_ptr<TimerNode> mtimer) 
	{
		timer_ = mtimer; // 注意是shared_ptr赋值给weak_ptr
	}
	std::shared_ptr<Channel> getChannel()  
	{	return channel_; } // httpData持有channel的shared_ptr
	EventLoop *getLoop()  
	{	return loop_; }

	void handleClose(); // 主动关闭
	void newEvent(); // 把Channel注册给loop

private:
	void handleRead();
	void handleWrite();
	void handleConn();
	void handleError(int fd, int err_num, std::string short_msg);

	URIState parseURI();
	HeaderState parseHeaders();
	AnalysisState analysisRequest();

private:
	EventLoop *loop_;
	std::shared_ptr<Channel> channel_;
	int fd_;
	std::string inBuffer_;
	std::string outBuffer_;
	bool error_;
	ConnectionState connectionState_;

	HttpMethod method_;
	HttpVersion HTTPVersion_;
	std::string fileName_;
	std::string path_;
	int nowReadPos_;
	ProcessState state_;
	ParseState hState_;
	bool keepAlive_;
	std::map<std::string, std::string> headers_;
	std::weak_ptr<TimerNode> timer_;

	
};