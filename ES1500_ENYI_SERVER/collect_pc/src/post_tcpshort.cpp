#include "post_tcpshort.h"
using namespace std;
#define DEBUG 0

/*
    函数名称：post_tcoshort
    函数功能：构造函数
    传入参数：
                const char *_ip     服务器ip
                const int _port     服务器端口
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-29
*/
post_tcpshort::post_tcpshort(const char *_ip, const int _port):post_ports(_ip, _port){
    // this->recv_dataBuff = (unsigned char *)0x00;
}

/*
    函数名称：~post_tcpshort
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-29
*/
post_tcpshort::~post_tcpshort(){
    // if(this->recv_dataBuff){ delete this->recv_dataBuff; this->recv_dataBuff=(unsigned char *)0x00; }
}

/*
    函数名称：init
    函数功能：初始化
    传入参数：无
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-29
*/
int post_tcpshort::init(){
    // this->recv_dataBuff = new unsigned char[POST_TCPSHORT_RCVEMAX];
    // if(!this->recv_dataBuff) return -0x01;

    return 0x00;
}

/*
    函数名称：sendData
    函数功能：发送数据
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-29
*/
int post_tcpshort::sendData(){
    tcp_sock_client _client(this->ip.c_str(), this->port);
    if(_client.connectToServer()) return -0x01;
    if(_client.sendData((const unsigned char *)this->send_dataBuff.c_str(), this->send_dataBuff.size()) <= 0x00) return -0x02;

    return 0x00;
}