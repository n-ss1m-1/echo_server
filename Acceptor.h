#pragma once

#include<functional>
#include "noncopyable.h"
#include "Socket.h"
#include "Channel.h"

class EventLoop;
class Channel;
class InetAddress;
class Socket;


class Acceptor : private noncopyable
{
public:
	using NewConnectionCallback = std::function<void(int sockfd, const InetAddress& peerAddr)>;

	Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reusePort);
	~Acceptor();

	void setNewConnectionCallback(const NewConnectionCallback& cb){NewConnectionCallback_=cb;}
	bool listening() const {return listening_;}
	void listen();

private:
	void handleRead();

	EventLoop* loop_;
	Socket acceptorSocket_;
	Channel acceptorChannel_;
	NewConnectionCallback NewConnectionCallback_;
	bool listening_;
};
