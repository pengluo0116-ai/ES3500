#ifndef __UPDATE_H__
#define __UPDATE_H__

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <pthread.h>
#include <unistd.h>

#include "conf.h"
#include "lsystem.h"

class Update{
    public:
        enum{
            STATUS_START    = 0,        //开始升级
            STATUS_WORK     = 1,        //升级进行中
            STATUS_END      = 2,        //升级结束
            STATUS_FAILED   = 3         //升级失败
        };

    private:
        static unsigned char status;    //升级状态
        static pthread_mutex_t lock;    //锁

    private:
        static void set_status(unsigned char _status);                      //设置升级状态
        static int work();                                                  //升级

    public:
        static int init();                                                  //初始化
        static void *RUN(void *arg);                                               //系统升级
        static unsigned char get_status();                                  //获取升级状态
};

#endif