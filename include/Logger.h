#pragma once

#include<string>
#include<string.h>
#include<errno.h>
#include<functional>
#include<pthread.h>

#include "LogStream.h"
#include "Timestamp.h"

#define OPEN_LOGGING


class SourceFile
{
public:
	explicit SourceFile(const char* fileName):data_(fileName)
	{
		const char* slash = strrchr(fileName,'/');
		if(slash)
		{
			data_ = slash + 1;
		}
		len_ = static_cast<int>(std::strlen(data_));
	}

	const char* data_;
	int len_;
};


class Logger
{
public:
	enum LogLevel
	{
		TRACE,
		DEBUG,
		INFO,
		WARM,
		ERROR,
		FATAL,
		LEVEL_COUNT,
       	};

	Logger(const char* fileName,int line,LogLevel level);
	~Logger();

	LogStream& stream() {return impl_.stream_;}

	using OutputFunc = std::function<void(const char* msg,int len)>;
	using FlushFunc = std::function<void()>;

	static OutputFunc g_output;
	static FlushFunc g_flush;

	static void setOutput(OutputFunc out) {g_output = std::move(out);}
	static void setFlush(FlushFunc flush) {g_flush = std::move(flush);}

private:
	class Impl
	{
	public:
		using LogLevel = Logger::LogLevel;

		Impl(LogLevel level,int savedErrno,const char* fileName,int line);

		void formatTime();

		void finish();

		Timestamp time_;
		LogStream stream_;
		LogLevel level_;
		int line_;
		SourceFile basename_;
	};

private:
	Impl impl_;
};


const char* getErrnoMsg(int savedErrno);


/**
 * 日志宏定义
 * 当日志等级小于对应等级才会输出
 * 例如：设置等级为FATAL，则DEBUG和INFO等级的日志就不会输出
 * 
 * 原理：
 * LOG_INFO 宏展开为：Logger(__FILE__, __LINE__, Logger::INFO).stream()
 * 1. 创建一个临时Logger对象
 * 2. 调用stream()获取LogStream引用
 * 3. 通过<<操作符写入日志内容
 * 4. Logger对象析构，输出完整日志
 */
#ifdef OPEN_LOGGING
    // 各种级别的日志宏
    #define LOG_DEBUG Logger(__FILE__, __LINE__, Logger::DEBUG).stream()        //预定义的宏：__FILE__代表当前运行的文件名  __LINE__代表当前运行的行号
    #define LOG_INFO Logger(__FILE__, __LINE__, Logger::INFO).stream()
    #define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
    #define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
    #define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
#else
    // 关闭日志时，返回一个空的LogStream，避免日志输出
    #define LOG(level) LogStream()
#endif
	




