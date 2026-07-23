#include "tcp_sock_client.h"
/*
    编译
    mipsel-openwrt-linux-g++ -o ../obj/tcp_sock_client.o -c tcp_sock_client.cpp
*/

/*
    函数名称：tcp_sock_client
    函数功能：构造函数
    传入参数：
            const char *_add -- 服务器ip
            int _port -- 服务器端口
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-16
*/
tcp_sock_client::tcp_sock_client(const char *_add, int _port){
    this->server_ip = string(_add);
    this->server_port = _port;
    this->sock_client = 0x00;
}

/*
    函数名称：~tcp_sock_client
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-16
*/
tcp_sock_client::~tcp_sock_client(){
    this->sock_close(); //关闭socket
}

/*
    函数名称：creat_sock
    函数功能：创建一个socket
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-16
*/
int tcp_sock_client::creat_sock(){
    try{
         this->sock_client = socket(AF_INET, SOCK_STREAM, 0);
         if(this->sock_client < 0){ this->sock_client=0x00; throw("");}
         
    }catch(...){
        //cout<<"创建socket失败"<<endl;
        return -1;
    }
   return 0;
}

/*
    函数名称：server_socke_conf
    函数功能：配置服务器信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-16
*/
int tcp_sock_client::server_socke_conf(){
    try{
        (void)memset(&(this->server_addr), 0, sizeof(this->server_addr));
        this->server_addr.sin_family = AF_INET;
        this->server_addr.sin_port = htons(this->server_port);
        this->server_addr.sin_addr.s_addr = inet_addr(this->server_ip.c_str());
    }catch(...){ /*cout<<"配置socket服务器参数失败"<<endl;*/ return -1;}
    
    return 0;
}

/*
    函数名称：conct_sock
    函数功能：连接服务器
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-16
*/
int tcp_sock_client::conct_sock(){
    int err = 0x00;
    
    //设置超时时间
    struct timeval timeout = {5, 0};
    err = setsockopt(this->sock_client,SOL_SOCKET, SO_SNDTIMEO,(const char*)&timeout, sizeof(timeout));
    if(err != 0x00){  return -1;}
    err = setsockopt(this->sock_client,SOL_SOCKET, SO_RCVTIMEO,(const char*)&timeout, sizeof(timeout));
    if(err != 0x00){ return -1;}
    
    
    //建立连接
    err = connect(
        this->sock_client,
        (struct sockaddr *)(&(this->server_addr)),
        sizeof(this->server_addr)
    );
    if(err < 0) { this->sock_close(); return -1;};
    
    //cout<<"服务器连接成功"<<endl;
    return 0;
}

/*
    函数名称：connectToServer
    函数功能：连接服务器
    传入参数：无
    传出数据：
            0   连接成功
            -1  配置服务器信息失败
            -2  创建socket失败
            -3  连接服务器失败
    注意事项：返回错误时，无须系统自动关闭socket
    编写人员：王凤龙
    编写时间：2016-04-08
*/
int tcp_sock_client::connectToServer(){
    if(this->server_socke_conf() != 0x00) return -1;    //配置服务器信息
    if(this->creat_sock() != 0x00) return -2;           //创建一个socket
    if(this->conct_sock() != 0x00) return -3;           //连接服务器
    return 0x00;
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
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-08
*/
int tcp_sock_client::sendData(const unsigned char *_data, int _data_len){
    
    if(_data == NULL || _data_len <= 0x00) return -1;

    return write(
        this->sock_client,
        (const char *)_data,
        _data_len
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
int tcp_sock_client::getData(unsigned char *_data, int _data_len){
    memset((void *)_data, 0x00, _data_len);
    
    return read(
        this->sock_client,
        (char *)_data,
        _data_len
    );
    
}

/*
    函数名称：sock_close
    函数功能：关闭与服务器间的连接
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-16
*/
void tcp_sock_client::sock_close(){
    if(this->sock_client != 0) {close(this->sock_client); this->sock_client = 0;}
    //cout<<"关闭socket"<<endl;
}

/*
    函数名称：tcpClose
    函数功能：关闭与服务器间的socket连接
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-08
*/
void tcp_sock_client::tcpClose(){
    this->sock_close();
}
