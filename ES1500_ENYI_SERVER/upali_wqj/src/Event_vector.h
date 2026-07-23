#ifndef __EVENT_VECTOR_H__
#define __EVENT_VECTOR_H__

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <unistd.h>
#include <string.h>
#include "../include/json/json.h"
using namespace std;

class Event_vector{
    private:
        enum{ VCT_MAXNUM = 256 };

    private:
        static pthread_mutex_t lock;                    //数据锁
        static std::vector<string> msg_vector;     //数据容器

    public:
        static int init();                                              //初始化
        static int put_msg(string _jsonMsg);                      //添加消息到容器
        static int get_msg(string _jsonMsg);                      //从容器获取一条数据
        static int clear();                                             //清空容器
};

#endif