#include<semaphore.h>

#include "Thread.h"
#include "CurrentThread.h"


//静态成员初始化
std::atomic_int Thread::numCreated_(0);

//构造
Thread::Thread(const ThreadFunc& func, const std::string& name):
	started_(false),
	joined_(false),
	tid_(0),
	func_(std::move(func)),
	name_(name)
{
	++numCreated_;
	setDefaultName();
}

//析构
Thread::~Thread()
{
	if(started() && !joined()) thread_->detach();
}

//start
void Thread::start()
{
	started_=true;
	
	sem_t sem;
	sem_init(&sem,false,0);
	
	//创建子线程
	thread_ = std::shared_ptr<std::thread>(new std::thread([&](){
		tid_ = CurrentThread::tid();
		sem_post(&sem);
		func_();
				}));

	sem_wait(&sem);
	sem_destroy(&sem);
}

//join
void Thread::join()
{
	joined_=true;
	thread_->join();
}

//设置默认名称
void Thread::setDefaultName()
{
	if(name_.empty())
	{
		char buf[32] = {0};
		snprintf(buf,sizeof(buf),"Thread%d",numCreated_.load());
		name_=buf;
	}
}










