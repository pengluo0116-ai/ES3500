#include "net_cont.h"
using namespace std;
#define DEBUG 0

/*
    函数名称：net_cont
    函数功能：构造函数
    传入参数：
                const char *_ip         设备ip
                const char *_gateway    网关
                const char *_netmask    子网掩码
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-12-07
*/
net_cont::net_cont(const char *_ip, const char *_gateway, const char *_netmask):
ip(string(_ip)), gateway(string(_gateway)), netmask(string(_netmask)){}

/*
    函数名称：~net_cont
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-12-07
*/
net_cont::~net_cont(){}

/*
    函数名称：_strToIp
    函数功能：字符串转换成ip模板
    传入参数：
                const string _ip        ip字符串
                struct ip_modle &_mip   解析后存储到该引用指向的存储空间
    传出数据：
                 0  运行正确
                -1  参数有误
                 1  数据格式有误
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-12-07
*/
int net_cont::_strToIp(const std::string &_ip, struct ip_modle &_mip){
    /*参数校验*/
    if(_ip.size()<=0x00) return -0x01;
    
    /*IP解析*/
    int tmp[4] = {0x00};
    int err = sscanf(
        _ip.c_str(),
        "%d.%d.%d.%d",
        &tmp[0], &tmp[1], &tmp[2], &tmp[3]
    );

    /*判断解析是否正确*/
    if(err != 0x04) return 0x01;

    /*写入模板*/
    _mip.ip_3 = (unsigned char)tmp[0];
    _mip.ip_2 = (unsigned char)tmp[1];
    _mip.ip_1 = (unsigned char)tmp[2];
    _mip.ip_0 = (unsigned char)tmp[3];

    return 0x00;
}

/*
    函数名称：_ck_netmask
    函数功能：校验子网掩码
    传入参数：无
    传出数据：
                -1  子网掩码为0.0.0.0或255.255.255.255
                -2  子网掩码校验有误
                 0  运行正确
    注意事项：运行该函数时，确保子网掩码已经被提取
    编写人员：王凤龙
    编写时间：2016-12-07
*/
int net_cont::_ck_netmask(){
    /*数据转换*/
    unsigned int tmp = *((unsigned int *)&this->mnetmask);
    if(tmp == 0x00 || tmp == 0xFFFFFFFF) return -0x01;

    /*校验*/
    int point = 0x00;
    for(point=0x00; point<32; point++){ if(tmp & (1 << point)) break; }
    if( (~((0xFFFFFFFF >> point) << point)) & tmp ){
        #if DEBUG
        cout<<"子网掩码有误" << tmp << ":" << point <<endl;
        #endif
        return -0x02;
    }

    return 0x00;
}

/*
    函数名称：_ck_netinfo
    函数功能：校验网络信息
    传入参数：无
    传出数据：
                0   运行正确
                1   设备IP和网关不在同一个网段
                2   网关IP不合法
                3   设备IP不合法
    注意事项：运行该函数时，确保ip和网关，子网掩码已经被提取
    编写人员：王凤龙
    编写时间：2016-12-07
*/
int net_cont::_ck_netinfo(){
    /*校验是否在同一个网段*/
    if(
       ( (*(unsigned int *)&this->mip) & (*(unsigned int *)&this->mnetmask) ) !=
       ( (*(unsigned int *)&this->mgateway) & (*(unsigned int *)&this->mnetmask) )
    ){
        #if DEBUG
        cout<< "设备IP和网关不在同一网段" <<endl;
        #endif
        return 0x01;
    }

    unsigned int tmp = 0x00;

    /*校验网关ip是否合法*/
    tmp = (*(unsigned int *)&this->mgateway) & (~(*(unsigned int *)&this->mnetmask));
    if(tmp==0 || tmp==(~(*(unsigned int *)&this->mnetmask))){
        #if DEBUG
        cout<< "网关IP不合法" <<endl;
        #endif
        return 0x02;
    }

    /*校验设备IP是否合法*/
    tmp = (*(unsigned int *)&this->mip) & (~(*(unsigned int *)&this->mnetmask));
    if(tmp==0 || tmp==(~(*(unsigned int *)&this->mnetmask))){
        #if DEBUG
        cout<< "设备IP不合法" <<endl;
        #endif
        return 0x03;
    }

    #if DEBUG
    cout<< "网络信息校验正确" <<endl;
    #endif
    return 0x00;
}

/*
    函数名称：init
    函数功能：初始化
    传入参数：无
    传出数据：
                0   运行正确
                1   提取设备IP失败
                2   提取网关IP失败
                3   提取子网掩码失败
    注意事项：使用该类时，需要最先运行该函数
    编写人员：王凤龙
    编写时间：2016-12-07
*/
int net_cont::init(){
    /*提取设备IP*/
    if(this->_strToIp(this->ip, this->mip)) return 0x01;

    /*提取网关IP*/
    if(this->_strToIp(this->gateway, this->mgateway)) return 0x02;

    /*提取子网掩码*/
    if(this->_strToIp(this->netmask, this->mnetmask)) return 0x03;

    return 0x00;
}

/*
    函数名称：check
    函数功能：校验网络信息
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-12-07
*/
int net_cont::check(){
    int err = 0x00;

    /*校验子网掩码*/
    if(err = this->_ck_netmask()) return err;

    /*校验网络信息*/
    if(err = this->_ck_netinfo()) return err;

    return err;
}







