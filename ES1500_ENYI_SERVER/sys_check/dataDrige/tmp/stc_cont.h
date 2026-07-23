#ifndef __STC_CONT_H__
#define __STC_CONT_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

#include "usart_read.h"
#include "conf.h"
#include "crc8.h"


/*数据包结构体*/
struct stc_package{
    unsigned char crc8;
    unsigned char len;
    unsigned char cmd;
    unsigned char data[STC_UART_PKGDATABUFF_MAXSIZE];
};

class stc_cont:public usart_read{
    private:
        unsigned short getFlag;                             //是否获取到报文头,数据长度          
        int getPoint;                                       //接收到的数据长度
        unsigned char *dataBuff;                            //接收数据临时缓冲区

    public:
        stc_cont(const char *_ttl_path, int _nSpeed, char _nEvent, int _nPartity, int _nStop);              //构造函数
        ~stc_cont();                                                                                        //析构函数
    
    public:
        int stc_init();                                                                                     //初始化
        int stc_getpkg(struct stc_package *pkg);                                                             //获取一包数据
};

#endif