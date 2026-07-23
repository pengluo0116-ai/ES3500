#include "radio.h"
using namespace std;
#define DEBUG 1

/*
    函数名称：radioInfo
    函数功能：构造函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写世家：2017-05-20
*/
radioInfo::radioInfo(){
    this->id = "";
    this->dev_name = "";
    this->rad_name = "";

    this->gid = 0;
    this->gname = "";
    this->rad_gname = "";

    this->pgid = 0;
    this->pgname = "";
    this->rad_pgname = "";

    this->cmd_alarmRadio = "";

    *((unsigned char *)&this->status) = 0x00;
    *((unsigned char *)&this->cmd) = 0x00;
    this->cmd.disconnect = 0x01;
}

/*
    函数名称：~radioInfo
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-20
*/
radioInfo::~radioInfo(){}

/*******************************************************************************/

/*radio类变量初始化*/
pthread_mutex_t radio::lock;
pthread_mutex_t radio::dataLock;
int radio::playState = 0;                       //默认为关闭状态
std::vector<radioInfo *> radio::liftList;

Json::Reader radio::JReader;
Json::Value radio::json_liftListReal;
Json::Value radio::json_liftListInfo;
std::string radio::json_liftListReal_str = "";
std::string radio::json_liftListInfo_str = "";

std::ostringstream radio::cmd_play;

/*
    函数名称：init
    函数功能：构造函数
    传入参数：无
    传出数据：
                 0  运行成功
                -1  运行有误--锁初始化失败
                -2  运行有误--数据锁初始化失败
    注意事项：使用该类前，需确认已经执行了该函数，该函数只能运行一次
    编写人员：王凤龙
    编写时间：2017-05-20
*/
int radio::init(){
    /*清空电梯列表*/
    vector<radioInfo *>::iterator it;
    for(it=radio::liftList.begin(); it!=radio::liftList.end(); it++){ delete (*it); }
    radio::liftList.clear();

    /*电梯语音关闭使能*/
    radio::playState = 0x01;

    /*初始化锁*/
    if(pthread_mutex_init(&radio::lock, NULL)) return -0x01;
    if(pthread_mutex_init(&radio::dataLock, NULL)) return -0x02;
    return 0x00;
}

/*
    函数名称：ON
    函数功能：开启播放器
    传入参数：无
    传出数据：保留 0x00
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-20
*/
int radio::ON(){
    pthread_mutex_lock(&radio::lock);
    radio::playState = 0x01;
    pthread_mutex_unlock(&radio::lock);

    return 0x00;
}

/*
    函数名称：OFF
    函数功能：关闭播放器
    传入参数：无
    传出数据：保留 0x00
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-20
*/
int radio::OFF(){
    pthread_mutex_lock(&radio::lock);
    radio::playState = 0x00;
    lsystem("kill -9 `pidof mplayer`");
    pthread_mutex_unlock(&radio::lock);

    return 0x00;
}

/*
    函数名称：get_playStatus
    函数功能：获取播放器播放状态
    传入参数：无
    传出数据：0 关闭 1开启
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-22
*/
int radio::get_playStatus(){
    int status;
    pthread_mutex_lock(&radio::lock);
    status = radio::playState;
    pthread_mutex_unlock(&radio::lock);
    return status;
}

/*
    函数名称：liftInfo_update
    函数功能：单台设备信息刷新
    传入参数：Json::Value &jsonItem  
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-20
*/
int radio::liftInfo_update(Json::Value &jsonItem){
    radioInfo *item = NULL;
    
    /*校验已经存在的设备信息是否有修改*/
    for(unsigned int point=0x00; point<radio::liftList.size(); point++){
        item = *(radio::liftList.begin() + point);
        if(item->id != jsonItem["id"].asString()) continue;
        
        /*设备名称校验*/
        item->cmd.del = 0x00;
        if(item->dev_name != jsonItem["dev_name"].asString()){
            item->dev_name = jsonItem["dev_name"].asString();
            cmd5_str((const unsigned char *)item->dev_name.c_str(), item->dev_name.size(), item->rad_name);
            item->rad_name += ".mp3";
        }

        /*设备父组校验*/
        if(item->gid != jsonItem["gid"].asInt() || item->gname != jsonItem["gname"].asString()){
            item->gid = jsonItem["gid"].asInt();
            item->gname = jsonItem["gname"].asString();
            
            item->rad_gname = "";
            if(item->gname.size() > 0x00){
                cmd5_str((const unsigned char *)item->gname.c_str(), item->gname.size(), item->rad_gname);
                item->rad_gname += ".mp3";
            }
        }

        /*设备祖先组校验*/
        if(item->pgid != jsonItem["pgid"].asInt() || item->pgname != jsonItem["pgname"].asString()){
            item->pgid = jsonItem["pgid"].asInt();
            item->pgname = jsonItem["pgname"].asString();

            item->rad_pgname = "";
            if(item->pgname.size() > 0x00){
                cmd5_str((const unsigned char *)item->pgname.c_str(), item->pgname.size(), item->rad_pgname);
                item->rad_pgname += ".mp3";
            }
        }
        return 0x00;
    }

    /*添加信息设备*/
    item = new radioInfo();
    item->id = jsonItem["id"].asString();
    item->dev_name = jsonItem["dev_name"].asString();
    cmd5_str((const unsigned char *)item->dev_name.c_str(), item->dev_name.size(), item->rad_name);
    item->rad_name += ".mp3";

    item->gid = jsonItem["gid"].asInt();
    item->gname = jsonItem["gname"].asString();
    item->rad_gname = "";
    if(item->gname.size() > 0x00){
        cmd5_str((const unsigned char *)item->gname.c_str(), item->gname.size(), item->rad_gname);
        item->rad_gname += ".mp3";
    }

    item->pgid = jsonItem["pgid"].asInt();
    item->pgname = jsonItem["pgname"].asString();
    item->rad_pgname = "";
    if(item->pgname.size() > 0x00){
        cmd5_str((const unsigned char *)item->pgname.c_str(), item->pgname.size(), item->rad_pgname);
        item->rad_pgname += ".mp3";
    }

    radio::liftList.push_back(item);
    return 0x00;
}

/*
    函数名称：liftReal_update
    函数名称：单台设备实时信息刷新
    传入参数：Json::Value &jsonItem
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-20
*/
int radio::liftReal_update(Json::Value &jsonItem){
    radioInfo *item = NULL;
    unsigned long dt_now=0x00, dt_tmp=0x00;
    
    cpp_datetimer::now_sec(&dt_now);                                                                //获取当前时间
    for(unsigned int point=0x00; point<radio::liftList.size(); point++){
        item = *(radio::liftList.begin() + point);
        if(item->id != jsonItem["Parameters"]["ID"].asString()) continue;

        item->status.Power         = atoi(jsonItem["Alarm"]["Power"].asString().c_str());
        item->status.Down_Alarm    = atoi(jsonItem["Alarm"]["Down_Alarm"].asString().c_str());
        item->status.Up_Alarm      = atoi(jsonItem["Alarm"]["UP_Alarm"].asString().c_str());
        item->status.Kunren        = atoi(jsonItem["Alarm"]["Kunren"].asString().c_str());
        item->status.Speeding      = atoi(jsonItem["Alarm"]["Speeding"].asString().c_str());
        item->status.Run_door      = atoi(jsonItem["Alarm"]["Run_door"].asString().c_str());
        item->status.Kaceng        = atoi(jsonItem["Alarm"]["Kaceng"].asString().c_str());
        item->status.Door_unclose  = atoi(jsonItem["Alarm"]["Stp_DoorUnclose"].asString().c_str());
        item->status.Door_unopen   = atoi(jsonItem["Alarm"]["Stp_DoorUnopen"].asString().c_str());
        item->status.Reapet        = atoi(jsonItem["Alarm"]["Stp_Reapet"].asString().c_str());
        item->status.Slant_lr      = atoi(jsonItem["Alarm"]["Slant_lr"].asString().c_str());
        item->status.Slant_fb      = atoi(jsonItem["Alarm"]["Slant_fb"].asString().c_str());
        item->status.Shake         = atoi(jsonItem["Alarm"]["Amplitude"].asString().c_str());
        item->status.Manaual       = atoi(jsonItem["Alarm"]["Manual_Alarm"].asString().c_str());
        item->status.Voltage       = atoi(jsonItem["Alarm"]["Voltage"].asString().c_str());
        item->status.Power_down    = atoi(jsonItem["Alarm"]["Power_Down"].asString().c_str());
        item->status.Electromobile = atoi(jsonItem["Alarm"]["Electromobile"].asString().c_str());
        item->status.longtime      = atoi(jsonItem["Alarm"]["Maintain"].asString().c_str());
        item->status.Y_N           = atoi(jsonItem["Alarm"]["Y_N"].asString().c_str());

        if(cpp_datetimer::stringToDatetime_sec(jsonItem["Parameters"]["Recv_time"].asString().c_str(), &dt_tmp)){
            item->cmd.disconnect = 0x01;
            return 0x00;
        }

        if((long)(dt_now-dt_tmp) < DEV_ONLINE_TIMEOUT){ item->cmd.disconnect = 0x00; }
        else{ item->cmd.disconnect = 0x01; }

        return 0x00;
    }

    return 0x00;
}

/*
    函数名称：lift_checkDel
    函数功能：检测指定的设备是否删除，并打入标志
    传入参数：radioInfo *item   设备节点指针
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-20
*/
int radio::lift_checkDel(radioInfo *item){
    /*参数校验--内部函数，无需校验*/

    /*检测是否被删除*/
    for(unsigned int point=0x00; point<radio::json_liftListInfo.size(); point++){
        if(item->id == radio::json_liftListInfo[point]["id"].asString()) return 0x00;
    }

    /*打印删除标志*/
    item->cmd.del = 0x01;
    return 0x00;
}

/*
    函数名称：liftList_checkDel
    函数功能：检测多余的设备，并打入标志
    传入参数：无
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-20
*/
int radio::liftList_checkDel(){
    for(unsigned int point=0x00; point<radio::liftList.size(); point++){
        radio::lift_checkDel(*(radio::liftList.begin() + point));
    }

    return 0x00;
}

/*
    函数名称：liftList_clear
    函数功能：去除多余设备信息节点
    传入参数：无
    传出数据：无
    注意事项：该函数需要在播放完一个周期后，再能执行，否则会有偶然的可能出现溢出问题
    编写人员：王凤龙
    编写时间：2017-05-20
*/
int radio::liftList_clear(){
    radioInfo *item = NULL;
    for(int point=radio::liftList.size()-1; point>=0x00; point--){
        item = *(radio::liftList.begin() + point);
        if(!item->cmd.del) continue;

        delete item;
        radio::liftList.erase(radio::liftList.begin()+point);
    }

    return 0x00;
}

/*
    函数名称：liftInfoList_update
    函数功能：设备信息列表刷新
    传入参数：无
    传出数据：
                 0  运行正确
                -1  运行有误
    注意事项：调用该函数，可以不用关心执行结果
    编写人员：王凤龙
    编写时间：2017-05-20
*/
int radio::liftInfoList_update(){
    /*获取电梯设备信息*/
    real_cont::getStr_devInfo_All(radio::json_liftListInfo_str);
    if(radio::json_liftListInfo_str.size() <= 0x00) return 0x00;
    
    /*数据转换成JSON*/
    radio::json_liftListInfo.clear();
    if(!radio::JReader.parse(radio::json_liftListInfo_str, radio::json_liftListInfo)) return -0x01;

    /*更新信息*/
    for(unsigned int i=0x00; i<radio::json_liftListInfo.size(); i++){
        radio::liftInfo_update(radio::json_liftListInfo[i]);
    }

    return 0x00;
}

/*
    函数名称：liftRealList_update
    函数功能：设备报警信息列表刷新
    传入参数：无
    传出数据：
                 0  运行正确
                -1  运行有误
    注意事项：调用该函数时，可以不用关心执行结果
    编写人员：王凤龙
    编写时间：2017-05-20
*/
int radio::liftRealList_update(){
    /*获取电梯实时信息*/
    real_cont::getStr_real_All(radio::json_liftListReal_str);
    if(radio::json_liftListReal_str.size() <= 0x00) return 0x00;

    /*转换成json数据*/
    radio::json_liftListReal.clear();
    if(!radio::JReader.parse(radio::json_liftListReal_str, radio::json_liftListReal)) return -0x01;

    /*更新信息*/
    for(unsigned int i=0x00; i<radio::json_liftListReal.size(); i++){
        radio::liftReal_update(radio::json_liftListReal[i]);
    }

    return 0x00;
}

/*
    函数名称：playCmd_creat
    函数功能：创建播放指令
    传入参数：无
    传出数据：
                 0 创建成功
                -1 参数校验有误
    注意事项：创建指定设备的报警信息
    编写人员：王凤龙
    编写时间：2017-05-20
*/
int radio::playCmd_creat(radioInfo *item){
    /*参数校验*/
    if(!item) return -0x01;

    /*生成语音报警指令*/
    item->cmd_alarmRadio = "";
    if(*((unsigned char *)&item->status) == 0x00) return 0x00;

    radio::cmd_play.str("");
    radio::cmd_play<< "mplayer " << TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_START << " ";               //提示音
    
    if(item->rad_pgname.size() > 0x00){                                                                //祖先组          
        radio::cmd_play<< TTS_GROUNDNAME_DIR_PATH << "/" << item->rad_pgname << " ";
    }

    if(item->rad_gname.size() > 0x00){                                                                  //父组
        radio::cmd_play<< TTS_GROUNDNAME_DIR_PATH << "/" << item->rad_gname << " ";
    }

    radio::cmd_play<< TTS_LIFTNAME_DIR_PATH << "/" << item->rad_name << " ";                                        //电梯名称
    if(item->status.Kunren)         radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_KUNREN << " ";         //困人
    if(item->status.Up_Alarm)       radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_CHONGDING << " ";      //冲顶
    if(item->status.Down_Alarm)     radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_DUNDI << " ";          //蹲底
    if(item->status.Speeding)       radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_CHAOSU << " ";         //超速
    if(item->status.Run_door)       radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_RUNDOOR << " ";        //开门走梯
    if(item->status.Kaceng)         radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_KACENG << " ";         //卡层
    if(item->status.Manaual)        radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_MANUAL << " ";         //手动报警
    if(item->status.Power)          radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_DIAODIAN << " ";       //掉电
    if(item->status.Door_unopen)    radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_STP_DOORUNOPEN << " "; //停梯不开门
    if(item->status.Door_unclose)   radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_STP_DOORCLOSE << " ";  //停梯不关门
    if(item->status.Reapet)         radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_DOOR_REPEATE << " ";   //重复开关门
    if(item->status.Slant_fb)       radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_SLANT_FB << " ";       //电梯左右倾斜
    if(item->status.Slant_lr)       radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_SLANT_LR << " ";       //电梯前后倾斜
    if(item->status.Shake)          radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_SHAKE << " ";          //电梯震动
    if(item->status.Electromobile)  radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_ELECTROMOBILE << " ";  //电动车入梯
    if(item->status.Voltage)        radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_VOLTAGE << " ";        //电池电量低
    if(item->status.Power_down)     radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_POWER_DOWN << " ";     //外部掉电
    if(item->status.longtime)       radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_LONGTIME << " ";       //长期未维保
    
    radio::cmd_play<< TTS_ALARM_DIR_PATH << "/" << TTS_ALARM_END;

    /*语音指令写入对象*/
    item->cmd_alarmRadio = radio::cmd_play.str();
    return 0x00;
}

/*
    函数名称：liftAlarmList_update
    函数功能：报警播放指令列表更新
    传入参数：无
    传出数据：保留 0x00
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-22
*/
int  radio::liftAlarmList_update(){
    for(unsigned int point=0x00; point<radio::liftList.size(); point++){
        radio::playCmd_creat(*(radio::liftList.begin() + point));
    }

    return 0x00;
}

/*
    函数名称：run_collect
    函数功能：采集数据线程工作函数
    传入参数：arg    保留 NULL
    传出数据：保留 NULL
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-20
*/
void* radio::run_collect(void *arg){
    pthread_detach(pthread_self());
    arg = arg;

    for(;;){
        pthread_mutex_lock(&radio::dataLock);
        /*设备属性信息采集*/
        if(radio::liftInfoList_update()){ goto ONE_COLLECT_END; }

        /*设备实时信息采集*/
        if(radio::liftRealList_update()){ goto ONE_COLLECT_END; }

        /*设备报警指令更新*/
        if(radio::liftAlarmList_update()){ goto ONE_COLLECT_END; }

        ONE_COLLECT_END:
        pthread_mutex_unlock(&radio::dataLock);
        usleep(TTS_COLLECT_CYC);
    }

    return NULL;
}

/*
    函数名称：run_radio
    函数功能：音频播放线程函数
    传入参数：void *arg 保留 NULL
    传出数据：保留 NULL
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-20
*/
void* radio::run_radio(void *arg){
    pthread_detach(pthread_self());
    arg = arg;

    unsigned char status_dd = 0x00;     //删除，离线标志
    unsigned int point;
    unsigned int num;
    string alarmCmd = "";
    for(;;){
        /*获取设备列表数量*/
        pthread_mutex_lock(&radio::dataLock);
        num = radio::liftList.size();
        pthread_mutex_unlock(&radio::dataLock);

        for(point=0x00; point<num; point++){
            /*判断音频使能是否开启*/
            if(!radio::get_playStatus()){ break; }
            
            /*检测是否删除/离线*/
            pthread_mutex_lock(&radio::dataLock);
            status_dd = ((*(radio::liftList.begin() + point))->cmd.del || (*(radio::liftList.begin() + point))->cmd.disconnect) ? 1 : 0;
            pthread_mutex_unlock(&radio::dataLock);
            
            if(status_dd){ continue; }

            /*获取报警内容*/
            pthread_mutex_lock(&radio::dataLock);
            alarmCmd = (*(radio::liftList.begin() + point))->cmd_alarmRadio;
            pthread_mutex_unlock(&radio::dataLock);

            /*播放语音*/
            if(!radio::get_playStatus()){ break; }
            lsystem(alarmCmd.c_str());
            
        }

        /*删除多余设备*/
        pthread_mutex_lock(&radio::dataLock);
        radio::liftList_checkDel();
        radio::liftList_clear();
        pthread_mutex_unlock(&radio::dataLock);

        sleep(TTS_PLAY_CYC);
    }

    return NULL;
}

/*
    函数名称：start
    函数功能：音频工作
    传入参数：无
    传出数据：保留 0x00
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-22
*/
int radio::start(){
    pthread_t ptmp;

    /*开启采集线程*/
    pthread_create(&ptmp, NULL, radio::run_collect, NULL);

    /*开启音频线程*/
    pthread_create(&ptmp, NULL, radio::run_radio, NULL);

    return 0x00;
}