#ifndef __EVENT_TCP_SOCK_SERVER_H__
#define __EVENT_TCP_SOCK_SERVER_H__
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tcp_sock_server_client.h"
class event_tcp_sock_server{
    
    private:
        int server_fd;                  //服务器文件描述符
        struct sockaddr_in server_addr; //服务器结构体
        int client_num;
    public:
        event_tcp_sock_server(int _port, int _client_num);  //构造函数
        ~event_tcp_sock_server();          //析构函数
        
    public:
        void sock_conf(int _port);   //配置服务器
        int sock_create();          //创建socket
        int sock_bind();            //绑定地址和端口
        int sock_lisen();           //监听
        void sock_close();          //关闭服务器
        void sock_accept();          //接收请求处理 
    public:
       static void* fun_thread(void *_client);
       static void* run(void *argv);
};
#endif