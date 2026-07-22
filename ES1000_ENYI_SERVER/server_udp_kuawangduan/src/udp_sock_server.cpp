#include "udp_sock_server.h"
using namespace std;
#define DEBUG 1

/*
    函数名称：udp_sock_server
    函数功能：构造函数
    传入参数：int _port -- 端口号
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-11
*/
udp_sock_server::udp_sock_server(int _port):port(_port), localSocket(0x00){}

/*
    函数名称：~udp_sock_server
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-11
*/
udp_sock_server::~udp_sock_server(){}

/*
    函数名称：create_server
    函数功能：创建socket
    传入参数：无
    传出数据：
            0：运行成功
            1：运行异常
            -1：创建socket失败
            -2：绑定到指定端口失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-11
*/
int udp_sock_server::create_server(){
    int result = 0x00;
    
    try{
        //创建一个socket
        this->localSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if(this->localSocket <= 0x00) return -1;
        
        //socket参数设置
        bzero(&(this->localAddr), sizeof(this->localAddr));
        this->localAddr.sin_family = AF_INET;
        this->localAddr.sin_port = htons(this->port);
        this->localAddr.sin_addr.s_addr = INADDR_ANY;
        
        //参数绑定到socket，使socket工作
        result = bind(this->localSocket, (struct sockaddr *)&(this->localAddr), sizeof(this->localAddr));
        if(result != 0) { this->close_server(); return -2; }
        
        //接收模块的长度
        this->remoteAddrLength = sizeof(this->remoteAddr);
    }catch(...){ this->close_server(); return 1;}
    
    return 0;
}

/*
    函数名称：sendData
    函数功能：发送数据
    传入参数：
            const unsigned char *_data -- 传入数据指针
            int _data_len -- 传入数据长度
    传出数据:
            <0 发送错误 
            >0 发送的实际长度 
            =0 没有发送出去
    注意事项：仅仅用于接收到外部连接后，才能发送
    编写人员：王凤龙
    编写时间：2016-04-11
*/
int udp_sock_server::sendData( const unsigned char *_data, int _data_len){
    if(_data == NULL || _data_len <= 0x00) return -1;

    return sendto(
        this->localSocket,
        (char *)_data,
        _data_len,0,
        (struct sockaddr *)&(this->remoteAddr),
        this->remoteAddrLength
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
int udp_sock_server::getData(unsigned char *_data, int _data_len){
    memset((void *)_data, 0x00, _data_len);
    
    return recvfrom(
        this->localSocket,
        (char *)_data,
        _data_len,0,
        (struct sockaddr *)&(this->remoteAddr),
        &this->remoteAddrLength
    );
    
}

/*
    函数名称：close_server
    函数功能：关闭socket服务器
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-11
*/
void udp_sock_server::close_server(){
    if(this->localSocket <= 0x00) return;
    close(this->localSocket);
    this->localSocket = 0x00;
}

/*
    函数名称：getRemoteIP
    函数功能：获取连接到本服务器的客户端的IP
    传入参数：std::string &_ip 用于存储获取到的IP
    传出数据：0操作成功，非0操作失败
    注意事项：只有在获取到连接时，本函数的结果才有效
    编写人员：王凤龙
    编写时间：2016-05-19
*/
int udp_sock_server::getRemoteIP(std::string &_ip){
    
    if(!(unsigned int)this->remoteAddr.sin_addr.s_addr) return -1;
    
    ostringstream _str_stm_; _str_stm_.str("");
    _str_stm_   << (unsigned int)(*((unsigned int *)&this->remoteAddr.sin_addr.s_addr) & 0xff) << "."
                << (unsigned int)((*((unsigned int *)&this->remoteAddr.sin_addr.s_addr)>>8) & 0xff) << "."
                << (unsigned int)((*((unsigned int *)&this->remoteAddr.sin_addr.s_addr)>>16) & 0xff) << "."
                << (unsigned int)((*((unsigned int *)&this->remoteAddr.sin_addr.s_addr)>>24) & 0xff);
    _ip = _str_stm_.str(); return 0x00;
}

#if 0

int main(){
    udp_sock_server rbserver(9999);
    if(rbserver.create_server() != 0x00){ std::cout<<"开启端口失败"<<std::endl; return 0x00;}
    
    unsigned char tmp_Buff[1024] = {0x00};
    for(;;){
        switch(rbserver.getData((unsigned char *)tmp_Buff, 1024)){
            case 0: std::cout<<"数据为0"<<std::endl; break;
            case -1: std::cout<<"接收错误"<<std::endl; break;
            default:
                std::cout<<tmp_Buff<<std::endl;
                rbserver.sendData((const unsigned char *)"dddd", 4);
        }
    }
    
    return 0x00;
}

#endif