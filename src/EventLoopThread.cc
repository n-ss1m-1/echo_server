#include "EventLoopThread.h"
#include "EventLoop.h"


EventLoopThread::EventLoopThread(const ThreadInitCallback& cb, const std::string& name):
	loop_(nullptr),
	thread_(std::bind(&EventLoopThread::threadFunc,this),name),
	mutex_(),
	cond_(),
	callback_(cb),
	exiting_(false)
{
}

EventLoopThread::~EventLoopThread()
{
	exiting_=true;
	if(loop_ != nullptr)
	{
		loop_->quit();
		thread_.join();
	}
}

EventLoop* EventLoopThread::startLoop()
{
	//启动子线程
	thread_.start();	
	
	//使用条件变量，等待子线程创建loop
	EventLoop* loop=nullptr;
	{
		std::unique_lock<std::mutex> lock(mutex_);
		cond_.wait(lock,[this](){return loop_!=nullptr;});
		loop=loop_;
	}

	//return
	return loop;
}

void EventLoopThread::threadFunc()
{
	//创建子线程的loop
	EventLoop loop;
	
	//线程初始化callback(判断非空)
	if(callback_) callback_(&loop);
	
	//通知父线程(条件变量)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		loop_ = &loop;
		cond_.notify_one();
	}

	//开始loop
	loop.loop();

	//清理工作
	std::unique_lock<std::mutex> lock(mutex_);
	loop_ = nullptr;

}











