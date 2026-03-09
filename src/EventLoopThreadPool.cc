#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const std::string& name):
	baseLoop_(baseLoop_),
	name_(name),
	started_(false),
	numThreads_(0),
	next_(0),
	threads_(),
	loops_()
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
	//自动销毁 1.EvThread->unique_ptr   2.subLoop->子线程栈空间自动销毁
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
	started_=true;

	for(int i=0; i<numThreads_; i++)
	{
		char buf[32+name_.size()] = {0};
		snprintf(buf,sizeof(buf),"%s%d",name_.c_str(),i);

		EventLoopThread* t = new EventLoopThread(cb,buf);
		threads_.push_back(std::unique_ptr<EventLoopThread>(t));
		loops_.push_back(t->startLoop());
	}

	if(numThreads_ == 0 && cb)
	{
		cb(baseLoop_);
	}

}

EventLoop* EventLoopThreadPool::getNextLoop()
{
	EventLoop* loop = baseLoop_;
	if(!loops_.empty())
	{
		loop = loops_[next_];
		++next_;
		if(next_ >= loops_.size()) next_ = 0;	
	}
	return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
	if(loops_.empty())	return std::vector<EventLoop*>(1,baseLoop_);
	else			return loops_;
}


















