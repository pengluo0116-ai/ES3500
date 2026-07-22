#ifndef __MQ_COLLECTER_H__
#define __MQ_COLLECTER_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>

#include "conf.h"
#include "../include/MQTTClient.h"
#include "mq_vector.h"

class mq_collecter{
    private:
        std::string addr;                           //MQ服务器地址
        std::string clientID;                       //客户端ID
        std::string topic;                          //主题主题
        
        MQTTClient client;                          //MQTT客户端
        MQTTClient_connectOptions conn_opts;        //MQTT连接


    public:
        mq_collecter(const char *_addr, const char *_clientID, const char *_topic);
        ~mq_collecter();

    private:
        int _init();                                                                        //初始化函数
        int _recvCont();                                                                    //接收处理
        int _close();                                                                       //关闭连接并且销毁客户端
    public:
        int RUN();
};

#endif