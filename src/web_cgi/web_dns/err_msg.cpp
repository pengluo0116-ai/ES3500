#include "err_msg.h"
using namespace std;

/*
    函数名称：err_msg
    函数功能：返回错误信息
    传入参数：_code->错误编码 _err_msg->错误信息
    输出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-16
*/
void err_msg(const char *_err_msg){
    string err_msg = "{\"error\":404, \"data\":\""+ string(_err_msg) +"\"}";
    cout<<"Content-type:text/html\r\n\r\n";
    cout<<err_msg<<endl;
}

void err_msg(const char *_code, const char *_err_msg){
    string err_msg = "{\"error\":"+ string(_code) +", \"data\":\""+ string(_err_msg) +"\"}";
    cout<<"Content-type:text/html\r\n\r\n";
    cout<<err_msg<<endl;
}