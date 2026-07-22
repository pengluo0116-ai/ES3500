#include "stc_cont.h"
using namespace std;
#define DEBUG 1

/*
    函数名称：stc_cont
    函数功能：构造函数
    传入参数：
                const char *_ttl_path           串口文件路径
                int _nSpeed                     波特率
                char _nEvent                    奇偶校验位
                int _nPartity                   数据位
                int _nStop                      停止位
    传出数据：无
    注意事项：无
    编写人员：王凤龙 
*/
stc_cont::stc_cont(const char *_ttl_path, int _nSpeed, char _nEvent, int _nPartity, int _nStop):
usart_read(_ttl_path, _nSpeed, _nEvent, _nPartity, _nStop){}

/*
    函数名称：~stc_cont
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-25
*/
stc_cont::~stc_cont(){
}

/*
    函数名称：stc_init
    函数功能：初始化函数
    传入参数：无
    传出数据：
                 0  运行成功
                -1  串口文件路径有误
                -2  波特率设置有误
                -3  奇偶校验位有误
                -4  数据位有误
                -5  停止位有误
                -6  打开串口文件失败
                -7  获取串口原有配置失败
                -8  提交串口配置失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-25
*/
int stc_cont::stc_init(){
    /*初始化串口*/
    return this->init();
}
