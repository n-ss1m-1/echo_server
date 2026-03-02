#include<cstring>
#include<unistd.h>
#include<errno.h>

#include "EPollPoller.h"
#include "Channel.h"

//Channel在epoll上的状态
const int New = -1;
const int Added = 1;
const int Deleted = 2;


EPollPoller::EPollPoller(EventLoop* loop):
	Poller(loop),
	epfd_(::epoll_create(EPOLL_CLOEXEC)),
	revents_(InitEventListSize)
{
	if(epfd_<0) 
	{

	}
}

EPollPoller::~EPollPoller()
{
	::close(epfd_);
}

std::chrono::steady_clock::time_point EPollPoller::poll(int timeoutMs, ChannelList* activeChannels) 
{
	
	int numEvents = ::epoll_wait(epfd_, &*revents_.begin(), static_cast<int>(sizeof(revents_)), timeoutMs);
	int saveErrno = errno;
	auto now = std::chrono::steady_clock::now();

	if(numEvents>0)
	{
		fillActiveChannels(numEvents, activeChannels);
		if(numEvents == revents_.size()) revents_.resize(2 * revents_.size());
	}
	else if(numEvents==0)
	{

	}
	else
	{
		if(saveErrno != EINTR)
		{
			errno=saveErrno;

		}
	}

	return now;
}

void EPollPoller::updateChannel(Channel* channel) 
{
	const int index = channel->index();
	
	if(index==New || index==Deleted)
	{
		if(index==New)
		{
			int fd=channel->fd();
			channels_[fd]=channel;
		}
		else
		{
		}
		channel->set_index(Added);
		update(EPOLL_CTL_ADD, channel);
	}
	else
	{
		if(channel->isNoneEvent())
		{
			channel->set_index(Deleted);
			update(EPOLL_CTL_DEL, channel);
		}
		else
		{
			update(EPOLL_CTL_MOD, channel);
		}
	}
}
void EPollPoller::removeChannel(Channel* channel) 
{
	int fd=channel->fd();
	channels_.erase(fd);

	const int index = channel->index();
	if(index==Added)
	{
		update(EPOLL_CTL_DEL, channel);
	}
	channel->set_index(New);
}

void EPollPoller::update(int operation, Channel* channel)	
{
	epoll_event event;
	::memset(&event,0,sizeof(event));

	int fd=channel->fd();

	event.events=channel->events();
	event.data.fd=fd;
	event.data.ptr=channel;

	if(epoll_ctl(epfd_, operation, fd, &event)<0)
	{
		if(operation == EPOLL_CTL_DEL) 	
		{

		}
		else
		{

		}			
	}
}


void EPollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels)	
{
	for(int i=0;i<numEvents;i++)
	{
		Channel* channel = static_cast<Channel*>(revents_[i].data.ptr);
		channel->save_revents(revents_[i].events);
		activeChannels->push_back(channel);
	}
}





