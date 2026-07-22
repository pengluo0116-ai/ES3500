#include "cpp_datetimer.h"
using namespace std;

/*
    函数名称：stringToDateTime
    函数功能：时间字符串转换成时间(秒数)
    传入参数：
                const char *_dateTimeStr    时间字符串
                time_t *_dateTime           转换成的时间存储到该指针指向的存储空间
    传出数据：
                 0  运行成功
                -1  参数校验有误
                -2  时间字符串格式有误，无法提取有效数据
    注意事项：字符串时间格式 “2017-01-01 12:00:00”
    编写人员：王凤龙
    编写时间：2017-05-19
*/
int cpp_datetimer::stringToDatetime(const char *_dateTimeStr, time_t *_dateTime){
    struct tm dateTime;

    /*参数校验*/
    if(!_dateTimeStr || strlen(_dateTimeStr)<=0x00 || !_dateTime) return -0x01;

    /*将字符串转换成整数*/
    if(!strptime(_dateTimeStr, "%Y-%m-%d %H:%M:%S", &dateTime)) return -0x02;

    /*转换格式*/
    *_dateTime = mktime(&dateTime);

    return 0x00;
}

/*
    函数名称：stringToDatetime_sec
    函数功能：日期时间转换秒数
    传入参数：
                _dateTimeStr        日期时间字符串
                unsigned long *_dateTime_sec    转换后的秒数存储到该指针指向的存储空间
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-20
*/
int cpp_datetimer::stringToDatetime_sec(const char *_dateTimeStr, unsigned long *_dateTime_sec){
    *_dateTime_sec = 0x0;
    return cpp_datetimer::stringToDatetime(_dateTimeStr, (time_t *)_dateTime_sec);
}

/*
    函数名称：dateTimeToString
    函数功能：日期时间转换成字符串
    传入参数：
                const time_t _dateTime          当前的时间--秒数
                char *_dateTimeStr              生成的时间字符串存储到该指针执行的存储空间
                unsinged int _dateTimeStr_len   存储时间字符串的指针指向的存储空间大小
    传出数据：
                 0  运行正确
                -1  参数校验有误
                -2  时间转换有误--一般为缓冲区大小
    注意事项：转换成的字符串格式 “2017-01-01 12:00:00”
    编写人员：王凤龙
    编写时间：2017-05-20
*/
int cpp_datetimer::dateTimeToString(const time_t _dateTime, char *_dateTimeStr, unsigned int _dateTimeStr_len){
    /*参数校验*/
    if(!_dateTimeStr || _dateTimeStr_len<=0x00) return -0x01;
    
    /*转换为本地时间*/
    tm *local = localtime(&_dateTime);
    
    /*转换为时间字符串*/
    memset((void *)_dateTimeStr, 0x00, _dateTimeStr_len);
    if(strftime(_dateTimeStr, _dateTimeStr_len, "%Y-%m-%d %H:%M:%S", local) <= 0x00) return -0x02;

    return 0x00;
}

/*
    函数名称：dateTimeToString
    函数功能：日期时间转换成字符串
    传入参数：
                const time_t _dateTime
                std::string &_dateTimeStr
    传出数据：0 运行成功 非0 运行失败
    注意事项：
                该函数生成的字符串为c++字符串
                转换成的字符串格式 “2017-01-01 12:00:00”
    编写人员：王凤龙
    编写时间：2017-05-20
*/
int cpp_datetimer::dateTimeToString(const time_t _dateTime, std::string &_dateTimeStr){
    char tmp[32] = { 0x00 };
    _dateTimeStr = "";

    /*转换成时间字符串*/
    if(int err = cpp_datetimer::dateTimeToString(_dateTime, tmp, 32)) return err;

    /*生成字符串*/
    _dateTimeStr = tmp;
    return 0x00;
}

/*
    函数名称：now_sec
    函数功能：获取当前日期时间的秒数
    传入参数：unsigned long *_dateTime_sec   生成的时间秒数存储到该指针指向的存储空间
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-20
*/
int cpp_datetimer::now_sec(unsigned long *_dateTime_sec){
    *_dateTime_sec = (unsigned long)time(NULL);
    return 0x00;
}

/*
    函数名称：nowToStirng
    函数功能：当前时间转换成C风格字符串
    传入参数：
                char *_dateTime,                    生成的时间字符串存储到该指针指向的存储空间 
                unsigned int _dateTimeStr_len       缓冲区尺寸
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-26
*/
int cpp_datetimer::nowToString(char *_dateTime, unsigned int _dateTimeStr_len){
    time_t t = time(NULL);
    return cpp_datetimer::dateTimeToString(t, _dateTime, _dateTimeStr_len);
}

/*
    函数名称：nowToString 
    函数功能：当前时间生成字符串
    传入参数：std::string &_dateTime 生成的时间字符串存储到该引用指向的存储空间
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-26
*/
int cpp_datetimer::nowToString(std::string &_dateTime){
    time_t t = time(NULL);
    return cpp_datetimer::dateTimeToString(t, _dateTime);
}