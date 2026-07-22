#include "post_tcplong.h"
using namespace std;
#define DEBUG 0

/*
    函数名称：post_tcplong
    函数功能：构造函数
    传入参数：
                const char *_ip     服务器IP
                const int _port     服务器端口
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-12-21
*/
post_tcplong::post_tcplong(const char *_ip, const int _port):post_ports(_ip, _port){
    this->recv_dataBuff = (unsigned char *)0x00;
    this->tcp_client = (tcp_sock_client *)0x00;
}

/*
    函数名称：~post_tcplong
    函数功能：析构函数
    传入参数：无
    传出数据：
                 0  运行成功
                -1  tcp客户端创建失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-12-21
*/
post_tcplong::~post_tcplong(){
    if(this->tcp_client){ delete this->tcp_client; this->tcp_client = (tcp_sock_client *)0x00; }
    if(this->recv_dataBuff){ delete this->recv_dataBuff; this->recv_dataBuff = (unsigned char *)0x00; }
}

/*
    函数名称：init
    函数功能：初始化函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-12-21
*/
int post_tcplong::init(){
    /*服务器接收缓存区初始化*/
    this->recv_dataBuff = new unsigned char[POST_TCPLONG_RCVEMAX];

    /*tcp客户端初始化*/
    this->tcp_client = new tcp_sock_client(this->ip.c_str(), this->port);
    if(this->tcp_client->connectToServer()){ return -0x01;}

    return 0x00;
}

/*
    函数名称：sendData
    函数功能：发送数据
    传入参数：无
    传出数据：
    注意事项：2016-12-21
    编写人员：王凤龙
    编写时间：2016-12-21
*/
int post_tcplong::sendData(){
    /*tcp发送数据*/
    if(this->tcp_client->sendData((const unsigned char *)this->send_dataBuff.c_str(), this->send_dataBuff.size()) <= 0x00) return -0x01;

    /*tcp接收数据*/
    if(this->tcp_client->getData(this->recv_dataBuff, POST_TCPLONG_RCVEMAX) <= 0x00); //;return -0x02;

    return 0x00;
}

/*
    函数名称：recvData
    函数名称：接收数据
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-12-21
*/
int post_tcplong::recvData(){

    return 0x00;
}