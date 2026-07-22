#ifndef __UDPRADIO_H__
#define __UDPRADIO_H__
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include <cstring>

class udp_radio{
    private:
        struct sockaddr_in addrto;
        int addrto_len;
        int sock_client;                //socket文件描述符
        int opt;                        //广播类型 
        
    public:
        udp_radio(const char *Bcastaddr, int port);
        ~udp_radio();
        int create_radio();
        void close_radio();
        int sendData(const char *_data, int len);
};

#endif