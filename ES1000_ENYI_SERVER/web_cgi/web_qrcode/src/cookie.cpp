#include "cookie.h"
using namespace std;

/*
    编译：
    mipsel-openwrt-linux-g++ -o ../obj/cookie.o -c cookie.cpp
*/

/*
    函数名称：cookie
    函数功能：构造函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-20
*/
cookie::cookie(){}

/*
    函数名称：~cookie
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-20
*/
cookie::~cookie(){}

/*
    函数名称：setCookie
    函数功能：设置cookie
    传入参数：要设置cookie的内容
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-20
*/
int cookie::setCookie(const char *_cookie_data){
    cout<< "Set-Cookie: " << _cookie_data <<"\r\n";
    return 0x00;
}

/*
    函数名称：getCookie
    函数功能：获取cookie
    传入参数：要设置cookie的内容
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-20
*/
int cookie::getCookie(std::string &_cookie_str){
    char *cookie = (char *)0x00;
    cookie = getenv("HTTP_COOKIE");
    if(cookie == (char *)0x00) return 0x01;
    
    _cookie_str = string(cookie);
    return 0x00;
}