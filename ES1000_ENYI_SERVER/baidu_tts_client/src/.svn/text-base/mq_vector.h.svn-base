#ifndef __MQ_VECTOR_H__
#define __MQ_VECTOR_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <unistd.h>
#include "conf.h"

class mq_vector{
    private:
        static pthread_mutex_t lock;                            //锁
        static std::vector<std::string> msg_vector;             //容器
        
    public:
        static int init();                                      //初始化
        static int put_msg(const std::string &_msg);            //添加消息到容器
        static int get_msg(std::string &_msg);                  //从容器中获取一条消息
};

#endif