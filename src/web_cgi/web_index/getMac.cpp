#include "getMac.h"
using namespace std;
#define DEBUG 1

/*
    函数名称：dev_getMac
    函数功能：获取设备的MAC地址
    传入参数：
                unsigned char *macBuff  将获取到的MAC地址存入该指针指向的buff
                int buffLen             buff的大小
    传出数据：
                 0  运行成功
                -1  输入参数有误
                -2  创建socket失败
                 1  获取MAC地址失败

    注意事项：获取的为eth0的MAC地址
    编写人员：王凤龙
    编写时间：2016-09-14
*/
int  dev_getMac(unsigned char *macBuff, int buffLen){
    /*校验输入参数*/
    if(macBuff == (unsigned char *)0x00 || buffLen < 0x06){
        #if DEBUG
        cout<< "获取MAC地址-参数有误" <<endl;
        #endif
        return -0x01;
    }

    /*创建socket，ifreq*/
    int sock = 0x00;
    if((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0){
        #if DEBUG
        cout<< "获取MAC地址-创建socket失败" <<endl;
        #endif
        return -0x02;
    }

    struct ifreq mifreq;
    strcpy(mifreq.ifr_name, "eth0");

    /*获取MAC*/
    if(ioctl (sock, SIOCGIFHWADDR, &mifreq) < 0){
        #if DEBUG
        cout<< "获取MAC地址-获取MAC失败" <<endl;
        #endif
        close(sock);
        return 0x01;
    }

    /*将MAC地址进行赋值*/
    memcpy((void *)macBuff, (void *)mifreq.ifr_hwaddr.sa_data, 0x06);

    close(sock);
    return 0x00;
}