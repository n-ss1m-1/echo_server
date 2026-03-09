#include "LogStream.h"

static const char digits[] = "9876543210123456789";

template<typename T>
void LogStream::formatInteger(T num)
{
	if(buffer_.valid(maxNumSize))
	{
		char* start = buffer_.current();
		char* cur = start;
		static const char* zero = digits + 9;
		bool negative = ((num<0)?true:false);

		while(num!=0)
		{
			int remainder = static_cast<int>(num%10);
			num /= 10;
			*cur=zero[remainder];
			cur++;
		}
		if(negative)
		{
			*cur='-';
			cur++;
		}
		*cur = '\0';

		std::reverse(start,cur);
		int length = static_cast<int>(cur-start);	
		buffer_.add(length);
	}
}

LogStream& LogStream::operator<<(bool express)
{
	buffer_.append(express?"true":"false",express?4:5);
	return *this;
}

LogStream& LogStream::operator<<(int number)
{
	formatInteger(number);
	return *this;
}
LogStream& LogStream::operator<<(unsigned int number)
{
	formatInteger(number);
	return *this;
}
LogStream& LogStream::operator<<(short number)
{
	formatInteger(number);
	return *this;
}
LogStream& LogStream::operator<<(unsigned short number)
{
	formatInteger(number);
	return *this;
}
LogStream& LogStream::operator<<(long number)
{
	formatInteger(number);
	return *this;
}
LogStream& LogStream::operator<<(unsigned long number)
{
	formatInteger(number);
	return *this;
}
LogStream& LogStream::operator<<(long long number)
{
	formatInteger(number);
	return *this;
}
LogStream& LogStream::operator<<(unsigned long long number)
{
	formatInteger(number);
	return *this;
}

LogStream& LogStream::operator<<(float number)
{
	*this<<static_cast<double>(number);
	return *this;
}
LogStream& LogStream::operator<<(double number)
{
	char buffer[32];
	snprintf(buffer,sizeof(buffer),"%.12g",number);
	buffer_.append(buffer,std::strlen(buffer));
	return *this;
}

LogStream& LogStream::operator<<(char c)
{
	buffer_.append(&c,1);
	return *this;
}
LogStream& LogStream::operator<<(const char* str)
{
	buffer_.append(str,std::strlen(str));
	return *this;
}
LogStream& LogStream::operator<<(const unsigned char* str)
{
	buffer_.append(reinterpret_cast<const char*>(str),std::strlen(reinterpret_cast<const char*>(str)));
	return *this;
}
LogStream& LogStream::operator<<(const std::string& str)
{
	buffer_.append(str.c_str(),str.size());
	return *this;
}
LogStream& LogStream::operator<<(const GeneralTemplate& g)
{
	buffer_.append(g.data_,g.len_);
	return *this;
}




