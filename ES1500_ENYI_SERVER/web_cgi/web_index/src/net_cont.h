#ifndef __NET_CONT_H__
#define __NET_CONT_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

/*ip模板*/
struct ip_modle{
    unsigned char ip_0;
    unsigned char ip_1;
    unsigned char ip_2;
    unsigned char ip_3;
};

class net_cont{
    private:
        std::string ip;             //IP地址
        std::string gateway;        //网关
        std::string netmask;        //子网掩码
        std::string mac;            //MAC地址

        struct ip_modle mip;        //IP地址
        struct ip_modle mgateway;   //网关
        struct ip_modle mnetmask;   //子网掩码
    public:
        net_cont(const char *_ip, const char *_gateway, const char *_netmask, const char *_mac);
        ~net_cont();

    private:
        int _strToIp(const std::string &_ip, struct ip_modle &_mip);    //字符串转换成ip模板
        int _ck_netinfo();                                              //校验网络信息
        int _ck_netmask();                                              //校验子网掩码
        int _ck_mac();                                                  //校验MAC地址

    public:
        int init();                                                     //初始化
        int check();                                                    //校验
};

#endif