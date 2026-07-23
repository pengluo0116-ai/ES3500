#ifndef __UDP_SOCK_SERVER_H__
#define __UDP_SOCK_SERVER_H__
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sstream>

class udp_sock_server{
    private:
        int port;                               //服务器端口
        int localSocket;                        //本地[服务器]socket
        struct sockaddr_in localAddr;           //本地[服务器]socket配置信息
        struct sockaddr_in remoteAddr;          //连接外部客户端socket配置信息
        socklen_t remoteAddrLength;             //
            
    public:
        udp_sock_server(int _port);             //构造函数
        ~udp_sock_server();                     //析构函数
        int create_server();                    //创建socket服务器
        void close_server();                    //关闭socket
        
        int sendData(const unsigned char *_data, int _data_len);    //发送数据
        int getData(unsigned char *_data, int _data_len);           //接收数据
        int getRemoteIP(std::string &_ip);                          //获取客户端的ip
};




#endif