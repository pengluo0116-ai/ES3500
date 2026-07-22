#include "display.h"
using namespace std;

#define DEBUG 1

/*
    函数名称：r
    函数功能：获取随机数
    传入参数：int fanwei 用于获取0-fanwei间的随机数
    传出数据：生成的随机数
    注意事项：无
*/
int r(int fanwei){
    srand((unsigned)time(NULL));    //用于保证是随机数
    return rand()%fanwei;           //用rand产生随机数并设定范围
}

/*
    函数名称：getMac
    函数功能：获取网口的mac地址
    传入参数：const char *_wlan_name, unsigned char *_mac
    传出数据：0正确 非0错误
    注意事项：_mac所指向的缓存区不得小于6个字节
    编写人员：王凤龙
    编写时间：2016-05-31
*/
int getMac(const char *_wlan_name, unsigned char *_mac){
    struct ifreq ifreq;
    int sock;
    
    if((sock=socket(AF_INET,SOCK_STREAM,0)) <0) { 
        #if DEBUG
        perror( "socket");
        #endif
         
        return   1; 
    } 
    
    strcpy(ifreq.ifr_name, _wlan_name);
    
    if(ioctl(sock,SIOCGIFHWADDR,&ifreq) <0){
        #if DEBUG 
        perror( "ioctl");
        #endif 
        
        return   2; 
    } 
    
    _mac[0] = (unsigned char)ifreq.ifr_hwaddr.sa_data[0];
    _mac[1] = (unsigned char)ifreq.ifr_hwaddr.sa_data[1];
    _mac[2] = (unsigned char)ifreq.ifr_hwaddr.sa_data[2];
    _mac[3] = (unsigned char)ifreq.ifr_hwaddr.sa_data[3];
    _mac[4] = (unsigned char)ifreq.ifr_hwaddr.sa_data[4];
    _mac[5] = (unsigned char)ifreq.ifr_hwaddr.sa_data[5];
    
    return 0x00;
}