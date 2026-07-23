#ifndef __TCP_SOCK_CLIENT_H__
#define __TCP_SOCK_CLIENT_H__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

class tcp_sock_client{
    private:
        string server_ip;                   //ip
        int server_port;                    //端口
        struct sockaddr_in	server_addr;    //服务器结构体
        int sock_client;                    //socket客户端
        

    public:
        tcp_sock_client(const char *_add, int _port);
        ~tcp_sock_client();
        
    private:
        int server_socke_conf();            //要连接的socket服务器信息配置
        int creat_sock();                   //创建socket
        int conct_sock();                   //socket建立连接
        void sock_close();                  //socket关闭
   
   public:  
        int connectToServer();   
        int sendData(const unsigned char *_data, int _data_len);
        int getData(unsigned char *_data, int _data_len);
        void tcpClose();
};

#endif