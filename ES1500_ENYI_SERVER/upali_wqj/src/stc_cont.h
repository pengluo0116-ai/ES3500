#ifndef __STC_CONT_H__
#define __STC_CONT_H__

/*
    文档简介：
    连接STC串口，并获取实时数据
*/

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

#include "usart_read.h"

class stc_cont:public usart_read{
    public:
        stc_cont(const char *_ttl_path, int _nSpeed, char _nEvent, int _nPartity, int _nStop);              //构造函数
        ~stc_cont();                                                                                        //析构函数
    
    public:
        int stc_init();                                                                                     //初始化
};

#endif