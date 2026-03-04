#pragma once
#include<functional>
#include<memory>
#include<thread>
#include<unistd.h>
#include<string>
#include<atomic>

#include "noncopyable.h"

class Thread : noncopyable
{
public:
	//线程的回调函数
	using ThreadFunc = std::function<void()>;
	
	//构造
	Thread(const ThreadFunc& func, const std::string& name = std::string());
	//析构
	~Thread();
	
	//启动和关闭
	void start();
	void join();
	
	//状态查询接口 3
	bool started() {return started_;}
	bool joined() {return joined_;}
	const std::string& name() {return name_;}

	//获取全局线程数
	static int numCreated() {return numCreated_;}

private:
	//设置默认线程名称
	void setDefaultName();

	//状态 2
	bool started_;
	bool joined_;
	
	//底层线程对象
	std::shared_ptr<std::thread> thread_;

	//线程ID
	pid_t tid_;

	//子线程的回调函数
	ThreadFunc func_;

	//线程名称
	std::string name_;
	
	//全局线程数目
	static std::atomic_int numCreated_;

};






















