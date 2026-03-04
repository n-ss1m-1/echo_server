#pragma once

#include<functional>
#include<string>
#include<mutex>
#include<condition_variable>

#include "noncopyable.h"
#include "Thread.h"

class EventLoop;

class EventLoopThread : noncopyable
{
public:
	using ThreadInitCallback = std::function<void(EventLoop* loop)>;
	
	//构造
	EventLoopThread(const ThreadInitCallback& cb, const std::string& name = std::string());
	//析构
	~EventLoopThread();


	EventLoop* startLoop();

private:
	void threadFunc();

	//核心 2
	EventLoop* loop_;
	Thread thread_;
	//同步 2
	std::mutex mutex_;
	std::condition_variable cond_;
	//回调
	ThreadInitCallback callback_;
	//状态
	bool exiting_;
};





















