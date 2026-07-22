#include "dateTime_cont.h"
using namespace std;

/*
    函数名称：get_newDateTime
    函数功能：获取当前的时间
    传入参数：
                unsigned char *buff     生成的时间字符串存入该指针指向的存储空间
                int buffLen             存储空间的大小
    传出数据：
                 0      运行成功
                -1      参数有误
                -2      时间转换失败
    注意事项：
                时间格式：2016-10-11 15:03:08
                buff大小不得小于20
    编写人员：王凤龙
    编写时间：2016-10-11
*/
int get_newDateTime(unsigned char *buff, int buffLen){
    /*参数校验*/
    if(!buff || buffLen<20) return -0x01;

    /*获取当前时间*/
    time_t t = time(0x00);

    /*转换时间格式*/
    memset((void *)buff, 0x00, buffLen);
    if(strftime((char *)buff, buffLen, "%Y-%m-%d %H:%M:%S", localtime(&t)) <= 0x00) return -0x02;

    return 0x00;
}