#include "post_ports.h"
using namespace std;

#define DEBUG 0

/*
    函数名称：post_ports
    函数功能：构造函数
    传入参数：
                const char *_ip     服务器IP
                const int _port     服务器端口
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-26
*/
post_ports::post_ports(const char *_ip, const int _port):ip(string(_ip)), port(_port){
    this->send_dataBuff = "";
}

/*
    函数名称：~post_ports
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-26
*/
post_ports::~post_ports(){}

/*
    函数名称：getData_fromVct
    函数功能：从容器中读取要发送的数据
    传入参数：无
    传出数据：小于0 运行有误 =0 容器内消息为空 =1 获取消息
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-26
*/
int post_ports::getData_fromVct(){
    return lift_vector::get_msg(this->send_dataBuff);
}

/*
    函数名称：init
    函数功能：初始化函数
    传入参数：无
    传出数据：保留 0
    注意事项：该函数用于用户实现接口，发送前的初始化
    编写人员：王凤龙
    编写时间：2016-11-26
*/
int post_ports::init(){ return 0x00; }

/*
    函数名称：sendData
    函数功能：发送数据到服务器
    传入参数：无
    传出数据：保留 0
    注意事项：该函数用于用户实现接口，发送数据使用
    编写人员：王凤龙
    编写时间：2016-11-26
*/
int post_ports::sendData(){
    #if DEBUG
    cout<<"发送的数据为:" << this->send_dataBuff.c_str() <<endl;
    #endif

    return 0x00;
}

/*
    函数名称：recvData
    函数功能：接收服务器返回的数据
    传入参数：无
    传出数据：保留 0
    注意事项：该函数用于用户实现接口，从服务器接收数据使用
    编写人员：王凤龙
    编写时间：2016-11-26
*/
int post_ports::recvData(){ return 0x00; }

/*
    函数名称：work
    函数功能：工作函数
    传入参数：无
    传出数据：保留 0
    注意事项：该函数用于用户实现接口，发送前的初始化
    编写人员：王凤龙
    编写时间：2016-11-26
*/
int post_ports::work(){
    /*参数校验*/
    if(this->ip.size() <= 0x00 || this->port<=0x00) return -0x01;

    /*初始化*/
    if(this->init()) return -0x02;

    /*清空采集数据缓存*/
    while(this->getData_fromVct()){  }

    /*工作函数*/
    string send_dataBak = "";   //发送数据备份 
    int cf_num = 0x00; //重复次数
    for(;;){
        switch(this->getData_fromVct()){
            case 0: usleep(20000); continue;
            case 1: break;
            default: return -0x03;
        }

        /*重复判断*/
        if((send_dataBak == send_dataBuff) && ++cf_num < 20){ usleep(20000); continue; }

        #if DEBUG
        cout<<"********************" << cf_num <<endl;
        cout<<send_dataBak.c_str() <<endl;
        cout<< this->send_dataBuff.c_str() <<endl;
        #endif

        if(this->sendData()) return 0x01;
        if(this->recvData()) return 0x02;
        usleep(30000);
        
        send_dataBak = this->send_dataBuff;
        cf_num = 0x00;
    }
    return 0x03;
}