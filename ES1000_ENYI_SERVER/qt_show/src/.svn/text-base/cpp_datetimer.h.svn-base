#ifndef __CPP_DATETIMER_H__
#define __CPP_DATETIMER_H__

/*
    文档简介：仅适用于linux/unix c++
    时间与字符串之间的转换
    参考网址：http://blog.csdn.net/educast/article/details/17239735
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <time.h>

class cpp_datetimer{
    public:
    static int stringToDatetime(const char *_dateTimeStr, time_t *_dateTime);                                       //时间字符串转换成秒数--timt_t类型
    static int stringToDatetime_sec(const char *_dateTimeStr, unsigned long *_dateTime_sec);                        //时间字符串转换成秒数--long类型
    static int dateTimeToString(const time_t _dateTime, char *_dateTimeStr, unsigned int _dateTimeStr_len);         //日期时间转换成字符串--C风格字符串
    static int dateTimeToString(const time_t _dateTime, std::string &_dateTimeStr);                                 //日期时间转换成字符串--CPP风格字符串
    
    static int now_sec(unsigned long *_dateTime_sec);
    static int nowToString(char *_dateTime, unsigned int _dateTimeStr_len);                                         //当前日期时间转换成字符串--C风格字符串
    static int nowToString(std::string &_dateTime);                                                                 //当前日期时间转换成字符串--CPP风格字符串
};

#endif
