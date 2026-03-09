#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<errno.h>

#include "Acceptor.h"
#include "InetAddress.h"
#include "Logger.h"


static int createNonblocking()
{
	int sockfd = ::socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, IPPROTO_TCP);
	if(sockfd<0)
	{
		LOG_FATAL << "listen socket create err " << errno;
	}
	return sockfd;
}

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reusePort):
	loop_(loop),
	acceptorSocket_(createNonblocking()),
	acceptorChannel_(loop, acceptorSocket_.fd()),
	listening_(false)
{
	acceptorSocket_.setReuseAddr(true);
	acceptorSocket_.setReusePort(true);
	acceptorSocket_.bindAddress(listenAddr);
	acceptorChannel_.setReadCallback(std::bind(&Acceptor::handleRead,this));
}
Acceptor::~Acceptor()
{
	acceptorChannel_.disableAll();
	acceptorChannel_.remove();
}

void Acceptor::listen()
{
	listening_ = true;
	acceptorSocket_.listen();
	acceptorChannel_.enableReading();
}

void Acceptor::handleRead()
{
	InetAddress peerAddr;
	int connfd = acceptorSocket_.accept(&peerAddr);
	if(connfd >=0)
	{
		if(NewConnectionCallback_)
		{
			NewConnectionCallback_(connfd, peerAddr);
		}
		else
		{
			::close(connfd);
		}
	}
	else
	{
		LOG_ERROR<<"accept Err";

		if(errno==EMFILE)
		{
			LOG_ERROR<<"sockfd reached limit";
		}
	}
}

