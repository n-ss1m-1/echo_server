#include<errno.h>
#include<unistd.h>
#include<sys/uio.h>	//?
	

#include "Buffer.h"

ssize_t Buffer::readFd(int fd,int* saveErrno)
{
	//预先分配的缓冲区(LT模式，不知道tcp数据的大小)
	char extraBuffer[65536] = {0};
	const size_t writable = writableBytes();

	//使用iovec分配两个连续的缓存区
	struct iovec vec[2];
	vec[0].iov_base = beginWrite();
	vec[0].iov_len = writable;
	vec[1].iov_base = extraBuffer;
	vec[1].iov_len = sizeof(extraBuffer);

	//根据经验判断，是否需要额外缓冲区
	const int iovcnt = (writable < sizeof(extraBuffer)) ? 2 : 1;

	//readv  顺序读取到连续的iovcnt个缓冲区中(针对多个不连续缓存区)
	const ssize_t n = ::readv(fd,vec,iovcnt);

	//n判断
	if(n<0) 		*saveErrno = errno;
	else if(n<writable) 	writerIndex_ += n;
	else 			append(extraBuffer,n-writable);

	return n;
}
ssize_t Buffer::writeFd(int fd,int* saveErrno)
{
	//此处使用write，针对单个缓冲区的直接写
	const ssize_t n = ::write(fd,beginWrite(),writableBytes());

	if(n<0) *saveErrno=errno;

	return n;	
}


