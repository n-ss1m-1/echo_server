#!/bin/bash
# build_test.sh
# 编译Echo服务器测试程序

echo "=== Building Echo Server Test ==="

# 设置编译器
CXX=g++
CXXFLAGS="-std=c++11 -g -O0 -Wall -Wextra -pthread"

# 源文件列表
SOURCES="
    echo_server_test.cc
    Acceptor.cc
    Channel.cc
    EPollPoller.cc
    EventLoop.cc
    EventLoopThread.cc
    EventLoopThreadPool.cc
    Poller.cc
    TcpConnection.cc
    TcpServer.cc
    Buffer.cc
    InetAddress.cc
    Socket.cc
    Thread.cc
    CurrentThread.cc
"

# 编译命令
$CXX $CXXFLAGS -o echo_server_test $SOURCES

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo ""
    echo "Run the server:"
    echo "  ./echo_server_test"
    echo ""
    echo "Then connect with telnet:"
    echo "  telnet localhost 9876"
else
    echo "❌ Build failed!"
    exit 1
fi
