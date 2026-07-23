#ifndef __TCP_SOCK_SERVER_CLIENT_H__
#define __TCP_SOCK_SERVER_CLIENT_H__
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include "tcp_sock_cmd.h"
#include "Event_vector.h"
#include "../include/json/json.h"

#define SOCK_DATABUFF_SIZE 1024

class tcp_sock_server_client{
    public:
        int client_fd;                                  //socket文件描述符
        struct sockaddr client_addr;                    //客户端socket结构体
        socklen_t sock_len;
        unsigned char getDataBuff[SOCK_DATABUFF_SIZE];  //获取数据的缓冲区
        int dataLen;                                    //数据长度
        
        pthread_t tid;
        
    public:
        tcp_sock_server_client();
        ~tcp_sock_server_client();
        
    private:
        void client_reset();                            //复位初始化
        int getData();                                  //获取数据
        int sendData(const char *_buff, int size);      //发送数据
        int receiveSize(int sockfd,unsigned char* strData,int iLen);
    public:
        void run();                                     //运行函数
        void client_close();                            //关闭客户端
};

#endif

