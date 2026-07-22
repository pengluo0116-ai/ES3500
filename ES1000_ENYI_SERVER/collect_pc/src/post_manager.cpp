#include "post_manager.h"
using namespace std;
#define DEBUG 0

/*
    函数名称：work_enable_cont
    函数功能：转发控制使能
    传入参数：const char *_path
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-10
*/
void work_enable_cont(const char *_path){
    /*参数校验*/
    if(!_path || strlen(_path)<=0x00){
        #if DEBUG
        cout<< "转发使能：传入参数有误" <<endl;
        #endif
        sleep(5);
        exit(-1);
    }

    /*获取配置信息*/
    Json::Value jsonConf;
    if(JreadConf(_path, jsonConf)){
        #if DEBUG
        cout<<"转发使能：读取配置文件失败"<<endl;
        #endif
        exit(-0x02);
    }

    if(jsonConf["iswork"].asInt()) return;

    cout<< "禁用转发" <<endl;
    for(;;){
        sleep(3000);
    }
    return;
}

void *post_work(void *argv)
{
    pthread_detach(pthread_self());

    /*参数校验*/
    if(!argv){
        #if DEBUG
        cout<< "输入参数为空" <<endl;
        #endif
        exit(-0x01);
    }

    /*获取配置信息*/
    Json::Value jsonConf;
    if(JreadConf((const char *)argv, jsonConf)){
        #if DEBUG
        cout<<"采集-推送：读取配置文件失败"<<endl;
        #endif
        exit(-0x02);
    }

    /*运行*/
    for(;;){
        post_ports *task = (post_ports *)0x00;

        switch(jsonConf["post_type"].asInt()){
            case 0: task = new post_http(jsonConf["ip"].asString().c_str(), jsonConf["port"].asInt()); break;
            case 2: task = new post_tcpshort(jsonConf["ip"].asString().c_str(), jsonConf["port"].asInt()); break;
            case 1: task = new post_tcplong(jsonConf["ip"].asString().c_str(), jsonConf["port"].asInt()); break;
            default:
                #if DEBUG
                cout<<"采集-推送：推送方式有误"<<endl;
                #endif
                exit(-0x03);
                return NULL;
        }

        task->work();
        delete task; task = (post_ports *)0x00;
        sleep(3);
    }

    return (void *)0x00;
}
