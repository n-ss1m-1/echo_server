#include<string>

#include "InetAddress.h"


InetAddress::InetAddress(uint16_t port, std::string ip)
{
	::memset(&addr_, 0, sizeof(addr_));
	addr.sin_family = AF_INET;
	addr.sin_port = ::htons(port);
	::inet_pton(AF_INET, &ip, &addr.sin_addr.s_addr);
}

InetAddress::InetAddress(const sockaddr_in& addr):
	addr_(addr)
{

}

std::string InetAddress::toIp() const
{
	string buf[32]={0};
	::inet_ntop(AF_INET, &addr_.sin_addr.s_addr, &buf, sizeof(buf));
	return buf;
}
std::string InetAddress::toIpPort() const
{
	string buf[64]={0};
	::inet_ntop(AF_INET, &addr_.sin_addr.s_addr, &buf, sizeof(buf));
	uint16_t port = ::ntohs(addr_.sin_port);
	size_t end = ::strlen(buf);
	size_t remaining = buf.size()-end;
	int n = snprintf(buf+end, remaining, ":%u", port);
	if(n<0)
	{

	}
	else if(n>=remaining)
	{

	}
	return buf;
}
uint16_t InetAddress::toPort() const
{
	uint16_t port = ::ntohs(addr_.sin_port);
	return port;
}



