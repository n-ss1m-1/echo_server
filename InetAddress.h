#pragma once

#include<arpa/inet.h>
#include<netinet/in.h>
#include<string>


class InetAddress
{
public:
	//explicit:避免传入参数时的隐式转换(单参数的构造函数)
	//用于lfd
	explicit InetAddress(uint16_t port=0, std::string ip = "127.0.0.1");
	//用于cfd
	explicit InetAddress(const sockaddr_in& addr);

	//返回本地字节序的ip/port
	std::string toIp() const;
	std::string toIpPort() const;
	uint16_t toPort() const;

	//获取/设置 addr_
	const sockaddr_in getSockAddr() const {return addr_;}
	void setSockAddr(const sockaddr_in& addr) {addr_=addr;}

private:
	sockaddr_in addr_;
};




