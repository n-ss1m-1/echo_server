#include<iostream>
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/eventfd.h>
#include<fcntl.h>
#include<memory>
#include<errno.h>

#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"

// 防止一个线程创建多个EventLoop   // 线程级别的“全局变量”
thread_local EventLoop* t_loopInThisThread = nullptr;

//poll()的超时时长
const int PollTimeMs = 10000;

int createEventfd()
{
	int evfd = ::eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);
	if(evfd<0)
	{
		//cout<<"eventfd error"<<errno<<endl;
	}	
	return evfd;
}


//构造
EventLoop::EventLoop():
	looping_(false),
	quit_(true),
	poller_(Poller::newDefaultPoller(this)),
	threadId_(gettid()),
	wakeupFd_(createEventfd()),
	wakeupChannel_(new Channel(this,wakeupFd_)),
	callingPendingFunctors_(false)
{
	if(t_loopInThisThread)
	{
		//cout<<"Another EventLoop"<<t_loopInThisThread<<"exists in this thread "<<threadId_;
	}
	else
	{
		t_loopInThisThread=this;
	}

	wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead,this));
	wakeupChannel_->enableReading();
}

//析构
EventLoop::~EventLoop()
{
	wakeupChannel_->disableAll();
	wakeupChannel_->remove();
	::close(wakeupFd_);
	t_loopInThisThread = nullptr;
}

//事件循环
void EventLoop::loop()
{
	looping_=true;
	quit_=false;

	while(!quit_)
	{
		activeChannels_.clear();
		pollReturnTime_ = poller_->poll(PollTimeMs, &activeChannels_);
		
		for(auto& channel:activeChannels_)
		{
			channel->handleEvent(pollReturnTime_);
		}

		doPendingFunctors();	
	}
	
	looping_=false;
}
void EventLoop::quit()
{
	quit_=true;
	if(!isInLoopThread())
	{
		wakeup();
	}
}					

//任务队列
void EventLoop::runInLoop(Functor cb)
{
	if(isInLoopThread())
	{
		cb();
	}
	else 
	{
		queueInLoop(cb);
	}
}			
void EventLoop::queueInLoop(Functor cb)
{
	{
		std::unique_lock<std::mutex> lock(mutex_);
		pendingFunctors_.emplace_back(cb);
	}

	if(!isInLoopThread() || callingPendingFunctors_) 
	{
		wakeup();
	}
}	

//wakeup唤醒机制
void EventLoop::wakeup()
{
	uint64_t one = 1;
	ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
	if(n<sizeof(one))
	{

	}
}		

//wakeup机制
void EventLoop::handleRead()
{
	uint64_t one = 1;
	ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
	if(n<sizeof(one))
	{

	}
}			

//EventLoop的方法->Poller的方法 Channel相关
void EventLoop::updateChannel(Channel* channel)
{
	poller_->updateChannel(channel);
}
void EventLoop::removeChannel(Channel* channel)
{
	poller_->removeChannel(channel);
}
bool EventLoop::hasChannel(Channel* channel)
{
	return poller_->hasChannel(channel);
}		

//处理任务队列
void EventLoop::doPendingFunctors()
{
	std::vector<Functor> functors;
	callingPendingFunctors_=true;
	{
		std::unique_lock<std::mutex> lock(mutex_);
		functors.swap(pendingFunctors_);
	}

	for(const Functor& func:functors)
	{
		func();
	}

	callingPendingFunctors_=false;
}





