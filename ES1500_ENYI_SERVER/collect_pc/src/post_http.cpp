#include "post_http.h"
using namespace std;
#define DEBUG 0

/*
    函数名称：post_http
    函数功能：构造函数
    传入参数：
                const char *_ip     服务器ip
                const int _port     服务器端口
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-26
*/
post_http::post_http(const char *_ip, const int _port):post_ports(_ip, _port){
    this->httpClient = (CHttpClient *)0x00;
    this->recv_dataBuff = "";
    this->post_url = "";
}

/*
    函数名称：post_http
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-26
*/
post_http::~post_http(){
    delete this->httpClient; this->httpClient = (CHttpClient *)0x00;
}

/*
    函数名称：init
    函数功能：初始化函数
    传入参数：无
    传出数据：无
    注意事项：接口函数
    编写人员：王凤龙
    编写时间：2016-11-26
*/
int post_http::init(){
    this->httpClient = new CHttpClient();
    ostringstream ostr; ostr.str("");
    ostr << "http://" << this->ip << ":" << this->port << "/";
    this->post_url = ostr.str();
    return 0x00;
}

/*
    函数名称：sendData
    函数功能：发送数据函数
    传入参数：无
    传出数据：无
    注意事项：接口函数
    编写人员：王凤龙
    编写时间：2016-11-26
*/
int post_http::sendData(){
    this->recv_dataBuff = "";
    //cout<< this->send_dataBuff.c_str() <<endl;
    this->httpClient->Post(this->post_url, this->send_dataBuff, this->recv_dataBuff);
    return 0x00;
}

/*
    函数名称：recvData
    函数功能：接收数据函数
    传入参数：无
    传出数据：无
    注意事项：接口函数
    编写人员：王凤龙
    编写时间：2016-11-26
*/
int post_http::recvData(){
    #if DEBUG
    cout<< "接收数据：" << this->recv_dataBuff.c_str() <<endl;
    #endif
    
    return 0x00;
}