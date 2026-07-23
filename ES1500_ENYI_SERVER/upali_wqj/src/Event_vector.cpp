#include "Event_vector.h"
using namespace std;
#define DEBUG 1

std::vector<string> Event_vector::msg_vector;
pthread_mutex_t Event_vector::lock;

/*
    函数名称：init
    函数功能：初始化
    传入参数：无
    传出数据：
                 0  运行成功
                -1  初始化锁失败
                -2  清空容器失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-22
*/
int Event_vector::init(){
    if(pthread_mutex_init(&Event_vector::lock, NULL)) return -0x01;
    if(Event_vector::clear()) return -0x02;

    return 0x00;
}

/*
    函数名称：put_msg
    函数功能：添加消息到容器
    传入参数：Json::Value &_jsonMsg  要添加的数据
    传出数据：0  运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-22
*/
int Event_vector::put_msg(string _jsonMsg){
    int err = 0x00;

    pthread_mutex_lock(&Event_vector::lock);
    try{
        /*查看容器是否超限*/
        if(Event_vector::msg_vector.size() >= Event_vector::VCT_MAXNUM){ Event_vector::msg_vector.erase(Event_vector::msg_vector.begin()); }

        /*添加数据*/
        Event_vector::msg_vector.push_back(_jsonMsg);
    }
    catch(...){ err = -0x01; }
    pthread_mutex_unlock(&Event_vector::lock);

    return err;
}

/*
    函数名称：get_msg
    函数功能：从容器获取一条数据
    传入参数：Json::Value &_jsonMsg 获取到的数据存储到该引用指向的存储空间
    传出数据：
                <0 运行有误
                =0 信息为空
                =1 获取消息
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-22
*/
int Event_vector::get_msg(string _jsonMsg){
    int err = 0x00;
    vector<string>::iterator it = (vector<string>::iterator)0x00;
    
    pthread_mutex_lock(&Event_vector::lock);
    try{
        /*查看消息是否为空*/
        if(Event_vector::msg_vector.size() <= 0x00){ pthread_mutex_unlock(&Event_vector::lock); return 0x00; }

        /*获取消息*/
        it = Event_vector::msg_vector.begin();
        _jsonMsg = *it;

        /*从容器删除消息*/
        Event_vector::msg_vector.erase(it);

        err = 0x01;
    }
    catch(...){ err = -0x01; }
    pthread_mutex_unlock(&Event_vector::lock);

    return err;
}

/*
    函数名称：clear
    函数功能：清空容器
    传入参数：无
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-22
*/
int Event_vector::clear(){
    pthread_mutex_lock(&Event_vector::lock);
    Event_vector::msg_vector.clear();
    pthread_mutex_unlock(&Event_vector::lock);

    return 0x00;
}