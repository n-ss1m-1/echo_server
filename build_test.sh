#!/bin/bash
# build_test.sh

echo "=== Building Echo Server Test ==="

CXX=g++
CXXFLAGS="-std=c++11 -Wall -Wextra -g -O0 -pthread"
SOURCES="echo_server_test.cc"

# 编译所有 .cc 文件
for file in *.cc; do
    if [ "$file" != "echo_server_test.cc" ]; then
        SOURCES="$SOURCES $file"
    fi
done

echo "Compiling: $SOURCES"

# 编译
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
