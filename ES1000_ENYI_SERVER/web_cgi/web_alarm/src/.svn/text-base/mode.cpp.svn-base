#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include "../include/sqlite/sqlite3.h"
#include "dbcontrl.h"
#include "conf.h"
#include "../include/json/json.h"
using namespace std;
/*
    所用模板：VOIDAR_WEB_MD_1
    编写人员：王凤龙
    备注信息：
                无需关心底层是如何传递过来数据，以及如何提取和解析数据！
                终端传入参数模型(JSON)：{"display":1, ...}
                除了"相关操作函数添加处"和"分支操作"处外，其它地方不可做任何修改，包括注释
*/

#define GET_DATA_BUFF_LEN 1024              //获取终端传输数据缓冲区的大小
static char msg[GET_DATA_BUFF_LEN] = {0x00};
static void err_msg(const char *_err_msg);
static void err_msg(const char *_code, const char *_err_msg);

/*========================================================================================*/
/*相关操作函数添加处*/
static void sayHello(){
    err_msg("0", "hello world");
}
/*========================================================================================*/

/*
    函数名称：getPostData
    函数功能：获取终端传递过来的数据，并且进行解析，处理
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-06
*/
static void getPostData(){
    #if 0
    /*用户登录信息cookie的校验*/
    ckuser_cookie _user_ck;
    if(_user_ck.ckUser()){ err_msg("399", ""); return;}
    #endif

    int len = 0x00;
    char *lenStr = (char *)0x00;
    memset(msg, 0, GET_DATA_BUFF_LEN);
    
    /*获取post上传数据长度*/
    lenStr =  getenv("CONTENT_LENGTH");
    if(lenStr == (char *)0x00){ err_msg("上传数据为空"); return;}
    len = atoi(lenStr);
    if(len > (GET_DATA_BUFF_LEN-1)) { err_msg("上传数据错误"); return;} //上传数据量超限，返回错误信息
    
    /*获取上传数据*/
    fgets(msg, len+1, stdin);
    
    Json::Reader reader;
    Json::Value value;
    int display = 0x00;
    try{
        if(!reader.parse(string(msg), value)) throw("");
        display = value["display"].asInt();
        
        switch(display){
            /*分支操作，添加此处*/
            /*==========================================*/
            case 1: break;
            /*==========================================*/
            default: err_msg("上传数据格式错误"); break;
        }
    }catch(...){
        err_msg("上传数据格式错误");
    }
}


/*
    函数名称：err_msg
    函数功能：返回错误信息
    传入参数：_code->错误编码 _err_msg->错误信息
    输出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-16
*/
static void err_msg(const char *_err_msg){
    string err_msg = "{\"error\":404, \"data\":\""+ string(_err_msg) +"\"}";
    cout<<"Content-type:text/html\r\n\r\n";
    cout<<err_msg<<endl;
}

static void err_msg(const char *_code, const char *_err_msg){
    string err_msg = "{\"error\":"+ string(_code) +", \"data\":\""+ string(_err_msg) +"\"}";
    cout<<"Content-type:text/html\r\n\r\n";
    cout<<err_msg<<endl;
}

int main(){
    try{
        getPostData();
    }catch(...){ err_msg("访问有误，请稍候再试");}
    
    return 0;
}