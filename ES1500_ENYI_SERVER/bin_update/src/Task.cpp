#include "Task.h"
using namespace std;
#define DEBUG 1

string Task::CMD_RADIO_UPDATE_START     = "mplayer " RADIO_DPATH "start.mp3";
string Task::CMD_RADIO_UPDATE_END       = "mplayer " RADIO_DPATH "end.mp3";
string Task::CMD_RADIO_UPDATE_WORK      = "mplayer " RADIO_DPATH "working.mp3";
string Task::CMD_RADIO_UPDATE_FAILED    = "mplayer " RADIO_DPATH "updatefail.mp3";
string Task::CMD_RADIO_UPDATE_RESTART   = "mplayer " RADIO_DPATH "updaterestart.mp3";

/*
    函数名称：_update_one
    函数功能：升级系统一次
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-07-29
*/
int Task::_update_one(){
    /*开始升级*/
    lsystem(Task::CMD_RADIO_UPDATE_START.c_str());

    /*升级*/
    pthread_t ptmp;
    pthread_create(&ptmp, NULL, Update::RUN, NULL);

    for(;;){
        sleep(1);
        switch(Update::get_status()){
            case Update::STATUS_WORK    : lsystem(Task::CMD_RADIO_UPDATE_WORK.c_str()); sleep(10); break;
            case Update::STATUS_END     : lsystem(Task::CMD_RADIO_UPDATE_END.c_str()); return 0x00;
            case Update::STATUS_FAILED  : lsystem(Task::CMD_RADIO_UPDATE_FAILED.c_str()); return -0x01;
        }
    }

    return 0x00;
}

/*
    函数名称：update
    函数功能：升级
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-07-29
*/
void Task::update(){
    Update::init();

    for(;;){
        if(Task::_update_one()){ lsystem(Task::CMD_RADIO_UPDATE_RESTART.c_str()); sleep(1); continue; }
        break;
    }

    for(;;){
        sleep(2);
        lsystem(Task::CMD_RADIO_UPDATE_END.c_str()); 
    }
}