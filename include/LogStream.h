//LogStream.h

#pragma once

#include<string>
#include<cstring>
#include<algorithm>

#include "noncopyable.h"
#include "FixedBuffer.h"

//已知字符串长度时 无需计算strlen() + 传入指针 零拷贝
class GeneralTemplate : noncopyable
{
public:
	GeneralTemplate():data_(nullptr),len_(0) {}
	GeneralTemplate(const char* data,size_t len):data_(data),len_(len) {}

	const char* data_;
	size_t len_;	
};

//格式化整型 重载>>将日志信息输出到4k缓冲区
class LogStream : noncopyable
{
public:
	using Buffer = FixedBuffer<smallBufferSize>;

	//默认构造析构
	
	void append(char* buf,size_t len) {buffer_.append(buf,len);}
	void buffer_reset() {buffer_.reset();}
	const Buffer& buffer() {return buffer_;}

	LogStream& operator<<(bool express);

	LogStream& operator<<(int);
	LogStream& operator<<(unsigned int);
	LogStream& operator<<(short);
	LogStream& operator<<(unsigned short);
	LogStream& operator<<(long);
	LogStream& operator<<(unsigned long);
	LogStream& operator<<(long long);
	LogStream& operator<<(unsigned long long);

	LogStream& operator<<(float);
	LogStream& operator<<(double);

	LogStream& operator<<(char);
	LogStream& operator<<(const char*);
	LogStream& operator<<(const unsigned char*);
	LogStream& operator<<(const std::string&);
	LogStream& operator<<(const GeneralTemplate&);	

private:
	static const int maxNumSize = 32;	
	template<typename T>
	void formatInteger(T num);

	Buffer buffer_;
};






