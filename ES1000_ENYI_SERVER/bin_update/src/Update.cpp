#include "Update.h"
using namespace std;

unsigned char Update::status = 0x00;
pthread_mutex_t Update::lock;

/*
    函数名称：init
    函数功能：初始化
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-07-28
*/
int Update::init(){
    Update::status = Update::STATUS_START;
    return pthread_mutex_init(&Update::lock, NULL);
}

/*
    函数名称：set_status
    函数功能：设置升级状态标识
    传入参数：unsigned char _status  升级状态值
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-07-28
*/
void Update::set_status(unsigned char _status){
    pthread_mutex_lock(&Update::lock);
    Update::status = _status;
    pthread_mutex_unlock(&Update::lock);
}

/*
    函数名称：get_status
    函数功能：获取升级状态标示
    传入参数：无
    传出数据：unsigned char 升级状态值
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-07-28
*/
unsigned char Update::get_status(){
    unsigned char _status = 0x00;

    pthread_mutex_lock(&Update::lock);
    _status = Update::status;
    pthread_mutex_unlock(&Update::lock);

    return _status;
}

/*
    函数名称：work
    函数功能：系统升级
    传入参数：无
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-07-28
*/
int Update::work(){
    /*开始升级*/
    Update::set_status(Update::STATUS_START);

    /*升级中*/
    Update::set_status(Update::STATUS_WORK);
    int reg = lsystem(CMD_UPDATE);

    /*校验升级结果*/
    if(reg){ Update::set_status(Update::STATUS_FAILED); return -0x01; }
    else{ Update::set_status(Update::STATUS_END); return 0x00; }
}

/*
    函数名称：RUN
    函数功能：升级线程函数
    传入参数：void *arg  保留 NULL
    传出数据：保留 NULL
    注意事项：无
    编写人员：王凤龙
    编写时间：
*/
void* Update::RUN(void *arg){
    pthread_detach(pthread_self());
    arg = arg;

    Update::work();
    return NULL;
}