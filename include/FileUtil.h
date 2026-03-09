#pragma once

#include<string>
#include<stdio.h>
#include<sys/types.h>

class FileUtil
{
public:
	FileUtil(std::string& fileName);
	~FileUtil();

	void append(const char* data,size_t len);

	void flush();

	off_t writtenBytes() const {return writtenBytes_;}

private:
	size_t write(const char* data,size_t len);

	FILE* file_;
	char buffer_[64*1024];
	off_t writtenBytes_;
};







