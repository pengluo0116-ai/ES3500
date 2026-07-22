#ifndef __USART_READ_H__
#define __USART_READ_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

class usart_read{
    private:
        int fd;                         //串口文件描述符
        std::string ttl_path;           //串口文件路径
        int nSpeed;                     //波特率
        char nEvent;                    //奇偶校验位
        int nPartity;                   //数据位
        int nStop;                      //停止位

    public:
        usart_read(const char *_ttl_path, int _nSpeed, char _nEvent, int _nPartity, int _nStop);
        ~usart_read();

    private:
        int check();                                                                                    //校验参数
        int sopen();                                                                                     //打开串口
        int setUsart();                                                                                 //设置串口数据

    public:
        int init();                                                                                     //初始化
        int getData(unsigned char *dataBuff, int len);                                                  //读取数据
        void uclose();                                                                                  //关闭串口
        int writeData(unsigned char *dataBuff, int len);                                                //写数据
};


#endif