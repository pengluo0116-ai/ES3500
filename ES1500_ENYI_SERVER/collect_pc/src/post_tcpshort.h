#ifndef __POST_TCPSHORT_H__
#define __POST_TCPSHORT_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

#include "post_ports.h"
#include "tcp_sock_client.h"

#define POST_TCPSHORT_RCVEMAX 2048

class post_tcpshort:public post_ports{
    // private:
        // unsigned char *recv_dataBuff;              //从服务器接收到的数据

    public:
        post_tcpshort(const char *_ip, const int _port);
        ~post_tcpshort();

    private:
        int init();
        int sendData();
        // int recvData();
};

#endif