#pragma once

#include<sys/epoll.h>
#include<vector>
#include<chrono>

#include "Poller.h"

class Channel;

class EPollPoller : public Poller
{
public:
	EPollPoller(EventLoop* loop);
	~EPollPoller() override;

	//重写抽象方法
	std::chrono::steady_clock::time_point poll(int timeoutMs, ChannelList* activeChannels) override;
	void updateChannel(Channel* channel) override;		//更新的决策者
	void removeChannel(Channel* channel) override;		//更新的决策者

private:
	void update(int operation, Channel* channel);		//更新的实施者 此处调用epoll_ctl

	int epfd_;						//epoll_create的返回值

	static const int InitEventListSize = 16;
	using EventList = std::vector<epoll_event>;
	EventList revents_;					//epoll_wait传出的满足监听事件的fd的event

	void fillActiveChannels(int numEvents, ChannelList* activeChannels);	//revents_->ptr->Channel
};

