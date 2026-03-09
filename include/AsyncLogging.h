#pragma once

#include<vector>
#include<memory>
#include<mutex>
#include<condition_variable>

#include "noncopyable.h"
#include "Thread.h"
#include "FixedBuffer.h"
#include "LogStream.h"
#include "LogFile.h"


class AsyncLogging : noncopyable
{
public:
	AsyncLogging(const std::string& basename,off_t rollSize,int flushInterval = 3);
	~AsyncLogging();

	void start()
	{
		running_ = true;
		thread_.start();
	}
	void stop()
	{
		running_ = false;
		cond_.notify_one();
	}

	void append(const char* data,int len);

private:
	using LargeBuffer = FixedBuffer<largeBufferSize>;
	using BufferVector = std::vector<std::unique_ptr<LargeBuffer>>;
	using BufferPtr = std::unique_ptr<LargeBuffer>;

	void threadFunc();
	
	std::atomic<bool> running_;

	const std::string basename_;
	const off_t rollSize_;
	const int flushInterval_;

	Thread thread_;
	
	std::mutex mutex_;
	std::condition_variable cond_;
	
	BufferPtr currentBuffer_;
	BufferPtr nextBuffer_;
	BufferVector buffers_;
};




