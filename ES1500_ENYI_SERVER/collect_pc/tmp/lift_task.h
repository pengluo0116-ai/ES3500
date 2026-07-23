#ifndef __LIFT_TASK_H__
#define __LIFT_TASK_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>

#include "conf.h"
#include "shm_cont.h"
#include "stc_cont.h"
#include "jsonConfFile.h"
#include "tcp_sock_client.h"
#include "tcp_sock_pakage.h"
#include "lift_alarm.h"
#include "../include/json/json.h"

class lift_task{
    private:
        std::string lift_conf_path;      //配置文件路径
        std::string alarm_conf_path;    //报警配置文件路径
        Json::Value jsonConf;           //json格式的配置信息
        lift_alarm *alarm_cont;         //报警处理

        stc_cont *stc_uart;             //串口控制
        shm_cont *str_shm;              //共享变量

        int floor_num;                  //楼层数

        struct lift_real_info pkg_bak;     //实时数据备份
        int pak_cf;                     //实时数据重复次数

        ostringstream osdstr;

    public:
        lift_task(const char *_lift_conf_path, const char *_alarm_conf_path);
        ~lift_task();

    private:
        int _cmd_restore();                                                 //还原出厂设置
        int _cmd_liftCont(struct stc_package *pkg);                         //电梯实时处理，包含报警处理
        int cmd_contrl(struct stc_package *pkg);

    public:
        int init();
        int RUN();
};

#endif