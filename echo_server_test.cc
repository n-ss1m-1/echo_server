// echo_server_test.cc
#include "TcpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Logger.h"
#include "Buffer.h"

#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <chrono>
#include <iomanip>

// 简单日志宏（如果没有实现Logger.h）
#ifndef LOG_INFO
#define LOG_INFO(msg) std::cout << "[INFO] " << msg << std::endl
#endif

#ifndef LOG_ERROR
#define LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << std::endl
#endif

// Echo服务器类
class EchoServer {
public:
    EchoServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& name)
        : server_(loop, listenAddr, name)
    {
        // 设置连接建立回调
        server_.setConnectionCallback(
            std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
        
        // 设置消息回调
        server_.setMessageCallback(
            std::bind(&EchoServer::onMessage, this, 
                     std::placeholders::_1, std::placeholders::_2));
        
        // 设置线程数（0表示单线程，4表示4个IO线程）
        server_.setThreadNum(4);
    }
    
    void start() {
        LOG_INFO("Starting EchoServer on " + server_.ipPort());
        server_.start();
    }
    
private:
    void onConnection(const TcpConnectionPtr& conn) {
        if (conn->connected()) {
            LOG_INFO("New connection: " + conn->name() + 
                     " from " + conn->peerAddress().toIpPort());
            
            // 发送欢迎消息
            std::string welcome = "Welcome to Echo Server!\r\n";
            welcome += "Type 'quit' to exit, 'stats' for server info.\r\n";
            conn->send(welcome);
            
            connectionCount_++;
            updateStats();
        } else {
            LOG_INFO("Connection closed: " + conn->name());
            connectionCount_--;
            updateStats();
        }
    }
    
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf) {
        std::string msg = buf->retrieveAllAsString();
        messageCount_++;
        
        // 记录消息
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        char timeStr[100];
        std::strftime(timeStr, sizeof(timeStr), "%H:%M:%S", std::localtime(&in_time_t));
        
        LOG_INFO("[" + std::string(timeStr) + "] From " + conn->name() + 
                 ": " + std::to_string(msg.size()) + " bytes");
        
        // 特殊命令处理
        if (msg.find("quit") != std::string::npos) {
            conn->send("Goodbye!\r\n");
            conn->shutdown();
        } else if (msg.find("stats") != std::string::npos) {
            std::string stats = "Server Statistics:\r\n";
            stats += "  Active connections: " + std::to_string(connectionCount_) + "\r\n";
            stats += "  Total messages: " + std::to_string(messageCount_) + "\r\n";
            stats += "  Server uptime: " + std::to_string(getUptime()) + " seconds\r\n";
            conn->send(stats);
        } else {
            // Echo 回显
            conn->send("Echo: " + msg);
            
            // 测试大数据传输
            if (msg.find("big") != std::string::npos) {
                std::string bigData(10000, 'X');  // 10KB 数据
                conn->send("Big data test: " + bigData);
            }
        }
    }
    
    void updateStats() {
        LOG_INFO("Active connections: " + std::to_string(connectionCount_));
    }
    
    int getUptime() const {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(
            now - startTime_).count();
    }
    
    TcpServer server_;
    std::atomic<int> connectionCount_{0};
    std::atomic<int> messageCount_{0};
    std::chrono::steady_clock::time_point startTime_ = std::chrono::steady_clock::now();
};
