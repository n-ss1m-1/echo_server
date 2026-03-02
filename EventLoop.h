#pragma once

#include<functional>
#include<vector>
#include<atomic>
#include<chrono>
#include<memory>
#include<mutex>

#include "noncopyable.h"
#include "Channel.h"
class Channel;
class Poller;

class EventLoop : noncopyable
{
public:
	//构造
	EventLoop();

	//析构
	~EventLoop();

	//事件循环
	void loop();					//启动事件循环
	void quit();					//退出事件循环

	//任务队列
	using Functor = std::function<void()>;		//只处理无参无返回值的任务
	void runInLoop(Functor cb);			//执行/加入任务队列
	void queueInLoop(Functor cb);			//加入任务队列
	bool isInLoopThread(){return true;}		//判断是否对应loop-thread

	//wakeup唤醒机制
	void wakeup();					//写入1字节 解除poller阻塞

	//EventLoop的方法->Poller的方法 Channel相关
	void updateChannel(Channel* channel);		//更新该Channel在树上监听的事件
	void removeChannel(Channel* channel);		//将该Channel从树上摘下
	bool hasChannel(Channel* channel);		//该channel是否在相应poller_中


private:
	//标记loop状态
	std::atomic_bool looping_;			//已启动  原子操作 底层通过CAS实现
	std::atomic_bool quit_;				//已关闭

	//Poller
	std::unique_ptr<Poller> poller_;		//一个Loop <-> 一个poller <-> 多个channel
	//poll()返回的时间点
	std::chrono::steady_clock::time_point pollReturnTime_;
	//loop所属线程ID(全局唯一)
	const pid_t threadId_;

	//activeChannels
	using ChannelList = std::vector<Channel*>;	
	ChannelList activeChannels_;			//有事件发生的Channel

	//wakeup机制
	int wakeupFd_;			
	std::unique_ptr<Channel> wakeupChannel_;
	void handleRead();				//处理wakeupFd_的1字节
	
	//处理任务队列
	void doPendingFunctors();	
	std::atomic_bool callingPendingFunctors_;	//标识当前loop是否有正在执行的线程间的内部任务
	std::vector<Functor> pendingFunctors_;		//任务队列
	std::mutex mutex_;				//保护任务队列的线程安全(取出任务的同时 避免别的线程添加任务)


};
















