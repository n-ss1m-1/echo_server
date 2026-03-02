#pragma once

#include<memory>
#include<string>
#include<atomic>
#include<chrono>

#include "noncopyable.h"
#include "InetAddress.h"
#include "Callbacks.h"
#include "Buffer.h"

class EventLoop;
class Channel;
class Socket;

class TcpConnection : private noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
	TcpConnection(EventLoop* loop, const std::string& nameArg, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr);
	~TcpConnection();

	void connectEstablished();
	void connectDestroyed();

	void send(const std::string& buf);
	void sendFile(int fileDescriptor, off_t offset, size_t count);
	void shutdown();
	void sendInLoop(const void* data,size_t len);
	void sendFileInLoop(int fileDescriptor, off_t offset, size_t count);
	void shutdownInLoop();

	void setConnectionCallback(const ConnectionCallback& cb){connectionCallback_=cb;}
	void setCloseCallback(const CloseCallback& cb){closeCallback_=cb;}
	void setMessageCallback(const MessageCallback& cb){messageCallback_=cb;}
	void setWriteCompleteCallback(const WriteCompleteCallback& cb){writeCompleteCallback_=cb;}
	void setHighWaterMarkCallback(const HighWaterMarkCallback& cb,size_t highWaterMark){highWaterMarkCallback_=cb;highWaterMark_=highWaterMark;}

	bool connected(){return state_ == KConnected;}
	EventLoop* getLoop(){return loop_;}
	const std::string& name() const {return name_;}
	const InetAddress& localAddress() const {return localAddr_;}
	const InetAddress& peerAddress() const {return peerAddr_;}
	


private:
	enum StateE
	{
		KConnecting,
		KConnected,
		KDisconnecting,
		KDisconnected
	};
	void setState(StateE state){state_=state;}

	void handleRead(std::chrono::steady_clock::time_point receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();

	EventLoop* loop_;
	const std::string name_;
	std::atomic_int state_;
	bool reading_;

	std::unique_ptr<Socket> socket_;
	std::unique_ptr<Channel> channel_;

	InetAddress localAddr_;
	InetAddress peerAddr_;

	ConnectionCallback connectionCallback_;
	CloseCallback closeCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	HighWaterMarkCallback highWaterMarkCallback_;
	size_t highWaterMark_;

	Buffer inputBuffer_;
	Buffer outputBuffer_;

};




























