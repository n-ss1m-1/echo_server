#!/bin/bash
# build_test.sh

echo "=== Building Echo Server ==="

CXX=g++
CXXFLAGS="-std=c++11 -Wall -Wextra -g -O0 -pthread"
SOURCES="main.cc"

# 编译所有 .cc 文件
for file in *.cc; do
    if [ "$file" != "main.cc" ]; then
        SOURCES="$SOURCES $file"
    fi
done

echo "Compiling: $SOURCES"

# 编译
$CXX $CXXFLAGS -o main $SOURCES

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo ""
    echo "Run the server:"
    echo "  ./main"
    echo ""
    echo "Then connect with telnet:"
    echo "  telnet localhost 8080"
else
    echo "❌ Build failed!"
    exit 1
fi
