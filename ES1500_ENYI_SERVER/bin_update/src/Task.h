#ifndef __TASK_H__
#define __TASK_H__

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

#include "Update.h"
#include "lsystem.h"

class Task{
    private:
        static std::string CMD_RADIO_UPDATE_START;      //音频-升级开始
        static std::string CMD_RADIO_UPDATE_END;        //音频-升级结束
        static std::string CMD_RADIO_UPDATE_WORK;       //音频-升级进行
        static std::string CMD_RADIO_UPDATE_FAILED;     //音频-升级失败
        static std::string CMD_RADIO_UPDATE_RESTART;    //音频-升级失败，重新升级

    private:
        static int _update_one();
    
    public:
        static void update();
};

#endif