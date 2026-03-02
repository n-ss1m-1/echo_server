#pragma once

#include<vector>
#include<string>
#include<stddef.h>


class Buffer
{
public:
	//缓冲区最前端 预留的空间 可以用于添加协议头
	static const size_t CheapPrepend = 8;
	//默认的缓冲区buffer_大小
	static const size_t InitialSize = 1024;		

	//构造函数 禁止隐式转换 //xxxxx
	explicit Buffer(size_t initialSize = InitialSize):buffer_(CheapPrepend+initialSize),readerIndex_(CheapPrepend),writerIndex_(CheapPrepend){}

	//获取待读数据的长度
	size_t readableBytes() const {return writerIndex_-readerIndex_;}
	//获取可写空间的长度
	size_t writableBytes() const {return buffer_.size()-writerIndex_;}
	//获取前置空间的长度(预留+已读)
	size_t prependableBytes() const {return readerIndex_;}

	//返回待读数据的起始地址 即beginRead()
	const char* peek() const {return begin()+readerIndex_;}
	//获取可写空间的起始地址
	char* beginWrite() {return begin()+writerIndex_;}

	//回收已读数据空间：移动读指针
	void retrieve(size_t len)
	{
		if(len < readableBytes()) readerIndex_ += len;
		else			retrieveAll();
	}
	//重置读写指针到初始位置
	void retrieveAll()
	{
		readerIndex_ = writerIndex_ = CheapPrepend;
	}

	//把onMessage函数收到的Buffer数据 转换成string类型数据返回
	std::string retrieveAllAsString() {return retrieveAsString(readableBytes());}
	std::string retrieveAsString(size_t len) 
	{
		std::string result(peek(),len);
		retrieve(len);
		return result;
	}

	//确保有足够的可写空间 不足时扩容
	void ensureWritableBytes(size_t len)
	{
		if(writableBytes()<len) makeSpace(len);
	}
	//把[data,data+len]的数据添加到writable缓冲区当中
	void append(const char* data,size_t len)
	{
		ensureWritableBytes(len);
		std::copy(data,data+len,beginWrite());
		writerIndex_ += len;
	}

	//从fd上读取数据到buffer_
	ssize_t readFd(int fd,int* saveErrno);
	//将buffer_上的数据写到fd
	ssize_t writeFd(int fd,int* saveErrno);


private:
	//返回vector数组首地址
	char* begin() {return &*buffer_.begin();}
	const char* begin() const {return &*buffer_.begin();}
	
	//扩容/腾出空间
	void makeSpace(size_t len)
	{
		if(writableBytes() + prependableBytes() - CheapPrepend < len ) buffer_.resize(writerIndex_+len);
		else
		{
			size_t readable = readableBytes();
			std::copy(begin()+readerIndex_,begin()+writerIndex_,begin()+CheapPrepend);
			readerIndex_=CheapPrepend;
			writerIndex_=CheapPrepend + readable; 
		}
	}


	//底层的buffer
	std::vector<char> buffer_;
	//读指针：指向下一个可读数据位置
	size_t readerIndex_;
	//写指针：指向下一个可写数据位置
	size_t writerIndex_;

};


