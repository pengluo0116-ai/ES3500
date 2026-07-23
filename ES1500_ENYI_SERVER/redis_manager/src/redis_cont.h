#ifndef __REDIS_CONT_H__
#define __REDIS_CONT_H__

/*
    文档简介
    针对本软件，redis的相关操作
    该类属于模块类，只能实例化为一个对象
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <pthread.h>
#include <unistd.h>

#include "conf.h"
#include "redis_client.h"
#include "../include/json/json.h"

class redis_cont: public redis_client{
    private:
        Json::Value jsonList_devKeyName;                                            //REDIS中，所有设备属性键名列表
        Json::Value jsonList_realKeyName;                                           //REDIS中，所有实时信息的键名列表

    public:
        redis_cont(const char *_sip, int _port, const char *_pasd);
        ~redis_cont();
    
    private:
        int devKeyName_getAll();                                                                            //获取所有设备属性的的键名
        int realKeyName_getAll();                                                                           //获取所有设备实时信息的键名
    
    public:
        int init();                                                                                         //初始化
        int devInfo_getAll(Json::Value &jsonList);                                                          //获取所有设备的属性信息
        int devReal_getAll(Json::Value &jsonList);                                                          //获取所有设备的实时信息

    private:
        static Json::Reader JReader;                                                                        //josn字符串解析器
        static unsigned char SBUFF[REDIS_CONT_KEYNAMEBUFF_SIZE];                                            //键值名称缓冲区
        static unsigned char STRBUFF[REDIS_CONT_STRBUFF_SIZE];                                              //键值内容缓存区
        
    private:
        static int devKeyName_getAll_callback(void *_jsonList, const char *_keyName, int _keyName_len);     //回调函数--获取所有设备属性的键名
        static int realKeyName_getAll_callback(void *_jsonList, const char *_keyName, int _keyName_len);    //回调函数--获取所有设备的实时信息的键名
        static int devInfo_getAll_callback(void *_jsonList, const char *_value, int _value_len);            //回调函数--获取所有设备的属性信息
        static int devReal_getAll_callback(void *_jsinList, const char *_value, int _value_len);            //回调函数--获取所有设备的实时信息
};

#endif