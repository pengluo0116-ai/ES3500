#include "task_qt.h"
using namespace std;
#define DEBUG 1

/*
    函数名称：task_qt
    函数功能：构造函数
    传入参数：
                const char *_task_name      进程的名称
                const char _run_path        进程路径
                const char *_conf_info      进程参数
                int _open_maxNum            最大开启次数
                int _open_spaceTime         开启间隔
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-15
*/
task_qt::task_qt(const char *_task_name, const char *_run_path, const char *_conf_info, int _open_maxNum = 0, int _open_spaceTime = 0):
task(_task_name, _run_path, _conf_info, _open_maxNum, _open_spaceTime){}

/*
    函数名称：~task_qt
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-15
*/
task_qt::~task_qt(){}

/*
    函数名称：run_model
    函数功能：进程开启方式
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-15
*/
void task_qt::run_model(){
    execlp(this->run_path.c_str(), "", this->conf_info.c_str(), NULL);
}