#ifndef __POST_TCPLONG_H__
#define __POST_TCPLONG_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

#include "post_ports.h"
#include "tcp_sock_client.h"

#define POST_TCPLONG_RCVEMAX 2048

class post_tcplong:public post_ports{
    private:
        unsigned char *recv_dataBuff;       //从服务器接收到的数据
        tcp_sock_client *tcp_client;

    public:
        post_tcplong(const char *_ip, const int _port);
        ~post_tcplong();

    private:
        int init();
        int sendData();
        int recvData();
};


#endif