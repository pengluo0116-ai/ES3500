#include "task_udpserver.h"
using namespace std;

#define DEBUG 1

int task_udpserver::RunStatus = 0x00;
pthread_mutex_t task_udpserver::lock;

/*
    函数名称：task_udpserver
    函数功能：构造函数
    传入参数：
                const char *_task_name 进程的名称
                const char _run_path        进程路径
                const char *_conf_info      进程参数
                int _open_maxNum            最大开启次数
                int _open_spaceTime         开启间隔
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-09-12
*/
task_udpserver::task_udpserver(const char *_task_name, const char *_run_path, const char *_conf_info, int _open_maxNum, int _open_spaceTime):
task(_task_name, _run_path, _conf_info, _open_maxNum, _open_spaceTime){
    pthread_mutex_init(&task_udpserver::lock, NULL);
}

/*
    函数名称：~task_udpserver
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-09-12
*/
task_udpserver::~task_udpserver(){}

/*
    函数名称：setUdpserver_RunStatus
    函数功能：设置udpserver运行开启关闭状态
    传入参数：int _status_num 状态值
    传出数据：0 运行成功 非0 运行失败
    注意事项：静态函数
    编写人员：王凤龙
    编写时间：2016-09-12
*/
int task_udpserver::setUdpserver_RunStatus(int _status_num){
    pthread_mutex_lock(&task_udpserver::lock);
    task_udpserver::RunStatus = _status_num;
    pthread_mutex_unlock(&task_udpserver::lock);

    return 0x00;
}

/*
    函数名称：getUdpserver_RunStatus
    函数功能：获取udpserver运行开启关闭状态
    传入参数：int &_status_num 获取到的状态值会存储到该引用所指向的存储变量中
    传出数据：0 运行成功 非0 运行失败
    注意事项：静态函数
    编写人员：王凤龙
    编写时间：2016-09-12
*/
int task_udpserver::getUdpserver_RunStatus(int &_status_num){
    int err = 0x00;

    pthread_mutex_lock(&task_udpserver::lock);
    try{ _status_num = task_udpserver::RunStatus; }catch(...){ err = 0x01; }
    pthread_mutex_unlock(&task_udpserver::lock);

    return 0x00;
}

/*
    函数名称：init
    函数功能：任务初始化函数
    传入参数：无
    传出数据：默认 0  udpserver需要关闭 1
    注意事项：接口函数实现
    编写人员：王凤龙
    编写时间：2016-09-12
*/
int task_udpserver::init(){
    /*获取开关运行状态*/
    int _status = 0x00;
    if(getUdpserver_RunStatus(_status)) return -0x01;
    if(_status){
        #if DEBUG
        cout<<"设置udpserver关闭"<<endl;
        #endif
        return 0x01;
    }

    return 0x00;
}