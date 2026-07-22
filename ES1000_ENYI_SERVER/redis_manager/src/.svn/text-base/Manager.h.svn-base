#ifndef __MANAGER_H__
#define __MANAGER_H__

/*
    文档介绍：
    1，REDIS缓存自检，检测未添加设备信息，添加至缓存，并删除多余的设备缓存信息
    2，检测数据库中多余的组信息和设备信息，并进行删除--暂定
*/

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <unistd.h>

#include "conf.h"
#include "redis_cont.h"
#include "redis_devupdate.h"
#include "db_cont.h"
#include "../include/json/json.h"

class Manager{
    private:
        Json::Value jsonGroupList_db;           //数据库中组信息
        Json::Value jsonLiftList_db;            //数据库中设备信息
        Json::Value jsonListList_redis;         //REDIS中设备信息

        Json::FastWriter JWriter;               //JSON去格式化字符串生成器
        Json::Reader JReader;                   //JSON字符串解析器

        std::string liftInfo_bak;               //数据库设备信息-备份
        std::string liftInfo_new;               //数据库设备信息-实时

    public:
        Manager();
        ~Manager();

    private:
        int _get_liftList_f_db();                                           //从数据库获取设备属性列表
        int _get_liftList_f_redis();                                        //从REDIS中获取设备属性列表
        int _get_lift_f_dblist_byID(const char *ID, Json::Value &jsonInfo); //根据设备ID，从数据库Json列表中获取设备信息
        int _get_lift_f_rdlist_byID(const char *ID, Json::Value &jsonInfo); //根据设备ID，从REDIS缓存Json列表中获取设备信息
        int _find_lift_f_dblist_byID(const char *ID);                       //根据设备ID，查看数据Json列表中是否存该设备
        int _find_lift_f_rdlist_byID(const char *ID);                       //根据设备ID，查看AEDIS缓存Json列表中是否存在该设备
        int _get_loseLiftList(Json::Value &jsonList);                       //获取REDIS多余设备的信息
        int _get_newLiftList(Json::Value &jsonList);                        //获取REDIS未添加设备的信息
        int _del_loseListList(Json::Value &jsonList);                       //从REDIS中删除多余的设备属性信息
        int _add_newLiftList(Json::Value &jsonList);                        //向REDIS中添加信息的设备属性信息
        int _check_liftInfo_db_rd(Json::Value &jsonInfo_db, Json::Value &jsonInfo_rd);
        int _getInfo();                                                     //获取数据库和REDIS中的设备属性信息
        int _update_liftInfo();                                             //将修改过属性的设备信息写入REDIS缓存
        int _update();                                                      //同步数据库和REDIS中的设备属性信息

    public:
        int init();
        int run();
};

#endif