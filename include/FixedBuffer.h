//FixedBuffer.h

#pragma once

#include<string>
#include<string.h>

#include "noncopyable.h"

constexpr int smallBufferSize = 4096;
constexpr int largeBufferSize = 4096 * 1000;


template<int bufferSize>
class FixedBuffer : noncopyable
{
public:
        FixedBuffer():cur_(data_),length_(0){}
        ~FixedBuffer(){}

        void append(const char* buf, size_t len)
        {
                if(valid(len))
                {
                        memcpy(cur_,buf,len);
                        add(len);
                }
        }

        bool valid(size_t len) const {return bufferSize-length_ > len;}

        void add(size_t len) {cur_ += len; length_ += len;}
        void reset() {cur_=data_; length_=0;}

        const char* data() const {return data_;}
        char* current() {return cur_;}
        size_t length() const {return length_;}

        void bzero() {::bzero(data_,bufferSize);}

private:

        char data_[bufferSize];
        char* cur_;                     //data_可写的下一个位置
        size_t length_;                 //data_中有效数据的长度

};

