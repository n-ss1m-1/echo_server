#pragma once

#include<functional>
#include<memory>
#include<chrono>

#include "noncopyable.h"

class EventLoop;	//仅需要使用EventLoop的指针或引用

class Channel : private noncopyable
{
public:
	using EventCallback = std::function<void()>;
	using ReadEventCallback = std::function<void(std::chrono::steady_clock::time_point)>;	

	//构造
	Channel(EventLoop* loop, int fd);

	//析构
	~Channel();

	//设置回调
	void setReadCallback(ReadEventCallback cb){readCallback_ = std::move(cb);}
	void setWriteCallback(EventCallback cb){writeCallback_ = std::move(cb);}
	void setCloseCallback(EventCallback cb){closeCallback_ = std::move(cb);}
	void setErrorCallback(EventCallback cb){errorCallback_ = std::move(cb);}

	//设置监听事件
	void enableReading() {events_ |= ReadEvent; update();}
	void enableWriting() {events_ |= WriteEvent; update();}
	void disableReading() {events_ &= ~ReadEvent; update();}
	void disableWriting() {events_ &= ~WriteEvent; update();}
	void disableAll() {events_ = NoneEvent; update();}

	//返回正在监听的事件
	int events() {return events_;}

	//保存发生的事件 revents
	void save_revents(int revents) {revents_=revents;}

	//判断是否监听某事件
	bool isNoneEvent() const {return events_ == NoneEvent;}
	bool isWriting() const {return events_ & WriteEvent;}
	bool isReading() const {return events_ & ReadEvent;}

	//绑定TcpConnection
	void tie(const std::shared_ptr<void>&);

	//处理事件
	void handleEvent(std::chrono::steady_clock::time_point receiveTime);

	//返回/设置该Channel在Poller中的状态
	int index() {return index_;}
	void set_index(int idx) {index_=idx;}
	
	//从监听树上摘下
	void remove();

	//返回 fd_  loop_   
	int fd() const {return fd_;}
	EventLoop* ownerLoop() {return loop_;}

private:
	//事件状态
	static const int NoneEvent;		//无监听事件
	static const int ReadEvent;		//监听可读事件
	static const int WriteEvent;		//监听可写事件

	//更新事件状态
	void update();				//channel的update() 调用loop的update() -> loop调用pooler的update() 然后更新状态
	
	//保存的回调函数
	ReadEventCallback readCallback_;
	EventCallback writeCallback_;
	EventCallback closeCallback_;
	EventCallback errorCallback_;

	//相关参数
	EventLoop* loop_;			//所属的loop
	const int fd_;				//持有的文件描述符
	int events_;				//记录 正在监听的事件集
	int revents_;				//记录 返回的发生的事件集
	int index_;				//该channel在poller中的状态

	std::weak_ptr<void> tie_;		//绑定TcpConnection 
	bool tied_;				//标识是否已绑定(仅绑定TcpConnection)
		
	//有保障地事件处理
	void handleEventWithGuard(std::chrono::steady_clock::time_point receiveTime);		//绑定TcpConnection后的操作
};



