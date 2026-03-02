#include<sys/epoll.h>

#include "Channel.h"
#include "EventLoop.h"	//需要使用EventLoop的成员函数

//初始化事件类型
const int Channel::NoneEvent = 0;
const int Channel::ReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::WriteEvent = EPOLLOUT;

//构造
Channel::Channel(EventLoop* loop, int fd):
	loop_(loop),
	fd_(fd),
	events_(0),
	revents_(0),
	index_(-1),
	tied_(false)
	{}
//析构
Channel::~Channel() {}

//Channel->loop->poller 更新fd在树上的状态/监听事件
void Channel::update()
{
	loop_->updateChannel(this);
}

//Channel->loop->poller 更新fd在树上的状态/监听事件
void Channel::remove()
{
	loop_->removeChannel(this);
}

//绑定
void Channel::tie(const std::shared_ptr<void>& obj)
{
	tie_=obj;
	tied_=true;	
}

//根据tied_判断 是否需要提升为shared_ptr来保护该对象(TcpConnection才需要保护)
void Channel::handleEvent(std::chrono::steady_clock::time_point receiveTime)
{
	if(tied_)
	{
		std::shared_ptr<void> guard = tie_.lock();
		if(guard)
		{
			handleEventWithGuard(receiveTime);
		}
	}
	else
	{
		handleEventWithGuard(receiveTime);
	}
}

//根据发生的不同的事件 调用相应的回调函数
void Channel::handleEventWithGuard(std::chrono::steady_clock::time_point receiveTime)
{
	if((revents_ & EPOLLHUP) && !(revents_ & Channel::ReadEvent))	//仅当连接挂起&&数据已读取完毕 才关闭连接
	{
		if(closeCallback_) closeCallback_();
	}
	if(revents_ & EPOLLERR)
	{
		if(errorCallback_) errorCallback_();
	}
	if(revents_ & Channel::ReadEvent)
	{
		if(readCallback_) readCallback_(receiveTime);
	}
	if(revents_ & EPOLLOUT)
	{
		if(writeCallback_) writeCallback_();
	}
}


