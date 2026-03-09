#include "LogFile.h"


LogFile::LogFile(const std::string& basename,off_t rollsize,int flushInterval,int checkEveryN):
	basename_(basename),
	rollsize_(rollsize),
	flushInterval_(flushInterval),
	checkEveryN_(checkEveryN),
	count_(0),
	startOfPeriod_(0),
	lastRoll_(0),
	lastFlush_(0)
{
	//构建LogUtil对象 创建一个新的日志文件
	rollFile();
}

LogFile::~LogFile() = default;


void LogFile::append(const char* data,int len)
{
	std::lock_guard<std::mutex> lock(mutex_);
	appendInLock(data,len);
}

void LogFile::appendInLock(const char* data,int len)
{
	file_->append(data,len);

	time_t now = time(NULL);
	++count_;

	if(file_->writtenBytes() > rollsize_)
	{
		rollFile();
	}
	else if(count_ >=checkEveryN_)
	{
		count_ = 0;

		time_t thisPeriod = now / KRollPerSeconds_ * KRollPerSeconds_;
		if(thisPeriod != startOfPeriod_)
		{
			rollFile();
		}
	}

	if(now - lastFlush_ > flushInterval_)
	{
		lastFlush_ = now;
		flush();
	}

}

void LogFile::flush()
{
	file_->flush();
}

bool LogFile::rollFile()
{
	time_t now = 0;
	std::string filename = getLogFileName(basename_,&now);
	time_t start = now / KRollPerSeconds_ * KRollPerSeconds_;
	if(now > lastRoll_)
	{
		lastFlush_ = now;
		lastRoll_ = now;
		startOfPeriod_ = start;
		file_.reset(new FileUtil(filename));	//释放原来的堆->原FileUtil析构->fflush()+close() 然后此处新创建了一个文件
		return true;
	}
	return false;	
}

//basename_ + now + ".log"
std::string LogFile::getLogFileName(const std::string& basename, time_t* now)
{
	std::string filename;
	filename.reserve(basename.size()+64);
	filename = basename;

	char timebuf[32];
	struct tm tm;
	*now = time(NULL);
	localtime_r(now,&tm);	//输入事件戳 输出分解时间
	strftime(timebuf,sizeof(timebuf),".%Y%m%d-%H%M%S",&tm);	//将分解时间格式化
								
	filename += timebuf;
	filename += ".log";

	return filename;
}










