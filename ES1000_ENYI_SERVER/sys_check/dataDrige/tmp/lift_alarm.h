#ifndef __LIFT_ALARM_H__
#define __LIFT_ALARM_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <time.h>
#include <unistd.h>
#include <sstream>

#include "lift_vector.h"
#include "jsonConfFile.h"
#include "../include/json/json.h"

/*
    文档介绍：
    从实时数组中接收数据，区分信息为正常信息还是报警信息，并组装成发送报文体
*/

/***************************************************************************************/
/*传感器状态数据结构体*/
struct lift_real_alarm{
    unsigned char person:1;                             //有无人  有(0x01) 无(0x00)
    unsigned char door:1;                               //开关门  开(0x01) 关(0x00)
    unsigned char limit:1;                              //上行极限 有(0x01) 无(0x00)
    unsigned char lsensor:2;                            //楼层传感器上的两个光电管，无遮挡(00), 非0(有遮挡)
    unsigned char revd:3;                               //保留
};

/*电梯实时状态==与底层数据接收一致*/
struct lift_real_info{
    unsigned char floor;                                //楼层
    unsigned char status;                               //上下行   上(0x01) 下(0x02) 停(0x00)
    unsigned char runTime;                              //运行时间 上下行状态下的时间
    struct lift_real_alarm sensor;                     //传感器
};

/*报警配置参数*/
struct lift_alarm_conf{
    unsigned char lift_floorNum;                        //总楼层数
    unsigned char isCk_door;                            //是否检测开关门               不检测(0x00) 检测(0x01)
    unsigned char isCk_person;                          //是否检测人体传感器           不检测(0x00) 检测(0x01)
    unsigned char isCk_sensor;                          //是否检测上下极限             不检测(0x00) 检测(0x01)

    unsigned char timeOut_kaceng;                       //卡层超时时间
    unsigned char timeOut_door;                         //停梯开门超时时间
    unsigned char timeOut_speed;                        //最大超速时间
    unsigned char timeOut_person;                       //困人判定超时时间
    unsigned char timeOut_jx;                           //极限持续时间
    unsigned char timeOut_rt;                           //人体检测持续时间
};

/*电梯报警状态*/
struct lift_alarm_status{
    unsigned char kaceng:1;                             //卡层
    unsigned char kunren:1;                             //困人
    unsigned char up:1;                                 //冲顶
    unsigned char down:1;                               //蹲底
    unsigned char rundoor:1;                            //开门走梯
    unsigned char speed:1;                              //超速
    unsigned char power:1;                              //断电
    unsigned char stop_door:1;                          //停梯开门
};
/**************************************************************************************/



class lift_alarm{
    private:
        std::string lift_conf_path;                     //电梯配置文件路径
        std::string alarm_conf_path;                    //电梯报警文件配置路径
        
        std::string server_ip;                          //发送服务器IP
        int server_port;                                //发送服务器端口
        std::string lift_id;                            //设备ID
        std::string app_key;                            //设备APP_KEY
        std::string osd_str;                            //设备叠加字符串

        struct lift_alarm_conf alarm_conf;              //配置参数
        Json::Value jsonConf_lift;                      //json格式的电梯配置参数
        unsigned char isRun;                            //判定是否工作 0无 1工作

        struct lift_real_info   lift_status;            //电梯状态
        struct lift_real_info   lift_status_real;       //实时电梯状态
        struct lift_alarm_status    alarm_status;       //报警状态
        
        unsigned char status_stopDoor;                  //停梯是否开门
        time_t openDoor_time;                           //开门日期时间 
        time_t kunren_time;                             //困人检测日期时间 有人的状态持续时长

        time_t timerun_jx_up;                           //极限报警持续时间 冲顶
        time_t timerun_jx_down;                         //极限报警持续时间 蹲底
        time_t timerun_rt;                              //人体感应持续时间
        
    public:
        lift_alarm(const char *_lift_conf_path, const char *_alarm_conf_path);
        ~lift_alarm();

    private:
        int ck_kaceng();                                                    //卡层判断
        int ck_upAlarm();                                                   //冲顶判断
        int ck_downAlarm();                                                 //蹲底判断
        int ck_rundoor();                                                   //开门走梯判断
        int ck_speed();                                                     //超速判断
        int ck_power();                                                     //电梯掉电检测
        int ck_stopDoor();                                                  //停梯开门检测
        int ck_kunren();                                                    //困人检测

    private:
        int conf_check(Json::Value &_jsonConf_lift, Json::Value &_jsonConf_alarm);  //配置参数校验
        int get_realData();                                                 //获取实时数据
        
        int check_alarm();                                                  //分析报警信息
        int post_alarm();                                                   //组装并且发送报警信息

    public:
        int init();                                                         //初始化
        int add_realData(struct lift_real_info  *_lift_status_real, const char *_osd_str);                                                 //添加实时数据，自动解析报警
};

#endif
