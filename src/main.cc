#include<string>
#include<iostream>
#include<sstream>
#include<unistd.h>
#include<sys/stat.h>
#include<libgen.h>

#include "TcpServer.h"
#include "Logger.h"
#include "AsyncLogging.h"

static const off_t RollSize = 1*1024*1024;	//1MB

class EchoServer
{
public:
	EchoServer(EventLoop* loop, const InetAddress& localAddr, const std::string& name):
		server_(loop,localAddr,name),
		loop_(loop)
	{
		server_.setConnectionCallback(std::bind(&EchoServer::onConnection,this,std::placeholders::_1));
		server_.setMessageCallback(std::bind(&EchoServer::onMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
		server_.setThreadNum(3);
	}

	void start()
	{
		server_.start();
	}

private:	
	void onConnection(const TcpConnectionPtr& conn)
	{
		if(conn->connected())
		{
			LOG_INFO<<"Connection UP :"<<conn->peerAddress().toIpPort().c_str();
		}
		else
		{
			LOG_INFO<<"Connection DOWN :"<<conn->peerAddress().toIpPort().c_str();
		}
	}

	void onMessage(const TcpConnectionPtr& conn,Buffer* buf, std::chrono::steady_clock::time_point receiveTime)
	{
		std::string msg = buf->retrieveAllAsString();
		std::string http_resp =
        	"HTTP/1.1 200 OK\r\n"
        	"Content-Length: 12\r\n"
        	"Connection: keep-alive\r\n"  
        	"\r\n"
		+msg;
    		//conn->send(http_resp);
		conn->send(msg);	
	}

	TcpServer server_;
	EventLoop* loop_;

};

AsyncLogging* g_asyncLog = nullptr;
AsyncLogging* getAsyncLog() {return g_asyncLog;}

void asyncLog(const char* data,int len)
{
	AsyncLogging* logging = getAsyncLog();
	if(logging)
	{
		logging->append(data,len);
	}
}

int main(int argc,char* argv[])
{
	//启动日志
	const std::string LogDir = "logs";
	mkdir(LogDir.c_str(),0755);
	std::ostringstream LogfilePath;
	LogfilePath << LogDir << "/" << ::basename(argv[0]);
	AsyncLogging log(LogfilePath.str(),RollSize);
	g_asyncLog = &log;
	Logger::setOutput(asyncLog);
	log.start();
	
	EventLoop baseLoop;
	InetAddress localAddr(8080);
	EchoServer server(&baseLoop, localAddr, "EchoServer");
	server.start();

	std::cout<<"===================Start Web Server======================"<<std::endl;
	baseLoop.loop();
	std::cout<<"===================Stop  Web Server======================"<<std::endl;


	return 0;
}




