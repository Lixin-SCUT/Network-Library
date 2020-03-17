// Timer.cc
// Created by Lixin on 2020.02.20

#include "Timer.h"
	
#include <sys/time.h>
#include <unistd.h>
#include <queue>
	
TimerNode::TimerNode(std::shared_ptr<HttpData> requestData, int timeout)
	: deleted_(false), 
	  SPHttpData(requestData) 
{
	struct timeval now;
	gettimeofday(&now, NULL);
	// 以毫秒计
	expiredTime_ = (((now.tv_sec % 10000) * 1000) 
			+ (now.tv_usec / 1000)) + timeout;
}

TimerNode::~TimerNode() 
{
	if (SPHttpData) 
	{
		SPHttpData->handleClose();
	}
}

TimerNode::TimerNode(TimerNode &tn)
	: SPHttpData(tn.SPHttpData), 
	  expiredTime_(0) 
{ } 

void TimerNode::update(int timeout) 
{
	struct timeval now;
	gettimeofday(&now, NULL);
	expiredTime_ =
		(((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

bool TimerNode::isValid() 
{
	struct timeval now;
	gettimeofday(&now, NULL);
	size_t temp = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
	if (temp < expiredTime_)
	{	return true; }
	else 
	{
		this->setDeleted();
		return false;
	}
}

void TimerNode::clearReq() 
{
	SPHttpData.reset();
	this->setDeleted();
}

TimerManager::TimerManager() 
{ }

TimerManager::~TimerManager() 
{ }

void TimerManager::addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout) 
{
	SPTimerNode new_node(new TimerNode(SPHttpData, timeout));
	timerNodeQueue.push(new_node);
	SPHttpData->linkTimer(new_node);
}


// 处理逻辑：
// 使用优先队列管理到时节点，时间最早的会排在堆头
// 支持随机访问，因为随机删除某节点后破坏了堆的结构
//
// 对于被置为deleted的时间节点，会延迟到
// 超时 或 前面的节点都被删除时才会被删除。
// 当计时器被置为deleted,它最迟会在TIMER_TIME_OUT时间后被删除。
//
// 优点：
// 不需要遍历优先队列，降低时间复杂度。
// 给超时时间一个容忍的时间，就是设定的超时时间是删除的下限
// 而并不是一到超时时间就立即删除
// 如果监听的请求在超时后的下一次请求中又一次出现了，就不用再重新申请RequestData节点了
// 这样可以继续重复利用前面的RequestData，减少了一次delete和一次new的时间。


void TimerManager::handleExpiredEvent() 
{
	while (!timerNodeQueue.empty()) 
	{
		SPTimerNode ptimer_now = timerNodeQueue.top();
		if (ptimer_now->isDeleted())
		{
			timerNodeQueue.pop();
		}
		else if (ptimer_now->isValid() == false)
		{
			timerNodeQueue.pop();
		}
		else
		{	break; }
	}
}