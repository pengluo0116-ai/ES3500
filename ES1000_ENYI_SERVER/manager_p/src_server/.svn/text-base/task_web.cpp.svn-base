#include "task_web.h"
using namespace std;
#define DEBUG 1

/*
    函数名称：task_web
    函数功能：构造函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-08-29
*/
task_web::task_web():task(WEB_NAME), open_num(0x00){
    this->open_maxNum = 20;         //最大开启次数
    this->open_spaceTime = 2;       //重开间隔时间
}

/*
    函数名称：~task_web
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-08-29
*/
task_web::~task_web(){}

/*
    函数名称：init
    函数功能：初始化函数
    传入参数：无
    传出数据：无
    注意事项：0 运行成功 非0 运行失败
    编写人员：王凤龙
    编写时间：2016-08-29
*/
int task_web::init(){
    /*清空日志*/
    lsystem(WEB_CMD_DELLOG);
    this->run_path = WEB_PATH;
    return 0x00;
}

/*
    函数名称：run_model
    函数功能：进程开启方式
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-08-29
*/
void task_web::run_model(){
    execlp(WEB_PATH, "","--config", WEB_CONF_PATH, NULL);
}

/*
    函数名称：run_status_close
    函数功能：任务状态截数据
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写人员：2016-09-02
*/
int task_web::run_status_close(){
    /*如果开启次数大于30次返回错误信息*/
    // if(this->open_num++ > 20) { lsystem( SYS_REBOOT_CONT ); return 0x01; } 

    return 0x00;
}