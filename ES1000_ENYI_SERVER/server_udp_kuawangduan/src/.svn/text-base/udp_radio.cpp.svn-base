#include "udp_radio.h"
using namespace std;

#define DEBUG 0

/*
    编译
    mipsel-openwrt-linux-g++ -o ../obj/udp_radio.o -c udp_radio.cpp
*/

/*
    函数名称：udp_radio
    函数功能：类构造函数
    传入参数：const char *Bcastaddr 广播地址, port 端口号
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-26
*/
udp_radio::udp_radio(const char *Bcastaddr, int port){
    bzero(&(this->addrto), sizeof(struct sockaddr_in));
	this->addrto.sin_family = AF_INET;
    inet_pton(AF_INET, Bcastaddr, &this->addrto.sin_addr);
	this->addrto.sin_port = htons(port);
	this->addrto_len = sizeof(addrto);
    
    this->sock_client = 0x00;
    this->opt = 1;
}

/*
    函数名称：~udp_radio
    函数功能：类析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-26
*/
udp_radio::~udp_radio(){ this->close_radio();}

/*
    函数名称：create
    函数功能：创建socket
    传入参数：无
    传出数据：
             0 -- 创建成功
            -1 -- 创建socket失败
            -2 -- 创建广播模式失败
            
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-26
*/
int udp_radio::create_radio(){
    setvbuf(stdout, NULL, _IONBF, 0); 
	fflush(stdout); 
    
    /*创建socket*/
    this->sock_client = -1;
	if ((this->sock_client = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {   
        #if DEBUG
		cout<<"socket 创建失败"<<endl;
        #endif	
		return -1;
	}   
    
    /*设置socket为广播*/
    if(setsockopt(this->sock_client, SOL_SOCKET, SO_BROADCAST, (char *)&(this->opt), sizeof(this->opt)) < 0){
        #if DEBUG
        cout<<"socket设置广播失败"<<endl;
        #endif
        this->close_radio();
        return -2;
    }
    
    return 0;
}

/*
    函数名称：close
    函数功能：关闭socket
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-26
*/
void udp_radio::close_radio(){
    if(this->sock_client <= 0) return;
    
    close(this->sock_client);
    this->sock_client = 0x00;
}

/*
    函数名称：sendData
    函数功能：发送数据
    传入参数：_data 发送数据的指针， len 发送数据长度
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-26
*/
int udp_radio::sendData(const char *_data, int len){
    int res = 0x00;
    res = sendto(this->sock_client, (const char*)_data, len, 0, (sockaddr*)&this->addrto, this->addrto_len);
    if(res < 0) return -1;
    
    return 0;
}
