#include "event_tcp_sock_server.h"
using namespace std;
#define DEBUG  1
/*
    函数名称：server
    函数功能：构造函数
    传入参数：int port -> 服务器端口号  int _client_num -> 服务连接最大数量
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-03-18
*/
event_tcp_sock_server::event_tcp_sock_server(int _port, int _client_num){
    /*监听最大数量*/
    this->client_num = _client_num;     
    
    /*服务器结构体配置*/
    this->sock_conf(_port);
    
    /*服务器文件描述符设置为0*/
    this->server_fd = 0x00;
}

/*
    函数名称：~server
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-03-18
*/
event_tcp_sock_server::~event_tcp_sock_server(){
    this->sock_close();
}

/*
    函数名称：sock_conf
    函数功能：配置服务器socket结构体
    传入参数：int _port -> 服务器端口号
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-03-18
*/
void event_tcp_sock_server::sock_conf(int _port){
    (void)memset(&this->server_addr, 0, sizeof(struct sockaddr_in));
    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	this->server_addr.sin_port	= htons(_port);
}

/*
    函数名称：sock_create
    函数功能：创建服务器socket
    传入参数：无
    传出数据：0->成功 -1->失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-03-18
*/
int event_tcp_sock_server::sock_create(){
    int reg = 0x00;
    reg = socket(AF_INET, SOCK_STREAM, 0);
    if(reg <= 0){
        #if DEBUG
        std::cout<<"服务器创建soket失败"<<std::endl;
        #endif
        return -1;
    }
    this->server_fd = reg;

    int on = 1;
    if(setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))){
        #if DEBUG
        std::cout<<"服务器端口复用失败"<<std::endl;
        #endif
        this->sock_close();
        return -1;
    }
    
    return 0x00;
}

/*
    函数名称：sock_bind
    函数功能：服务器绑定端口
    传入参数：无
    传出数据：0->成功 -1->socket未开启 -2->绑定失败
    注意事项：绑定失败后，会自动关闭服务器socket
    编写人员：王凤龙
    编写时间：2016-03-18
*/
int event_tcp_sock_server::sock_bind(){
    int reg = 0x00;
    
    /*判断服务器socket是否开启*/
    if(this->server_fd <= 0x00){
        #if DEBUG
        std::cout<<"服务器socket未开启"<<std::endl;
        #endif
        
        return -1;
    }
    
    /*绑定操作*/
    reg = bind(
			this->server_fd,
			(struct sockaddr *)&this->server_addr,
			sizeof(struct sockaddr)
	);
    
    /*判断是否绑定成功*/
    if(reg < 0x00){
        #if DEBUG
        std::cout<<"阿里事件上报->服务器绑定端口失败"<<std::endl;
        #endif
        
        this->sock_close();
        return -2;
    }
    
    return 0x00;
}

/*
    函数名称：sock_lisen
    函数功能：监听端口
    传入参数：无
    传出数据：0->成功 -1->失败
    注意事项：绑定失败后，会自动关闭服务器socket
    编写人员：王凤龙
    编写时间：2016-03-18
*/
int event_tcp_sock_server::sock_lisen(){
    int reg = 0x00;
    reg = listen(this->server_fd, this->client_num);
    
    if(reg != 0x00){
        #if DEBUG
        std::cout<<"监听失败"<<std::endl;
        #endif
        return -1;
    }
    
    return 0x00;
}

/*
    函数名称：sock_close
    函数功能：关闭服务器socket
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-03-18
*/
void event_tcp_sock_server::sock_close(){
    if(this->server_fd != 0x00){ 
        close(this->server_fd);
        this->server_fd = 0x00;
    }
        
    #if DEBUG
    std::cout<<"服务器关闭"<<std::endl;
    #endif
}

/*
    函数名称：fun_thread
    函数功能：线程处理函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-03-18
*/
void* event_tcp_sock_server::fun_thread(void *_client){
    pthread_detach(pthread_self());
    tcp_sock_server_client *sock_client = (tcp_sock_server_client *)_client;
    try{
        struct timeval timeout = {10, 0};
        int err = 0x00;
        err = setsockopt(sock_client->client_fd, SOL_SOCKET, SO_SNDTIMEO,(const char*)&timeout, sizeof(timeout));
        if(err != 0x00){ perror("socket send conf err"); throw("");}
        err = setsockopt(sock_client->client_fd, SOL_SOCKET, SO_RCVTIMEO,(const char*)&timeout, sizeof(timeout));
        if(err != 0x00){ perror("socket recv conf err"); throw("");}
        sock_client->run();
    }catch(...){}
    sock_client->client_close();
    delete sock_client;
    
    return NULL;
}

/*
    函数名称：sock_accept
    函数功能：接收连接并开启客户端线程
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-03-18
*/
void event_tcp_sock_server::sock_accept(){
    int reg = 0x00;
    
    for(;;){
        reg = 0x00;
        tcp_sock_server_client *tmp = new tcp_sock_server_client();
        
        reg = accept(
            this->server_fd,
            &tmp->client_addr,
            &tmp->sock_len
        );
        
        if(reg <= 0){
            #if DEBUG
            std::cout<<"接收请求失败"<<std::endl;
            #endif
            
            delete tmp;
            usleep(300);
            return;
        }
        
        tmp->client_fd = reg;   //socket客户端文件描述符
        
        reg = pthread_create(&tmp->tid, NULL, event_tcp_sock_server::fun_thread, (void *)tmp);
        if(reg < 0){
            #if DEBUG
            std::cout<<"客户端线程创建失败"<<std::endl;
            #endif
            
            delete tmp;
            usleep(300);
            continue;
        }
        
        #if DEBUG
        std::cout<<"客户端创建成功"<<std::endl;
        #endif
    }
}

/*
    函数名称：run
    函数功能：外部控制接口服务器工作入口
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-06
*/
void* event_tcp_sock_server::run(void *argv){
    pthread_detach(pthread_self());
    argv = argv;
    
    event_tcp_sock_server wserver(8888, 12);
    
    web_con_start:
    if(0 != wserver.sock_create()) goto web_con_end;
    if( 0 == wserver.sock_bind() && 0 == wserver.sock_lisen()) wserver.sock_accept();
    
    wserver.sock_close(); goto web_con_end;
    
    web_con_end:
    sleep(5);
    goto web_con_start;
}
