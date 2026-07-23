#include "mq_vector.h"
using namespace std;
#define DEBUG 1

std::vector<std::string> mq_vector::msg_vector;
pthread_mutex_t mq_vector::lock;

/*
    函数名称：init
    函数功能：初始化
    传入参数：无
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-22
*/
int mq_vector::init(){
    return pthread_mutex_init(&mq_vector::lock, NULL);
}

/*
    函数名称：put_msg
    函数功能：添加消息到容器
    传入参数：const std::string &_msg 要添加的消息内容 
    传出数据：0 运行成功 非0 运行失败
    注意事项：如果消息满后，会删除容器保存最早的一条消息
    编写人员：王凤龙
    编写时间：2016-11-22
*/
int mq_vector::put_msg(const std::string &_msg){
    int err = 0x00;

    pthread_mutex_lock(&mq_vector::lock);
    try{

        /*查看容器使用是否超限*/
        if(mq_vector::msg_vector.size() >= MQTTCLIENT_VECTOR_MAXNUM){ mq_vector::msg_vector.erase(mq_vector::msg_vector.begin());}

        /*添加消息*/
        mq_vector::msg_vector.push_back(_msg);

    }catch(...){ err = -0x01; }
    pthread_mutex_unlock(&mq_vector::lock);

    return err;
}

/*
    函数名称：get_msg
    函数功能：获取一条消息
    传入参数：std::string &_msg
    传出数据：小于0 运行有误 =0 容器内消息为空 =1 获取消息
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-22
*/
int mq_vector::get_msg(std::string &_msg){
    int err = 0x00;
    vector<string>::iterator it = (vector<string>::iterator)0x00;

    pthread_mutex_lock(&mq_vector::lock);
    try{

        /*查看消息是否为空*/
        if(mq_vector::msg_vector.size() <= 0x00){ pthread_mutex_unlock(&mq_vector::lock); return 0x00; }

        /*获取消息*/
        it = mq_vector::msg_vector.begin();

        /*赋值消息*/
        _msg = *it;

        /*从容器中删除消息*/
        mq_vector::msg_vector.erase(it);

        err = 0x01;

    }catch(...){ err = -0x01; }
    pthread_mutex_unlock(&mq_vector::lock);

    return err;
}