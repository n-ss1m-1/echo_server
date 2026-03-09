#include "Logger.h"
#include "CurrentThread.h"

namespace ThreadInfo
{
	thread_local char t_errnobuf[512];
	thread_local char t_timer[64];
	thread_local time_t t_lastSecond;
}

const char* getErrnoMsg(int savedErrno)
{
        return strerror_r(savedErrno, ThreadInfo::t_errnobuf,sizeof(ThreadInfo::t_errnobuf));
}

const char* getLevelName[Logger::LogLevel::LEVEL_COUNT]{
	"TRACE ",
	"DEBUG ",
	"INFO  ",
	"WARM  ",
	"ERROR ",
	"FATAL ",
};

static void defaultOutput(const char* data, int len)
{
	fwrite(data,len,sizeof(char),stdout);
}

static void defaultFlush()
{
	fflush(stdout);
}

Logger::OutputFunc Logger::g_output = defaultOutput;
Logger::FlushFunc Logger::g_flush = defaultFlush;


Logger::Impl::Impl(LogLevel level,int savedErrno,const char* fileName,int line):
	time_(Timestamp::now()),
	stream_(),
	level_(level),
	line_(line),
	basename_(fileName)
{
	formatTime();
	stream_ << GeneralTemplate(getLevelName[level_],6);		//输出日志级别标签

	if(savedErrno != 0)
	{
		stream_ << getErrnoMsg(savedErrno) << " (errno=" << savedErrno << ") ";
	}
}

void Logger::Impl::formatTime()
{
	Timestamp now = Timestamp::now();

	time_t seconds = static_cast<time_t>(now.microSecondsSinceEpoch() / Timestamp::kMicroSecondsPerSecond);
	int microseconds = static_cast<int>(now.microSecondsSinceEpoch() % Timestamp::kMicroSecondsPerSecond);

	//将秒数转换为本地时间结构体
	struct tm* tm_timer = localtime(&seconds);

	//格式 YYYY/MM/DD HH:MM:SS
	snprintf(ThreadInfo::t_timer,sizeof(ThreadInfo::t_timer),
			"%4d/%02d/%02d %02d:%02d:%02d",
			tm_timer->tm_year + 1900,
			tm_timer->tm_mon + 1,
			tm_timer->tm_mday,
			tm_timer->tm_hour,
			tm_timer->tm_min,
			tm_timer->tm_sec);

	ThreadInfo::t_lastSecond = seconds;

	//微秒部分：6位微秒+1位空格
	char microTime[32] = {0};
	snprintf(microTime, sizeof(microTime), "%06d ", microseconds);

	stream_ << GeneralTemplate(ThreadInfo::t_timer,17) << GeneralTemplate(microTime,7);
}


void Logger::Impl::finish()
{
	stream_ << " - "
		<< GeneralTemplate(basename_.data_,basename_.len_)
		<< ":" << line_
		<< '\n' ;
}

Logger::Logger(const char* fileName,int line,LogLevel level):
	impl_(level,0,fileName,line)
{
}

Logger::~Logger()
{
	impl_.finish();

	const LogStream::Buffer& buffer = stream().buffer();

	g_output(buffer.data(),buffer.length());

	if(impl_.level_ == FATAL)
	{
		g_flush();
		abort();
	}
}




