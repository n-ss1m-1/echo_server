#pragma once

#include<mutex>
#include<memory>
#include<ctime>

#include "FileUtil.h"

class LogFile
{
public:
	LogFile(const std::string& basename,off_t rollsize,int flushInterval = 3,int checkEveryN_=1024);
       ~LogFile();

	void append(const char* data,int len);
	void flush();
	bool rollFile();

private:
	static std::string getLogFileName(const std::string& basename, time_t* now);
 	void appendInLock(const char* data,int len);	

	const std::string basename_;
	const off_t rollsize_;
	const int flushInterval_;
	const int checkEveryN_;

	int count_;

	std::mutex mutex_;
	time_t startOfPeriod_;
	time_t lastRoll_;
	time_t lastFlush_;
	std::unique_ptr<FileUtil> file_;
	const static int KRollPerSeconds_ = 24*60*60;
};




















