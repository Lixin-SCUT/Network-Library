## 项目简介

 * 项目的简介

&emsp;&emsp;本项目使用C++11实现一个基于Reactor模式的多线程网络服务器框架，封装了基本的 Socket API，为用户提供网络服务调用接口，能够自动处理连接和任务事件的分发与处理。该框架可用于搭建HTTP服务器等，用户可根据需要编写应用层服务。
&emsp;&emsp;本项目主要参考了陈硕的muduo网络库的思想，分析其中的差异，进行进一步的思考与学习。 
 
 * 主要技术点

   1. 非阻塞 I/O + 基于Epoll的 I/O 多路复用，其中Epoll使用边沿触发(ET)模式，相应地回调函数中的连接与读写操作均需要循环直至EAGAIN，避免了LT模式中持续触发的情况。
   2. 基于 Reactor模式+one loop per thread 的多线程模型，用线程池实现固定数量的处理线程。
   3. 线程主体为工作循环，根据工作内容可分为主线程与处理线程，主线程负责接收客户端连接（accept），并通过跨线程调用+Round-Robin轮询调度策略平均分发给处理线程，处理线程负责连接事件监听与读写操作。
   4. 采用智能指针管理的对象资源
   5. 双缓冲的异步日志系统LOG
   6. 连接的优雅关闭
      * 正常情况下，客户端主关闭连接动并发送FIN，服务端read返回0，服务器应等待数据发送完毕，而不是直接close。
      * 如果超出最大并发数或者文件描述符的soft-limit，就在accept后直接关闭。
      * 如果连接出错，则服务器可以直接close。

### 项目目的

 * 从用户的角度出发

&emsp;&emsp;在学习网络编程的过程中，我发现网络编程有很多重复琐碎而又容易遗忘的点揉合在了一起（比如每次都必须输入很多重复而无变化的代码），同时又有一些很重要的点容易被遗忘或者比较（比如加锁解锁等）。进一步地，使用非阻塞I/O等技术不适宜直接调用sockets API，而需要一个网络库作为支撑，为用户提供服务。

 * 从编程开发的角度出发

&emsp;&emsp;Web服务器能够很好的贯穿之前所学的知识，包括之前看过的《C++ Primer》、《STL源码剖析》、《计算机网络》、《TCP/IP详解》、《UNIX网络编程》等书，涵盖了下列知识点：
  
   1. C++语法
   2. TCP协议
   3. Linux网络编程
   4. 网络I/O模型
   5. 多线程与线程同步
   6. 编程规范
   7. git使用
 

### 测试

 * 测试环境：
   1. 系统：Ubuntu 18.04
   2. CPU：Intel Corei5-4590 CPU @ 3.30GHz
   3. 内存：4GB

 * 测试方法：

&emsp;&emsp;1. 分别对单线程和多线程、短连接和长连接进行压力测试。

&emsp;&emsp;2. 理想的测试环境是两台远程计算机通过互联网相互连接，但是如今带宽较低（低于千兆），很容易跑满带宽，因此选择单机环境，以达到最大带宽。

&emsp;&emsp;3. 暂不考虑I/O造成的影响，长短连接请求均直接返回内存中的报文对象。

 * 测试工具：
   1. 单线程（主线程同时为处理线程） 
   2. 主线程+4个处理线程
   3. 客户端工具WebBench
   4. 模拟1000个并发客户端，持续时间60秒。

 * 测试结果

| 线程数 | 短连接QPS | 长连接QPS | 
| - | :-: | :-: | 
| 单线程 | 13588 | 25889 | 
| 多线程| 47608 | 74019 | 

   1. 长连接能处理的请求数远高于短连接，因为没有了连接建立和断开的开销，不需要频繁使用系统调用和频繁建立和销毁对应的结构体。
   2. 无论是短连接还是长连接，多个处理线程能够大幅提高服务器的处理能力，并发连接能够平均分配到多个线程中进行并行处理。

备注：QPS:query per second，每秒接受的请求。

 * 测试结果截图
   1. 单线程 + 短连接
![WebBench](https://github.com/Lixin-SCUT/Network-Library/blob/master/test/resultImage/single%2Bshort.jpg)
   2. 单线程 + 长连接
![WebBench](https://github.com/Lixin-SCUT/Network-Library/blob/master/test/resultImage/single%2Bkeepalive.jpg)
   3. 多线程 + 短连接
![WebBench](https://github.com/Lixin-SCUT/Network-Library/blob/master/test/resultImage/multi%2Bshort.jpg)
   4. 多线程 + 长连接
![WebBench](https://github.com/Lixin-SCUT/Network-Library/blob/master/test/resultImage/multi%2Bkeepalive.jpg)

### 事件记录
04.17：重构了双缓冲的异步日志系统LOG。
04.19: 重构了定时器，修改原来的简易最大堆定时器，使用数组+链表数据结构为主体的时间轮。
