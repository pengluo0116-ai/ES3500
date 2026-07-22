#include "tcp_sock_server_client.h"
using namespace std;
#define DEBUG 1
/*
    函数名称：client
    函数功能：构造函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-03-18
*/
tcp_sock_server_client::tcp_sock_server_client(){}

/*
    函数名称：~client
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-03-18
*/
tcp_sock_server_client::~tcp_sock_server_client(){
    this->client_close();
    #if DEBUG
    std::cout<<"客户端析构函数"<<std::endl;
    #endif
}

/*
    函数名称：client_reset
    函数功能：复位初始化
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-03-18
*/
void tcp_sock_server_client::client_reset(){
    (void)memset((void *)&this->client_addr, 0, sizeof(struct sockaddr));
    this->client_close();
}

/*
    函数名称：client_close
    函数功能：关闭客户端
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-03-18
*/
void tcp_sock_server_client::client_close(){
    if(this->client_fd <= 0x00) return;
    
    close(this->client_fd); 
    this->client_fd = 0x00;
}

/*
    函数名称：getData
    函数功能：从socket获取数据
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-03-17
*/
int tcp_sock_server_client::getData(){
    int reg = 0x00;
    
    memset((void *)this->getDataBuff, 0, SOCK_DATABUFF_SIZE);
    this->dataLen = 0x00;
    
    reg = read(this->client_fd, this->getDataBuff, SOCK_DATABUFF_SIZE);
    
    if(reg <= 0){
        #if DEBUG
        std::cout<<"读取socket数据失败"<<std::endl;
        #endif
        return -1;
    }
    
    this->dataLen = reg;
    return 0x00;
}

/*
    函数名称：sendData
    函数功能：从socket获取数据
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-03-17
*/
int tcp_sock_server_client::sendData(const char *_buff, int size){
    int reg = 0x00;
    reg = write(this->client_fd, _buff, size);
    
    if(reg <= 0x00){
        #if DEBUG
        std::cout<<"发送数据失败"<<std::endl;
        #endif
        return -1;
    }
    
    return 0x00;
}

/*
    函数名称：run
    函数功能：tcp服务器衍生客户端处理程序
    传入数据：无
    传出数据：无
    注意事项：一般函数在继承该类的函数中重构
    编写人员：王清杰
    编写时间：2018-06-19
*/
void tcp_sock_server_client::run(){
    char recvbuf[1024] = {0};
    char head[4] = {0};
    char hd[4] = {0};
    memset(head,4,0);
    memset(recvbuf,1024,0);
    /*接收长度*/
    int len = this->receiveSize(this->client_fd,(unsigned char*)head,4);
    if(len <= 0)
    {
        return;
    } 
    hd[0] = head[3];
    hd[1] = head[2];
    hd[2] = head[1];
    hd[3] = head[0];

    /*接收指定长度的数据*/
    len = this->receiveSize(this->client_fd,(unsigned char*)recvbuf,*(int*)hd);
    if(len <= 0) return;

    if(Event_vector::put_msg(string(recvbuf))) return;

    this->sendData((const char*)"success",7);
    return;
}

/*接收指定长度的数据*/
int tcp_sock_server_client::receiveSize(int sockfd,unsigned char* strData,int iLen)
{
    if(strData == NULL) return -1;
    unsigned char* p = strData;
    int len = iLen;
    int ret = 0;
    int returnlen = 0;
    while(len > 0)
    {
        ret = recv(sockfd,(char*)p+(iLen-len),iLen-returnlen,0);
        if(ret < 0 || ret == 0) return ret;
        len -= ret;
        returnlen += ret;
    }
    return returnlen;
}