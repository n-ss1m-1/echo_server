#pragma once

#include<functional>
#include<string>
#include<atomic>
#include<memory>
#include<unordered_map>

#include "Callbacks.h"
#include "noncopyable"
#include "EventLoop.h"
#include "Channel.h"
#include "Acceptor.h"
#include "EventLoopThreadPool"
#include "TcpConnection.h"
#include "Buffer.h"


class TcpServer : noncopyable
{
public:
	using ThreadInitCallback = std::function<void(EventLoop*)>;
	
	enum Option 
	{
		KNoReusePort,
		KReusePort,
	};

	TcpServer(EventLoop* loop,const InetAddress& listenAddr,const std::string& nameArg,Option option=KReusePort);
	~TcpServer();

	void setThreadInitCallback(const ThreadInitCallback& cb) {threadInitCallback_=cb;}	
	void setConnectionCallback(const ConnectionCallback& cb) {connectionCallback_=cb;}	
	void setMessageCallback(const messageCallback& cb) {messageCallback_=cb;}	
	void setWriteCompleteCallback(const WriteCompleteCallback& cb) {writeCompleteCallback_=cb;}	

	void setThreadNum(int numThreads);

	void start();

private:
	void newConnection(int sockfd, const InetAddress& peerAddr);
	void removeConnection(const TcpConnectionPtr& conn);
	void removeConnectionInLoop(const TcpConnectionPtr& conn);

	using ConnectionMap = std::unordered_map<std::string,TcpConnectionPtr>;

	EventLoop* loop_;
	const std::string ipPort_;
	const std::string name_;

	std::unique_ptr<Acceptor> acceptor_;
	std::shared_ptr<EventLoopThreadPool> threadPool_;

	ThreadInitCallback threadInitCallback_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;

	int numThreads_;
	std::atomic_int started_;
	int nextConnId_;
	ConnectionMap connections_;

};






