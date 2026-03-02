#include<functional>
#include<string>

#include "TcpServer.h"

static EventLoop* CheckLoopNotNull(EventLoop* loop)
{
	if(loop == nullptr)
	{
		//log

	}	
	return loop;
}

TcpServer::TcpServer(EventLoop* loop,const InetAddress& listenAddr,const std::string& nameArg,Option option):
	loop_(CheckLoopNotNull(loop)),
	ipPort_(listenAddr.toIpPort()),
	name_(nameArg),
	acceptor_(new Acceptor(loop,listenAddr,option==KReusePort)),
	threadPool_(new EventLoopThreadPool(loop,nameArg)),
	threadInitCallback_(),
	connectionCallback_(),
	messageCallback_(),
	writeCompleteCallback_(),
	started_(0),
	nextConnId_(1),
	connections_()
{
	acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection,this,std::placeholders::_1,std::placeholders::_2));
}

TcpServer::~TcpServer()
{
	for(auto& it:connections_)
	{
		TcpConnectionPtr conn=it.second;
		it.second.reset();		
		conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed,conn));
	}
}

void TcpServer::setThreadNum(int numThreads)
{
	numThreads_=numThreads;
	threadPool_->setThreadNum(numThreads);
}

void TcpServer::start()
{
	if(started_.fetch_add(1)==0)
	{
		threadPool_->start(threadInitCallback_);
		loop_->runInLoop(std::bind(&Acceptor::listen,acceptor_.get()));
	}
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
	//轮询subloop
	EventLoop* ioLoop = threadPool_->getNextLoop();	
	
	//命名
	char buf[64] = {0};
	snprintf(buf,sizeof(buf),"-%s#%d",ipPort_.c_str(),nextConnId_);
	++nextConnId_;
	std::string connName = name_+buf;

	//log
	
	//获取其绑定的本地的ip和port
	sockaddr_in local;
	::memset(&local,0,sizeof(local));
	socklen_t addrlen = sizeof(local);
	if(::getsockname(sockfd, (sockaddr*)&local, &addrlen)<0)
	{
		//log
	}
	InetAddress localAddr(local);

	//创建TcpConnection对象
	TcpConnectionPtr conn(new TcpConnection(ioLoop,connName,sockfd,localAddr,peerAddr));
	
	//加入映射表
	connections_[connName] = conn;

	//设置回调 4
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(std::bind(&TcpServer::removeConnection,this,std::placeholders::_1));

	//子线程中继续连接建立逻辑
	ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished,conn));

}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
	loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop,this,conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
	//log
	
	//从连接表中移除
	connections_.erase(conn->name());
	
	//获取相应loop
	EventLoop* ioLoop = conn->getLoop();

	//在相应的loop中进行连接销毁
	ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed,conn));

}
































