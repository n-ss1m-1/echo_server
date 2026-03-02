#include<unistd.h>
#include<sys/socket.h>
#include<string>
#include<cstring>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/tcp.h>

#include "Socket.h"
#include "InetAddress.h"



void Socket::bindAddress(const InetAddress& localAddr)
{
	if(0 != ::bind(sockfd_, (sockaddr*)localAddr.getSockAddr(), sizeof(sockaddr_in)))
	{

	}
}
void Socket::listen()
{
	if(0 != ::listen(sockfd_, 1024))
	{

	}
}
int Socket::accept(InetAddress* peerAddr)
{
	sockaddr_in addr;
	socklen_t len = sizeof(addr);
	::memset(&addr, 0, len);

	int connfd = accept4(sockfd_, (sockaddr*)&addr, &len, SOCK_NONBLOCK|SOCK_CLOEXEC);
	if(connfd>=0)
	{
		peerAddr->setSockAddr(addr);
	}
	return connfd;
}

void Socket::shutdownWrite()
{
	if(::shutdown(sockfd_, SHUT_WR)<0)
	{

	}
}

void Socket::setTcpNoDelay(bool on)
{
	int optval = on ? 1 : 0;
	::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}
void Socket::setReuseAddr(bool on)
{
	int optval = on ? 1 : 0;
	::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}
void Socket::setReusePort(bool on)
{
	int optval = on ? 1 : 0;
	::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}
void Socket::setKeepAlive(bool on)
{
	int optval = on ? 1 : 0;
	::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}




