#pragma once 
#include<functional>
#include<memory>
#include<string>
#include<vector>

#include "noncopyable.h"

class EventLoop;
class EventLoopThread;


class EventLoopThreadPool : noncopyable
{
public:
	//线程初始化回调声明
	using ThreadInitCallback = std::function<void(EventLoop* loop)>;	

	//构造
	EventLoopThreadPool(EventLoop* baseLoop,const std::string& name = std::string());
	//析构
	~EventLoopThreadPool();
	
	//设置子线程数
	void setThreadNum(int numThreads) {numThreads_=numThreads;}
	
	//启动
	void start(const ThreadInitCallback& cb = ThreadInitCallback());
	
	//轮询获取subLoop
	EventLoop* getNextLoop();
	
	std::vector<EventLoop*> getAllLoops();
	bool started() {return started_;}	
	const std::string& name() {return name_;}

private:
	EventLoop* baseLoop_;
	const std::string name_;
	bool started_;
	int numThreads_;
	int next_;
	std::vector<std::unique_ptr<EventLoopThread>> threads_;
	std::vector<EventLoop*> loops_;

};









