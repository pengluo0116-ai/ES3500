#ifndef __UDP_SOCK_CLIENT_HH__
#define __UDP_SOCK_CLIENT_HH__
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
#include <arpa/inet.h>

class udp_sock_client{
    private:
        int server_port;                    //要连接的服务器的端口号
        std::string server_ip;              //要连接的服务器IP
        int socket_descriptor;              //socket文件描述符
        struct sockaddr_in serverAddr;      //[服务器]socket配置信息           
        socklen_t serverAddr_length;        //[服务器]socket结构体长度
        
    public:
        udp_sock_client(const char *_server_ip, int _server_port);
        ~udp_sock_client();
        
        int create_client();                    //创建socket服务器
        void close_client();                    //关闭socket
        
        int sendData(const unsigned char *_data, int _data_len);    //发送数据
        int getData(unsigned char *_data, int _data_len);           //接收数据
};

#endif