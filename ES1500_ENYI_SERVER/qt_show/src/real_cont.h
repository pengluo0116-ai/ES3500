#ifndef __REAL_CONT_H__
#define __REAL_CONT_H__

/*
    文档简介：
    实时数据获取及处理，本系统所有的实时数据获取，均从此处获取
    数据获取来源：REDIS
*/

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "redis_cont.h"

class obj_web;
class real_cont{
    private:
        //static obj_title *Pobj_title;                                     //标题栏指针
        static obj_web *Pobj_web;                                           //用于获取设备总数、在线总数、报警总数
        static Json::FastWriter JWriter;                                    //json字符串生成器
        static Json::Value jsonList_devInfo;                                //所有设备属性信息列表
        static Json::Value jsonList_real;                                   //所有设备实时信息列表
        static std::string strBak_devInfo;                                  //所有设备属性信息字符串-备份
        static std::string strNow_devInfo;                                  //所有设备属性信息字符串-实时
        static std::string strBak_real;                                     //所有设备实时信息字符串-备份
        static std::string strNow_real;                                     //所有设备实时信息字符串-实时

        static redis_cont *client;                                          //REDIS客户端
        static int client_connStatus;                                       //REDIS客户端在线状态

        static pthread_mutex_t client_lock;                                 //REDIS客户端锁
        static pthread_mutex_t data_lock;                                   //数据锁

    public:
        static int init();                                                                              //初始化
        static int getStr_devInfo_All(std::string &_str);                                               //获取设备属性信息字符串
        static int getStr_real_All(std::string &_str);                                                  //获取设备实时信息字符串
        static int isconnected();                                                                      //获取REDIS客户端是否在线状态
        static void start();                                                                            //线程启动函数
        static int set_objTitle(obj_web *_Pobj_web);                                                //设置展示界面

    private:
        static void * task_keepConnect(void *arg);                                                      //线程--连接保持
        static void * task_keepGetData(void *arg);                                                      //线程--实时获取数据
        static void * task_objTitle(void *arg);                                                         //线程--标题栏实时控制            
};

#endif