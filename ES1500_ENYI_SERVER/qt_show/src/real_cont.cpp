#include "real_cont.h"
#include "obj_web.h"
using namespace std;
#define DEBUG 1

Json::FastWriter real_cont::JWriter;                            //json字符串生成器--去格式化
obj_web *real_cont::Pobj_web = NULL;                            //展示栏指针
Json::Value real_cont::jsonList_devInfo;                        //所有设备属性信息列表
Json::Value real_cont::jsonList_real;                           //所有设备实时信息列表
std::string real_cont::strBak_devInfo = "";                     //所有设备属性信息字符串-备份
std::string real_cont::strNow_devInfo = "";                     //所有设备属性信息字符串-实时
std::string real_cont::strBak_real = "";                        //所有设备实时信息字符串-备份
std::string real_cont::strNow_real = "";                        //所有设备实时信息字符串-实时
redis_cont *real_cont::client = NULL;                           //REDIS客户端
int real_cont::client_connStatus = 0x00;                        //REDIS客户端在线状态
pthread_mutex_t real_cont::client_lock;                         //REDIS客户端锁
pthread_mutex_t real_cont::data_lock;                           //数据锁

/*
    函数名称：init
    函数功能：初始化
    传入参数：无
    传出数据：
    注意事项：该函数只能运行一次
    编写人员：王凤龙
    编写时间：2017-05-18
*/
int real_cont::init(){
    /*数据初始化*/
    real_cont::jsonList_devInfo.clear();
    real_cont::jsonList_real.clear();
    
    /*锁初始化*/
    if(pthread_mutex_init(&real_cont::client_lock, NULL)) return -0x01;
    if(pthread_mutex_init(&real_cont::data_lock, NULL)) return -0x02;

    /*创建并连接REDIS*/
    real_cont::client = new redis_cont(REDIS_CONT_SERVER_IP, REDIS_CONT_SERVER_PORT, REDIS_CONT_SERVER_PASD);
    if(!client) return -0x03;

    if(real_cont::client->init()){ delete real_cont::client; return -0x04; }

    real_cont::client_connStatus = 0x01;

    return 0x00;
}

/*
    函数名称：getStr_devInfo_All
    函数功能：获取设备属性信息字符串
    传入参数：std::string &_str  获取到的数据存储到该引用指向的存储空间
    传出数据：保留 0x00
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-18
*/
int real_cont::getStr_devInfo_All(std::string &_str){
    pthread_mutex_lock(&real_cont::data_lock);
    _str = real_cont::strNow_devInfo;
    pthread_mutex_unlock(&real_cont::data_lock);

    return 0x00;
}

/*
    函数名称：getStr_real_All
    函数功能：获取设备实时信息字符串
    传入参数：std::string &_str  获取到的数据存储到该引用指向的存储空间
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-18
*/
int real_cont::getStr_real_All(std::string &_str){
    pthread_mutex_lock(&real_cont::data_lock);
    _str = real_cont::strNow_real;
    pthread_mutex_unlock(&real_cont::data_lock);

    return 0x00;
}

/*
    函数名称：isconnected
    函数功能：获取REDIS客户端是否离线
    传入参数：无
    传出数据：0离线 1在线
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-18
*/
int real_cont::isconnected(){
    int status = 0x00;
    
    pthread_mutex_lock(&real_cont::client_lock);
    status = real_cont::client_connStatus;
    pthread_mutex_unlock(&real_cont::client_lock);

    return status;
}

/*
    函数名称：set_objTitle
    函数功能：设置展示栏指针
    传入参数：obj_web *_Pobj_web 展示栏控件指针
    传出数据：
    注意事项：所有线程开启前，需确保该函数已经正确执行
    编写人员：王凤龙
    编写时间：2017-05-18
*/
#if 1
int real_cont::set_objTitle(obj_web *_Pobj_web){
    /*参数校验*/
    if(!_Pobj_web) return -0x01;

    /*设置标题栏*/
    real_cont::Pobj_web = _Pobj_web;

    return 0x00;
}
#endif
/*
    函数名称：task_keepConnect
    函数功能：线程--保持与REDIS服务器间的连接
    传入参数：void *arg  保留 NULL
    传出数据：保留 NULL
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-18
*/
void *real_cont::task_keepConnect(void *arg){
    pthread_detach(pthread_self());
    arg = arg;

    int status = 0x00;
    for(;;){
        /*检测连接是否断开*/
        pthread_mutex_lock(&real_cont::client_lock);
        status = real_cont::client->isDisConnected();
        real_cont::client_connStatus = status ? 0 : 1;
        pthread_mutex_unlock(&real_cont::client_lock);
        
        /*连接状态保持*/
        if(!status){ sleep(REDIS_CONT_CONCHECK_CYC); continue; }

        /*连接状态断开*/
         pthread_mutex_lock(&real_cont::client_lock);
         if(real_cont::client){ delete real_cont::client; real_cont::client = NULL; }
         real_cont::client = new redis_cont(REDIS_CONT_SERVER_IP, REDIS_CONT_SERVER_PORT, REDIS_CONT_SERVER_PASD);
         if(real_cont::client){ if(!real_cont::client->init()){ real_cont::client_connStatus = 0x01; } }
         pthread_mutex_unlock(&real_cont::client_lock);
         
         sleep(REDIS_CONT_CONCHECK_CYC);
    }

    return NULL;
}

/*
    函数名称：task_keepGetData
    函数功能：获取实时数据
    传入参数：void *arg  保留 NULL
    传出数据：保留 NULL
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-18
*/
void *real_cont::task_keepGetData(void *arg){
    pthread_detach(pthread_self());
    arg = arg;

    for(;;){
        /*判断REDIS客户端是否处于连接状态*/
        if(!real_cont::isconnected()){ usleep(REDIS_CONT_COLLECT_CYC); }

        /*获取实时数据*/
        pthread_mutex_lock(&real_cont::client_lock);
        if(real_cont::client->devInfo_getAll(real_cont::jsonList_devInfo) > 0x00){                      //设备属性信息
            pthread_mutex_lock(&real_cont::data_lock);
            real_cont::strNow_devInfo = real_cont::JWriter.write(real_cont::jsonList_devInfo);          //生成字符串
            pthread_mutex_unlock(&real_cont::data_lock);
        }

        if(real_cont::client->devReal_getAll(real_cont::jsonList_real) > 0x00){                         //设备实时信息
            pthread_mutex_lock(&real_cont::data_lock);
            real_cont::strNow_real = real_cont::JWriter.write(real_cont::jsonList_real);                //生成字符串
            pthread_mutex_unlock(&real_cont::data_lock);
        } 
        pthread_mutex_unlock(&real_cont::client_lock);

        usleep(REDIS_CONT_COLLECT_CYC);              
    }

    return NULL;
}

/*
    函数名称：task_objTitle
    函数功能：线程--标题栏实时控制
    传入参数：void *arg  保留 NULL
    传出数据：保留 NULL
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-18
*/
#if 1
void *real_cont::task_objTitle(void *arg){
    pthread_detach(pthread_self());
    arg = arg;
    unsigned int point = 0x00;
    unsigned short dev_all=0x00, dev_online=0, dev_alarm=0;         //设备总数，在线数量，故障数量
    unsigned long dt_now=0x00, dt_tmp=0;                            //当前时间，临时时间（秒数）

    for(;;){
        /*查看设备总数*/
        dev_all = 0x00;
        pthread_mutex_lock(&real_cont::client_lock);
        dev_all = real_cont::jsonList_devInfo.size();
        pthread_mutex_unlock(&real_cont::client_lock);
 
        /*查看在线数量/故障数量*/
        dev_online = 0x00;
        dev_alarm = 0x00;
        pthread_mutex_lock(&real_cont::client_lock);
        for(point=0x00; point<real_cont::jsonList_real.size(); point++){
            /*获取当前时间*/
            cpp_datetimer::now_sec(&dt_now);

            /*在线数量*/
            if(cpp_datetimer::stringToDatetime_sec(real_cont::jsonList_real[point]["Parameters"]["Recv_time"].asString().c_str(), &dt_tmp)) continue;
            if((long)(dt_now-dt_tmp) < DEV_ONLINE_TIMEOUT){ dev_online++; } else{ continue; }
            
            /*故障检测*/
            if(real_cont::jsonList_real[point]["Alarm"]["Y_N"].asString() == "1") dev_alarm++;
        }
        pthread_mutex_unlock(&real_cont::client_lock);

        /*展示栏设置*/
        if(real_cont::Pobj_web){
           real_cont::Pobj_web->status(dev_all, dev_online, dev_alarm);
        }
        else{ cout<< "展示栏指针为空" <<endl; };

        sleep(RESIS_CONT_TITLECT_CYC);
    }

    return NULL;
}
#endif

/*
    函数名称：start
    函数功能：线程启动函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-22
*/
void real_cont::start(){
    pthread_t ptmp;

    /*连接保持*/
    pthread_create(&ptmp, NULL, real_cont::task_keepConnect, NULL);

    /*获取实时数据*/
    pthread_create(&ptmp, NULL, real_cont::task_keepGetData, NULL);

    /*标题栏实时控制*/
    pthread_create(&ptmp, NULL, real_cont::task_objTitle, NULL);

}