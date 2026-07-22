#ifndef __POST_PORTS_H__
#define __POST_PORTS_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

#include "lift_vector.h"

class post_ports{
    protected:
        std::string ip;             //服务器IP
        int port;                   //服务器端口
        std::string send_dataBuff;  //发送数据

    public:
        post_ports(const char *_ip, const int _port);
        virtual ~post_ports();

    private:
        int getData_fromVct();

    protected:
        virtual int init();                         //初始化
        virtual int sendData();                     //发送数据
        virtual int recvData();

    public:
        int work();                                  //运行函数
};

#endif