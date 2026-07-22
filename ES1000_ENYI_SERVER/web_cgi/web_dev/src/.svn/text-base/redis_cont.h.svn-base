#ifndef __REDIS_CONT_H__
#define __REDIS_CONT_H__

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

#include "redis_client.h"
#include "../include/json/json.h"

class redis_cont{
    private:
        redis_client *client;

    public:
        redis_cont();
        ~redis_cont();

    public:
        int init();                                                         //初始化
        int set_device(Json::Value &jsonData);                              //设置设备信息
};

#endif