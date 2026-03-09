#include<stdio.h>
#include "AsyncLogging.h"

AsyncLogging::AsyncLogging(const std::string& basename,off_t rollSize,int flushInterval):
	running_(false),
	basename_(basename),
	rollSize_(rollSize),
	flushInterval_(flushInterval),
	thread_(std::bind(&AsyncLogging::threadFunc,this),"Logging"),
	mutex_(),
	cond_(),
	currentBuffer_(new LargeBuffer),
	nextBuffer_(new LargeBuffer),
	buffers_()
{
	currentBuffer_->bzero();
	nextBuffer_->bzero();

	buffers_.reserve(16);
}

AsyncLogging::~AsyncLogging()
{
	if(running_)
	{
		stop();
	}
}

void AsyncLogging::append(const char* data,int len)
{
	std::lock_guard<std::mutex> lock(mutex_);

	if(currentBuffer_->valid(len))
	{
		currentBuffer_->append(data,len);
	}
	else
	{
		buffers_.push_back(std::move(currentBuffer_));

		if(nextBuffer_) 
		{
			currentBuffer_ = std::move(nextBuffer_);
		}
		else 
		{
			currentBuffer_.reset(new LargeBuffer);
		}

		currentBuffer_->append(data,len);
	}
	
	cond_.notify_one();
}

void AsyncLogging::threadFunc()
{
	LogFile output(basename_,rollSize_);

	BufferPtr newBuffer1(new LargeBuffer);
	BufferPtr newBuffer2(new LargeBuffer);

	newBuffer1->bzero();
	newBuffer2->bzero();

	BufferVector buffersToWrite;
	buffersToWrite.reserve(16);

	while(running_)
	{
		std::unique_lock<std::mutex> lock(mutex_);

		if(buffers_.empty())
		{
			cond_.wait_for(lock, std::chrono::seconds(flushInterval_));
		}

		buffers_.push_back(std::move(currentBuffer_));
		currentBuffer_ = std::move(newBuffer1);

		if(nextBuffer_ == nullptr)
		{
			nextBuffer_ = std::move(newBuffer2);
		}

		buffersToWrite.swap(buffers_);

		for(auto& buffer : buffersToWrite)
		{
			output.append(buffer->data(),buffer->length());
		}

		if(buffersToWrite.size()>2)
		{
			buffersToWrite.resize(2);
		}
		if(newBuffer1 == nullptr)
		{
			newBuffer1 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer1->reset();
		}
		if(newBuffer2 == nullptr)
		{
			newBuffer2 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer2->reset();
		}
		buffersToWrite.clear();

		output.flush();
	}
	//最后结束的时候 再刷新一次 确保所有数据都写入磁盘
	output.flush();
}















