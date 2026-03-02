#pragma once

#include<memory>
#include<functional>
#include<chrono>

class TcpConnection;
class Buffer;

//TcpConnection的share智能指针
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
//连接状态变化回调(通知上层应用)
using ConnectionCallback = std::function<void(const TcpConnection &)>;
//连接关闭回调
using CloseCallback = std::function<void(const TcpConnection &)>;
//接受到客户数据后的回调---业务逻辑操作(此处为回响)
using MessageCallback = std::function<void(const TcpConnection &, Buffer*, std::chrono::steady_clock::time_point)>;
//数据发送完成的回调(通知上层) | 未实现
using WriteCompleteCallback = std::function<void(const TcpConnection &)>;
//流量控制回调 | 未实现
using HighWaterMarkCallback = std::function<void(const TcpConnection &, size_t)>;

