// echo_server_test.cc
#include "TcpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include <iostream>

int main() {
    std::cout << "=== Echo Server Test ===" << std::endl;
    
    // 创建 EventLoop
    EventLoop loop;
    
    // 监听地址
    InetAddress listenAddr(9876);
    
    // 创建 TcpServer
    TcpServer server(&loop, listenAddr, "EchoServer");
    
    // 设置回调
    server.setConnectionCallback([](const TcpConnectionPtr& conn) {
        if (conn->connected()) {
            std::cout << "New connection: " << conn->name() << std::endl;
            conn->send("Welcome to Echo Server!\n");
        } else {
            std::cout << "Connection closed: " << conn->name() << std::endl;
        }
    });
    
    server.setMessageCallback([](const TcpConnectionPtr& conn, Buffer* buf, 
                                 std::chrono::steady_clock::time_point) {
        std::string msg = buf->retrieveAllAsString();
        std::cout << "Received: " << msg;
        conn->send("Echo: " + msg);
    });
    
    // 启动服务器
    std::cout << "Starting server on port 9876..." << std::endl;
    server.setThreadNum(4);
    server.start();
    
    // 运行事件循环
    loop.loop();
    
    return 0;
}
