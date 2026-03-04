#include<string>
#include<iostream>

#include "TcpServer.h"


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
			//log
		}
		else
		{
			//log
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
        	"Hello Muduo!";
    		conn->send(http_resp);
		//conn->send(msg);	
	}

	TcpServer server_;
	EventLoop* loop_;

};

int main(int argc,char* argv[])
{
	
	EventLoop baseLoop;
	InetAddress localAddr(8080);
	EchoServer server(&baseLoop, localAddr, "EchoServer");
	server.start();

	std::cout<<"===================Start Web Server======================"<<std::endl;
	baseLoop.loop();
	std::cout<<"===================Stop  Web Server======================"<<std::endl;


	return 0;
}




