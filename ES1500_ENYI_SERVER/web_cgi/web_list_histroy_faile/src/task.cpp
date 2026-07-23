#include "task.h"
using namespace std;

#define GET_DATA_BUFF_LEN 1024              //获取终端传输数据缓冲区的大小
static char msg[GET_DATA_BUFF_LEN] = {0x00};

/*
    函数名称：getPostData
    函数功能：获取终端传递过来的数据，并且进行解析，处理
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-06
*/
void getPostData(){
    //  Json::Value va;
    // //  cmd_get_filelist();
    //  va["data"] = "201610";
    //  cmd_get_tar_file(va);
    //  return ;


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
            // case 1: cmd_getAlarmConf(); break;
            // case 2: cmd_setAlarmConf(value); break;
         
            case 1: cmd_get_filelist(); break;
            case 2: cmd_get_tar_file(value); break;

            /*==========================================*/
            default: err_msg("上传数据格式错误"); break;
        }
    }catch(...){
        err_msg("上传数据格式错误");
    }
}