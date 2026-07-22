#include "lift_alarm.h"
using namespace std;
#define DEBUG 0

/*
    函数名称：lift_alarm
    函数功能：构造函数
    传入参数：
                const char *_lift_conf_path     电梯配置文件
                const char *_alarm_conf_path    报警配置文件
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-21
*/
lift_alarm::lift_alarm(const char *_lift_conf_path, const char *_alarm_conf_path):
lift_conf_path(string(_lift_conf_path)), alarm_conf_path(string(_alarm_conf_path)){
    this->isRun = 0x00;                                                                     //电梯是否开始工作

    *((unsigned char *)&this->alarm_status) = 0x00;                                         //报警状态清零

    this->status_stopDoor = 0x01;                                                           //停梯是否开门

    memset((void *)&this->alarm_conf, 0x00, sizeof(struct lift_alarm_conf));
    memset((void *)&this->lift_status, 0x00, sizeof(struct lift_real_info));
    memset((void *)&this->lift_status_real, 0x00, sizeof(struct lift_real_info));
}

/*
    函数名称：~lift_alarm
    函数功能：构造函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-21
*/
lift_alarm::~lift_alarm(){}

/*
    函数名称：init
    函数功能：初始化
    传入参数：无
    传出数据：
    注意事项：
                报警配置格式：
                {
                    "door":1,       //是否检测开关门
                    "person":1,     //是否有人体传感器
                    "sensor":1,     //是否检测上下极限

                    "timeout_kaceng":200,   //卡层超时时间
                    "timeout_door":200,     //停梯开门时间
                    "timeout_speed":200,    //最大超速时间
                    "timoout_person":200    //困人超时时间
                }
    编写人员：王凤龙
    编写时间：2016-11-21
*/
int lift_alarm::init(){
    /*读取电梯配置文件*/
    if(JreadConf(this->lift_conf_path.c_str(), this->jsonConf_lift)) return -0x01;

    /*读取报警配置文件*/
    Json::Value jsonConf_alarm;
    if(JreadConf(this->alarm_conf_path.c_str(), jsonConf_alarm)) return -0x02;

    /*配置校验*/
    if(this->conf_check(this->jsonConf_lift, jsonConf_alarm)) return -0x03;

    /*配置赋值*/
    this->alarm_conf.lift_floorNum = jsonConf_lift["floornum"].asInt();
    this->alarm_conf.isCk_door = jsonConf_alarm["door"].asInt();
    this->alarm_conf.isCk_person = jsonConf_alarm["person"].asInt();
    this->alarm_conf.isCk_sensor = jsonConf_alarm["up_down"].asInt();
    this->alarm_conf.timeOut_kaceng = jsonConf_alarm["timeout_kaceng"].asInt();
    this->alarm_conf.timeOut_door = jsonConf_alarm["timeout_door"].asInt();
    this->alarm_conf.timeOut_speed = jsonConf_alarm["timeout_speed"].asInt();
    this->alarm_conf.timeOut_person = jsonConf_alarm["timeout_person"].asInt();
    this->alarm_conf.timeOut_jx = jsonConf_alarm["timeout_jx"].asInt();
    this->alarm_conf.timeOut_rt = jsonConf_alarm["timeout_rt"].asInt();

    this->lift_id = jsonConf_alarm["id"].asString();
    this->app_key = jsonConf_alarm["appkey"].asString();

    /*停梯计时初始化*/
    time(&this->openDoor_time);

    /*困人计时初始化*/
    time(&this->kunren_time);

    /*极限报警持续时间初始化*/
    this->timerun_jx_down = this->timerun_jx_up = (time_t)0x00;

    /*人体感应持续时间初始化*/
    this->timerun_rt = (time_t)0x00;

    return 0x00;
}

/*
    函数名称：conf_check
    函数功能：读获取的配置数据进行校验
    传入参数：
                Json::Value &_jsonConf_lift     电梯配置数据
                Json::Value &_jsonConf_alarm    报警配置数据
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-22
*/
int lift_alarm::conf_check(Json::Value &_jsonConf_lift, Json::Value &_jsonConf_alarm){
    return 0x00;
}

/*
    函数名称：get_realData
    函数功能：获取实时数据
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-21
*/
int lift_alarm::get_realData(){
    //获取实时数据

    /*开始工作，实时数据赋值到备份数据*/
    if(!this->isRun){ memcpy((void *)&this->lift_status_real, (void *)&this->lift_status, sizeof(struct lift_real_info)); this->isRun = 0x01;}

    return 0x00;
}

/*
    函数名称：ck_kaceng
    函数功能：卡层判断
    传入参数：无
    传出数据：
    注意事项：判断依据-非平层状态下超过卡层超时时间
    编写人员：王凤龙
    编写时间：2016-11-21
*/
int lift_alarm::ck_kaceng(){
    /*状态清空*/
    this->alarm_status.kaceng = 0x00;

    /*平层判断*/
    if(!this->lift_status_real.status) return 0x00;

    /*卡层判断*/
    if(this->lift_status_real.runTime/10 >= this->alarm_conf.timeOut_kaceng) this->alarm_status.kaceng = 0x01;

    return 0x00;
}

/*
    函数名称：ck_upAlarm
    函数功能：冲顶判断
    传入参数：无
    传出数据：
    注意事项：判断依据-当电梯在顶端楼层时，继续向上时视为冲顶
    编写人员：王凤龙
    编写时间：2016-11-21
*/
int lift_alarm::ck_upAlarm(){
    /*清空状态*/
    this->alarm_status.up = 0x00;

    /*判定是否检测上下极限*/
    if(!this->alarm_conf.isCk_sensor) return 0x00;
    if(this->alarm_conf.isCk_sensor == 0x02) return 0x00;   //复用为开关门

    /*判断是否继续上行并且触发级性传感器*/
    if(this->lift_status_real.sensor.limit && this->lift_status_real.status == 0x01 ) time(&this->timerun_jx_up);
    if((unsigned int)difftime(time(NULL), this->timerun_jx_up) < this->alarm_conf.timeOut_jx) this->alarm_status.up = 0x01;

    return 0x00;
}

/*
    函数名称：ck_downAlarm
    函数功能：蹲底判断
    传入参数：无
    传出数据：
    注意事项：判断依据-当电梯在底端楼层时，继续向下时视为蹲底
    编写人员：王凤龙
    编写时间：2016-11-21
*/
int lift_alarm::ck_downAlarm(){
    /*清空状态*/
    this->alarm_status.down = 0x00;

    /*判定是否检测上下极限*/
    if(!this->alarm_conf.isCk_sensor) return 0x00;
    if(this->alarm_conf.isCk_sensor == 0x02) return 0x00;   //复用为开关门

    /*判断是否继续下行并且触发级性传感器*/
    if(this->lift_status_real.sensor.limit && this->lift_status_real.status == 0x02) time(&this->timerun_jx_down);
    if((unsigned int)difftime(time(NULL), this->timerun_jx_down) < this->alarm_conf.timeOut_jx) this->alarm_status.down = 0x01;

    return 0x00;
}

/*
    函数名称：ck_rundoor
    函数功能：开门走梯判断
    传入参数：无
    传出数据：
    注意事项：判断依据--电梯在不平层的状态下，门开
    编写人员：王凤龙
    编写时间：2016-11-21
*/
int lift_alarm::ck_rundoor(){
    /*清空状态*/
    this->alarm_status.rundoor = 0x00;

    /*是否检测电梯门*/
    if(!this->alarm_conf.isCk_door) return 0x00;

    /*判断电梯是否平层*/
    if(!this->lift_status_real.status) return 0x00;

    /*开门检测--开门，并且楼层传感器无遮挡*/
    if(this->lift_status_real.sensor.door && !this->lift_status_real.sensor.lsensor) this->alarm_status.rundoor = 0x01;

    return 0x00;
}

/*
    函数名称：ck_speed
    函数功能：超速判断
    传入参数：无
    传出数据：
    注意事项：判断依据-当楼层改变时，并且前一包数据为非平层状态，前一包数据状态保持时间小于超速时间
    编写人员：王凤龙
    编写时间：2016-11-21
*/
#if 0
int lift_alarm::ck_speed(){
    /*状态清空*/
    this->alarm_status.speed = 0x00;

    /*判定楼层是否改变*/
    if(this->lift_status_real.floor == this->lift_status.floor) return 0x00;

    /*判断上一包数据是否平层*/
    if(!this->lift_status.status) return 0x00;

    /*判断是否超速*/
    if(this->lift_status.runTime * 10 < this->alarm_conf.timeOut_speed) {
        this->alarm_status.speed = 0x01;
        
        #if DEBUG
        cout<<"上层/时间:=====" << (unsigned int)this->lift_status.floor << "===" << (unsigned int)this->lift_status.runTime <<endl;
        cout<<"超速时间设定：" << (unsigned int)(this->alarm_conf.timeOut_speed) << endl;
        cout<<"本层====" << (unsigned int)this->lift_status_real.floor << endl;
        #endif
    }
    return 0x00;
}
#endif
int lift_alarm::ck_speed(){
    /*状态清空*/
    //this->alarm_status.speed = 0x00;

    /*判定楼层是否改变*/
    if(this->lift_status_real.floor == this->lift_status.floor){
        /*当电梯到达平层或者电梯运行时时间超过超速阈值，取消超速报警*/
        if(!this->lift_status_real.status || (this->lift_status_real.runTime  > this->alarm_conf.timeOut_speed) ) this->alarm_status.speed = 0x00;
        return 0x00;
    }

    /*判断上一包数据是否平层*/
    if(!this->lift_status_real.status){ this->alarm_status.speed = 0x00; return 0x00; }

    /*判断是否超速*/
    if(this->lift_status.runTime  < this->alarm_conf.timeOut_speed) {
        this->alarm_status.speed = 0x01;
        
        #if DEBUG
        cout<<"上层/时间:=====" << (unsigned int)this->lift_status.floor << "===" << (unsigned int)this->lift_status.runTime <<endl;
        cout<<"超速时间设定：" << (unsigned int)(this->alarm_conf.timeOut_speed) << endl;
        cout<<"本层====" << (unsigned int)this->lift_status_real.floor << endl;
        #endif
    }
    return 0x00;
}

/*
    函数名称：ck_power
    函数功能：掉电检测
    传入参数：无
    传出数据：
    注意事项：保留 空函数
    编写人员：王凤龙
    编写时间：2016-11-21
*/
int lift_alarm::ck_power(){
    /*状态清空*/
    this->alarm_status.power = 0x00;
    return 0x00; 
}

/*
    函数名称：ck_stopDoor
    函数功能：检测停梯是否开门
    传入参数：无
    传出数据：
    注意事项：判断依据-电梯由非平层状态到平层状态后，判断是有有一次开门
    编写人员：王凤龙
    编写时间：2016-11-21
*/
int lift_alarm::ck_stopDoor(){
    /*清空状态*/
    this->alarm_status.stop_door = 0x00;

    /*开关门检测*/
    if(!this->alarm_conf.isCk_door) return 0x00;

    /*判断是否平层*/
    if(this->lift_status_real.status) return 0x00;

    /*判断上一包是否在非平层*/
    if(this->lift_status.status){ 
        time(&this->openDoor_time);                     //开门计时
        this->status_stopDoor = 0x00; return 0x00; 
    }

    /*开门检测*/
    if(this->lift_status_real.sensor.door){ this->status_stopDoor = 0x01; return 0x00; }

    /*超时检测*/
    if((unsigned int)difftime(time(NULL), this->openDoor_time) >= this->alarm_conf.timeOut_door && !this->status_stopDoor){
        this->alarm_status.stop_door = 0x01;
        // this->alarm_status.kaceng = 0x01;       //卡层
    }

    return 0x00;
}

/*
    函数名称：ck_kunren
    函数功能：困人检测
    传入参数：无
    传出数据：
    注意事项：检测依据，在关门的状态下，持续有人时间超过指定时间视为困人；
    编写人员：王凤龙
    编写时间：2016-12-10
*/
int lift_alarm::ck_kunren(){
    /*清空状态*/
    this->alarm_status.kunren = 0x00;

    /*有无人检测是否使能*/
    if(!this->alarm_conf.isCk_person) return 0x00;

    /*人体检测持续计算*/
    if(this->lift_status_real.sensor.person) time(&this->timerun_rt);
    if((unsigned int)difftime(time(NULL), this->timerun_rt) < this->alarm_conf.timeOut_rt) this->lift_status_real.sensor.person = 0x01;

    /*在平层开门或者没人的状态下，开始计时*/
    if(( !this->lift_status_real.status && this->lift_status_real.sensor.door ) || !this->lift_status_real.sensor.person){ time(&this->kunren_time); return 0x00; }

    /*检测是否困人*/
    if((unsigned int)difftime(time(NULL), this->kunren_time) >= this->alarm_conf.timeOut_person*60){ this->alarm_status.kunren = 0x01; return 0x00; }

    return 0x00;
}

/*
    函数名称：check_alarm
    函数功能：检测报警
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-21
*/
int lift_alarm::check_alarm(){
    this->ck_kaceng();
    this->ck_upAlarm();
    this->ck_downAlarm();
    this->ck_rundoor();
    this->ck_speed();
    this->ck_power();
    this->ck_stopDoor();
    this->ck_kunren();

    this->lift_status = this->lift_status_real;

    return 0x00;
}

/*数组转字符串*/
string numTostr(int _num){
    ostringstream ostr; ostr.str("");

    ostr << _num;

    return ostr.str();
}

/*
    函数名称：post_alarm
    函数功能：组装并且发送报警信息
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-21
*/
// int tmpnum = 0x01;
int lift_alarm::post_alarm(){
    Json::Value jsonData;

    /*基本信息*/
    Json::Value json_Parameters;
    json_Parameters["ID"] = this->lift_id;
    // json_Parameters["OSD"] = this->osd_str;

    /*临时添加*/
    // ostringstream ostr; ostr.str("");
    // ostr << tmpnum++; if(tmpnum > 250) tmpnum = 0x01;
    // json_Parameters["Number"] = ostr.str();
    
    
    json_Parameters["appkey"] = this->app_key;

    /*实时数据*/
    Json::Value json_State;
    json_State["DS_DX"] = atoi(this->jsonConf_lift["floor"][this->lift_status_real.floor].asString().c_str()) <= 0 ? "0" : "1";
    
    ostringstream ostrtmp; ostrtmp.str("");
    if(atoi(this->jsonConf_lift["floor"][this->lift_status_real.floor].asString().c_str()) < 0x00) ostrtmp << atoi(this->jsonConf_lift["floor"][this->lift_status_real.floor].asString().c_str()) * -1;
    else ostrtmp << this->jsonConf_lift["floor"][this->lift_status_real.floor].asString().c_str();
    json_State["Lou_Ceng"] = ostrtmp.str(); //此处需要变化
    json_State["status"] = numTostr(this->lift_status_real.status);
    json_State["Door"] = this->alarm_conf.isCk_door ? numTostr(this->lift_status_real.sensor.door) : "0";
    
    json_State["People"] = this->alarm_conf.isCk_person ? numTostr(this->lift_status_real.sensor.person) : "0";
    json_State["G_X"] = "0";
    json_State["G_Y"] = "0";
    json_State["G_Z"] = "0";

    /*报警信息*/
    Json::Value json_Alarm;
    json_Alarm["Y_N"] = numTostr((*((unsigned char *)&this->alarm_status) & 0x7F) ? 1 : 0);
    json_Alarm["Kaceng"] = numTostr(this->alarm_status.kaceng);
    json_Alarm["Kunren"] = numTostr(this->alarm_status.kunren);
    json_Alarm["UP_Alarm"] = numTostr(this->alarm_status.up);
    json_Alarm["Down_Alarm"] = numTostr(this->alarm_status.down);
    json_Alarm["Run_door"] = numTostr(this->alarm_status.rundoor);
    json_Alarm["Speeding"] = numTostr(this->alarm_status.speed);
    json_Alarm["Power"] = numTostr(this->alarm_status.power);

    /*组装数据*/
    jsonData["Parameters"] = json_Parameters;
    jsonData["State"] = json_State;
    jsonData["Alarm"] = json_Alarm;

    /*发送数据*/
    Json::FastWriter fast_writer;
    string jsonStr = fast_writer.write(jsonData);
    lift_vector::put_msg(jsonStr);

    return 0x00;
}

/*
    函数名称：add_realData
    函数功能：添加实时信息
    传入参数：struct lift_real_info  *_lift_status_real 实时信息
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-22
*/
int lift_alarm::add_realData(struct lift_real_info  *_lift_status_real, const char *_osd_str){
    /*参数校验*/
    if(!_lift_status_real) return -0x01;

    /*判断极限是否为双开门*/
    if(this->alarm_conf.isCk_sensor == 0x02){
        _lift_status_real->sensor.door = _lift_status_real->sensor.door || (_lift_status_real->sensor.limit ? 0 : 1);    
        _lift_status_real->sensor.limit = 0x00;
    }

    /*赋值实时数据*/
    this->lift_status_real = *_lift_status_real;
    this->osd_str = string(_osd_str);

    /*开始工作，实时数据赋值到备份数据*/
    if(!this->isRun){ memcpy((void *)&this->lift_status_real, (void *)&this->lift_status, sizeof(struct lift_real_info)); this->isRun = 0x01;}

    /*报警校验*/
    if(this->check_alarm()) return -0x02;

    /*推送报警信息*/
    return this->post_alarm();
}