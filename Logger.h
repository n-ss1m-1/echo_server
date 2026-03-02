// Logger.h
#pragma once

#include <iostream>
#include <string>
#include <sstream>

// 简单的日志宏
#define LOG_INFO(msg) std::cout << "[INFO] " << msg << std::endl
#define LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << std::endl
#define LOG_FATAL(msg) std::cerr << "[FATAL] " << msg << std::endl; exit(1)
#define LOG_DEBUG(msg) std::cout << "[DEBUG] " << msg << std::endl
#define LOG_SYSERR(msg) std::cerr << "[SYSERR] " << msg << ": " << strerror(errno) << std::endl
