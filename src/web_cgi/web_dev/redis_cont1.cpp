#include "redis_cont.h"
using namespace std;
#define DEBUG 1

Json::Reader redis_cont::JReader;
unsigned char redis_cont::SBUFF[REDIS_CONT_KEYNAMEBUFF_SIZE] = { 0X00 };
unsigned char redis_cont::STRBUFF[REDIS_CONT_STRBUFF_SIZE] = { 0x00 };

/*
    函数名称：redis_cont
    函数功能：构造函数
    传入参数：
                const char *_sip    REDIS服务器ip
                int _port           REDIS服务器端口
                const char *_pasd   REDIS服务器登录密码
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-18
*/
redis_cont::redis_cont(const char *_sip, int _port, const char *_pasd): redis_client(_sip, _port, _pasd){}

/*
    函数名称：~redis_cont
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-18
*/
redis_cont::~redis_cont(){}

/*
    函数名称：init
    函数功能：初始化
    传入参数：无
    传出数据：
                 0  运行正确
                -1  REDIS配置数据有误--ip，密码，端口
                -2  连接服务器失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-18
*/
int redis_cont::init(){
    /*连接服务器*/
    if(int err = this->createClient()){ return err; }

    /*清空缓冲区*/
    memset((void *)this->STRBUFF, 0x00, REDIS_CONT_STRBUFF_SIZE);

    return 0x00;
}

/*
    函数名称：devKeyName_getAll_callback
    函数功能：从REDIS中获取所有设备属性的键名
    传入参数：
                void *_jsonList             获取到的数据添加进该指针指向的存储空间
                const char *_keyName        键值名称
                int _keyName_len            键值名称长度
    传出数据：
    注意事项：内部静态函数
    编写人员：王凤龙
    编写时间：2017-05-18
*/
int redis_cont::devKeyName_getAll_callback(void *_jsonList, const char *_keyName, int _keyName_len){
    /*参数校验*/
    if(!_jsonList) return -0x06;                                    //用户传入参数为空
    if(!_keyName || _keyName_len<=0x00) return 0x00;                //获取到的数据为空
    if(_keyName_len >= REDIS_CONT_KEYNAMEBUFF_SIZE) return -0x07;   //获取到的数据超限

    /*添加数据*/
    memset((void *)SBUFF, 0x00, REDIS_CONT_KEYNAMEBUFF_SIZE);
    memcpy((void *)SBUFF, (void *)_keyName, _keyName_len); 
    (*((Json::Value *)_jsonList)).append(string((const char *)SBUFF));

    return 0x00;
}

/*
    函数名称：realKeyName_getAll_callback
    函数功能：获取所有设备的实时信息键名
    传入参数：
                void *_jsonList             获取到的数据添加进该指针指向的存储空间
                const char *_keyName        键值名称
                int _keyName_len            键值名称长度
    传出数据：
    注意事项：内部静态函数
    编写人员：王凤龙
    编写时间：2017-05-18
*/
int redis_cont::realKeyName_getAll_callback(void *_jsonList, const char *_keyName, int _keyName_len){
    /*参数校验*/
    if(!_jsonList) return -0x06;                                    //用户传入参数为空
    if(!_keyName || _keyName_len<=0x00) return 0x00;                //获取到的数据为空
    if(_keyName_len >= REDIS_CONT_KEYNAMEBUFF_SIZE) return -0x07;   //获取到的数据超限

    /*添加数据*/
    memset((void *)SBUFF, 0x00, REDIS_CONT_KEYNAMEBUFF_SIZE);
    memcpy((void *)SBUFF, (void *)_keyName, _keyName_len); 
    (*((Json::Value *)_jsonList)).append(string((const char *)SBUFF));

    return 0x00;
}

/*
    函数名称：devKeyName_getAll
    函数功能：获取所有设备属性的的键名
    传入参数：无
    传出数据：
                -2  参数有误
                -3  客户端未连接
                -4  操作失败
                -5  未知错误
                <-5 回调函数的返回值错误
                >=0 获取到keys的个数
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-18
*/
int redis_cont::devKeyName_getAll(){
    this->jsonList_devKeyName.clear();
    return this->getKeys(REDIS_CONT_DEVINF_KEYS, devKeyName_getAll_callback, (void *)&this->jsonList_devKeyName);
}

/*
    函数名称：realKeyName_getAll
    函数功能：获取所有设备实时信息的键名
    传入参数：无
    传出数据：
                -2  参数有误
                -3  客户端未连接
                -4  操作失败
                -5  未知错误
                <-5 回调函数的返回值错误
                >=0 获取到keys的个数
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-18
*/
int redis_cont::realKeyName_getAll(){
    this->jsonList_realKeyName.clear();
    return this->getKeys(REDIS_CONT_REAL_KEYS, realKeyName_getAll_callback, (void *)&this->jsonList_realKeyName);
}

/*
    函数名称：devInfo_getAll
    函数功能：获取所有设备的属性信息
    传入参数：Json::Value &jsonList  获取到的设备信息存储到该引用指向的存储空间
    传出数据：
                -1  获取键名列表失败
                >=0 获取到属性信息的设备数量
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-18
*/
int redis_cont::devInfo_getAll(Json::Value &jsonList){
    /*获取所有设备的键名*/
    int len = 0x00;
    len = this->devKeyName_getAll();
    if(len < 0x00) return -0x01;
    if(len == 0x00) return 0x00;

    /*获取所有的设备的属性信息*/
    int err = 0x00;
    Json::Value jsonData;
    jsonList.clear();
    for(int i=0x00; i<len; i++){
        /*获取键值内容*/
        err = this->getStr(this->jsonList_devKeyName[i].asString().c_str(), (char *)STRBUFF, REDIS_CONT_STRBUFF_SIZE);
        if(err <= 0x00) continue;

        /*将获取到键值内容转换成json*/
        jsonData.clear();
        if(!redis_cont::JReader.parse(string((const char *)STRBUFF), jsonData)) continue;

        /*添加进json列表*/
        jsonList.append(jsonData);
    }

    return jsonList.size();
}

/*
    函数名称：devReal_getAll
    函数功能：获取所有设备的实时信息
    传入参数：Json::Value &jsonList  获取到的设备实时信息存储到该引用指向的存储空间
    传出数据：
                -1  获取键名列表失败
                >=0 获取到属性信息的设备数量
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-18
*/
int redis_cont::devReal_getAll(Json::Value &jsonList){
    /*获取所有实时信息的键名称*/
    int len = 0x00;
    len = this->realKeyName_getAll();
    if(len < 0x00) return -0x01;
    if(len == 0x00) return 0x00;

    /*获取所有设备的实时信息*/
    int err = 0x00;
    Json::Value jsonData;
    jsonList.clear();
    for(int i=0x00; i<len; i++){
        /*获取键值内容*/
        err = this->getStr(this->jsonList_realKeyName[i].asString().c_str(), (char *)STRBUFF, REDIS_CONT_STRBUFF_SIZE);
        if(err <= 0x00) continue;

        /*将获取到键值内容转换成json*/
        jsonData.clear();
        if(!redis_cont::JReader.parse(string((const char *)STRBUFF), jsonData)) continue;

        /*添加进json列表*/
        jsonList.append(jsonData);
    }

    return jsonList.size();
}