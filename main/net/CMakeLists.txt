set(SRCS
	Buffer.cc
	Channel.cc
	Epoll.cc
	EventLoop.cc
	EventLoopThread.cc
	EventLoopThreadPool.cc
	HttpData.cc
	main.cc
	TcpServer.cc
	Timer.cc
	Util.cc
)
include_directories(${PROJECT_SOURCE_DIR}/base)


add_executable(NetworkLibrary ${SRCS})
target_link_libraries(NetworkLibrary libserver_base)


add_subdirectory(base)