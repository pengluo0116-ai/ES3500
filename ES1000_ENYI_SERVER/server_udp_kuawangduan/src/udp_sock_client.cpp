#include "udp_sock_client.h"
using namespace std;
/*
    函数名称：udp_sock_client
    函数功能：构造函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-13
*/
udp_sock_client::udp_sock_client(const char *_server_ip, int _server_port):
server_ip(string(_server_ip)), server_port(_server_port), socket_descriptor(0x00){
    
}

/*
    函数名称：~udp_sock_client
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-13
*/
udp_sock_client::~udp_sock_client(){
    this->close_client();
}

/*
    函数名称：create_client
    函数功能：创建一个udp客户端
    传入参数：无
    传出数据：-1：创建socket失败 1：异常 0：成功
    注意事项：创建失败时，系统会自动关闭socket操作文件，无需重复进行关闭操作
    编写人员：王凤龙
    编写时间：2016-04-13
*/
int udp_sock_client::create_client(){
    int result = 0x00;
    
    try{
        //创建一个socket
        this->socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if(this->socket_descriptor <= 0x00) return -1;
        
        //socket参数设置
        bzero(&(this->serverAddr), sizeof(this->serverAddr));
        this->serverAddr.sin_family = AF_INET;
        this->serverAddr.sin_port = htons(this->server_port);
        this->serverAddr.sin_addr.s_addr = inet_addr(this->server_ip.c_str());
        
        //接收模块的长度
        this->serverAddr_length = sizeof(this->serverAddr);
    }catch(...){ this->close_client(); return 1;}
    
    return 0;
}

/*
    函数名称：close_client
    函数功能：关闭udp客户端
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-13
*/
void udp_sock_client::close_client(){
    if(this->socket_descriptor == 0x00) return;
    close(this->socket_descriptor);
    this->socket_descriptor = 0x00;
} 

/*
    函数名称：sendData
    函数功能：发送数据
    传入参数：无
    传出数据：   
            发送成功，返回实际发送的数据量
            发送失败：-1
            传入的数据有误：-2
    注意事项：需在建立客户端操作后忙，才能进行发送数据操作
    编写人员：王凤龙
    编写时间：2016-04-13
*/
int udp_sock_client::sendData(const unsigned char *_data, int _data_len){
    if(_data == NULL || _data_len <= 0x00) return -2;
    
    return sendto(
        this->socket_descriptor,
        (char *)_data,
        _data_len,0,
        (struct sockaddr *)&(this->serverAddr),
        this->serverAddr_length
    );
}

/*
    函数名称：getData
    函数功能：读取网络数据
    传入参数：  
            unsigned char *_data -- 用于接收数据的指针
            int *_data_len -- 接收数据的缓存区大小
    传出数据：
            接收到实际数量
            如果<0，则说明接收数据错误
    注意事项：接收数据时，应明确知道接收的数据量小于缓存区，以放置破坏内存数据
    编写人员：王凤龙
    编写时间：2016-04-08
*/
int udp_sock_client::getData(unsigned char *_data, int _data_len){
    memset((void *)_data, 0x00, _data_len);
    
    return recvfrom(
        this->socket_descriptor,
        (char *)_data,
        _data_len,0,
        (struct sockaddr *)&(this->serverAddr),
        &this->serverAddr_length
    );
}
