#pragma once

#include<vector>
#include<unordered_map>
#include<chrono>

#include "noncopyable.h"

class EventLoop;
class Channel;

class Poller
{
public:
	using ChannelList = std::vector<Channel*>;

	//构造
	Poller(EventLoop* loop);

	//析构
	virtual ~Poller() = default;

	//为所有IO复用保留统一的接口
	virtual std::chrono::steady_clock::time_point poll(int timeoutMs, ChannelList* activeChannels) = 0;
	//update和remove 更改channel在监听树上的状态
       virtual void updateChannel(Channel* channel) = 0;
       virtual void removeChannel(Channel* channel) = 0;

	//判断channel是否在当前Poller中
	bool hasChannel(Channel* channel) const;

	//获取默认的IO复用的具体实现
	static Poller* newDefaultPoller(EventLoop* loop);

protected:
	//该Poller持有的Channels
	using ChannelMap = std::unordered_map<int, Channel*>;		//fd->Channel*
	ChannelMap channels_;
	
private:
	//该Poller所属的EventLoop
	EventLoop* ownerLoop_;
};



