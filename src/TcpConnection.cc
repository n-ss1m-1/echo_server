#include<functional>
#include<string>
#include<cstring>
#include<errno.h>
#include<sys/types.h>		//系统类型定义
#include<sys/socket.h>		//socket API
#include<netinet/tcp.h>		//TCP选项
#include<sys/sendfile.h>	//sendfile系统调用
#include<fcntl.h>		//文件控制选项
#include<unistd.h>		//read write close等

#include "TcpConnection.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Socket.h"
#include "Logger.h"


static EventLoop* CheckLoopNotNull(EventLoop* loop)
{
	if(loop==nullptr)
	{
		LOG_FATAL<<" mainLoop is null!";	
	}
	return loop;
}

TcpConnection::TcpConnection(EventLoop* loop,const std::string& nameArg,int sockfd,const InetAddress& localAddr,const InetAddress& peerAddr)
	:loop_(loop),
	 name_(nameArg),
	 state_(KConnecting),
	 reading_(true),
	 socket_(new Socket(sockfd)),
	 channel_(new Channel(loop,sockfd)),
	 localAddr_(localAddr),
	 peerAddr_(peerAddr),
	 highWaterMark_(64*1024*1024)	//64M
{
	channel_->setReadCallback(std::bind(&TcpConnection::handleRead,this,std::placeholders::_1));
	channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite,this));
	channel_->setErrorCallback(std::bind(&TcpConnection::handleError,this));
	channel_->setCloseCallback(std::bind(&TcpConnection::handleClose,this));

	LOG_INFO<<"TcpConnection::ctor:["<<name_.c_str()<<"]at fd="<<sockfd;
	
	socket_->setKeepAlive(true);
}
TcpConnection::~TcpConnection()
{
	LOG_INFO<<"TcpConnection::dtor["<<name_.c_str()<<"]at fd="<<channel_->fd()<<"state="<<(int)state_;
}
void TcpConnection::connectEstablished()
{
	setState(KConnected);
	channel_->tie(shared_from_this());
	channel_->enableReading();

	connectionCallback_(shared_from_this());
}
void TcpConnection::connectDestroyed()
{
	if(state_==KConnected)
	{
		setState(KDisconnected);
		channel_->disableAll();
		connectionCallback_(shared_from_this());
	}
	channel_->remove();
}
void TcpConnection::handleRead(std::chrono::steady_clock::time_point receiveTime)
{
	int saveErrno=0;
	ssize_t n = inputBuffer_.readFd(channel_->fd(),&saveErrno);
	
	if(n>0)
	{
		messageCallback_(shared_from_this(),&inputBuffer_,receiveTime);
	}
	else if(n==0)
	{
		handleClose();
	}
	else
	{
		errno=saveErrno;
		LOG_ERROR<<"TcpConnection::handleRead";
		handleError();
	}	
}
void TcpConnection::handleClose()
{
	LOG_INFO<<"TcpConnection::handleClose fd="<<channel_->fd()<<"state="<<(int)state_;
	setState(KDisconnected);
	channel_->disableAll();
	
	TcpConnectionPtr connPtr(shared_from_this());
	connectionCallback_(connPtr);
	closeCallback_(connPtr);
}
void TcpConnection::handleError()
{
	int optval;
	socklen_t optlen=sizeof(optval);
	int err=0;

	if(::getsockopt(channel_->fd(),SOL_SOCKET,SO_ERROR,&optval,&optlen)<0)
	{
		err=errno;	//查询失败 查询失败的原因保存在errno中
	}
	else
	{
		err=optval;	//查询成功 错误值保存在optval中
	}

	LOG_ERROR<<"TcpConnection::handleError name:"<<name_.c_str()<<"- SO_ERROR:%"<<err;

	handleClose();
}
void TcpConnection::handleWrite()
{
	if(channel_->isWriting())
	{
		int saveErrno = 0;
		int n= outputBuffer_.writeFd(channel_->fd(),&saveErrno);
		if(n>0)
		{
			outputBuffer_.retrieve(n);
			if(outputBuffer_.readableBytes()==0)
			{
				channel_->disableWriting();
				if(writeCompleteCallback_)
				{
					loop_->queueInLoop(std::bind(writeCompleteCallback_,shared_from_this()));
				}
			
				if(state_ == KDisconnecting)
				{
					shutdownInLoop();
				}
			}
		}
		else
		{
			LOG_ERROR<<"TcpConnection::handleWrite";
		
		}	
	}
	else
	{
		LOG_ERROR<<"TcpConnection fd="<<channel_->fd()<<"is down, no more writing";

	}
}
void TcpConnection::send(const std::string& buf)
{
	if(state_==KConnected)
	{
		if(loop_->isInLoopThread())	sendInLoop(buf.c_str(),buf.size());
		else	loop_->runInLoop(std::bind(&TcpConnection::sendInLoop,this,buf.c_str(),buf.size()));
	}
}
void TcpConnection::sendInLoop(const void* data,size_t len)
{
	ssize_t nwrote = 0;
	size_t remaining = len;
	bool faultError = false;

	if(state_ == KDisconnected)
	{
		LOG_ERROR<<"disconnected, give up writing";

	}

	if(!channel_->isWriting() && outputBuffer_.readableBytes()==0)
	{
		nwrote = ::write(channel_->fd(),data,len);
		if(nwrote>=0)
		{
			remaining = len-nwrote;
			if(remaining==0 && writeCompleteCallback_)
			{
				loop_->queueInLoop(std::bind(&TcpConnection::writeCompleteCallback_,shared_from_this()));
			}
		}
		else
		{
			nwrote = 0;
			if(errno !=EWOULDBLOCK)
			{
				LOG_ERROR<<"TcpConnection::sendInLoop";
				if(errno == EPIPE || errno ==ECONNRESET)
				{
					faultError = true;
				}
			}
		}
	}

	if(!faultError && remaining>0)
	{
		size_t oldLen = outputBuffer_.readableBytes();
		if(oldLen+remaining>=highWaterMark_ && oldLen<highWaterMark_ && highWaterMarkCallback_)
		{
			loop_->queueInLoop(std::bind(highWaterMarkCallback_,shared_from_this(),oldLen+remaining));
		}

		outputBuffer_.append((char*)data+nwrote,remaining);
		if(!channel_->isWriting())
		{
			channel_->enableWriting();
		}
	}
}
void TcpConnection::sendFile(int fileDescripto,off_t offset,size_t count)
{

}
void TcpConnection::sendFileInLoop(int fileDescripto,off_t offset,size_t count)
{


}
void TcpConnection::shutdown()
{
	if(state_==KConnected)
	{
		setState(KDisconnecting);
		loop_->queueInLoop(std::bind(&TcpConnection::shutdownInLoop,this));
	}

}
void TcpConnection::shutdownInLoop()
{
	if(!channel_->isWriting())	
	{		
		socket_->shutdownWrite();	
	}
}






