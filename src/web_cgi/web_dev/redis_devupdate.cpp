#include "redis_devupdate.h"
using namespace std;

/*
    函数名称：group_getNameByID
    函数功能：获取组名称
    传入参数：   int id                         组ID
                Json::Value &jsonList_group    组数据库列表信息
    传出数据：组名称
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-26
*/
static string group_getNameByID(int id, Json::Value &jsonList_group){
    for(int point=0x00; point<jsonList_group.size(); point++){
        if(id == jsonList_group[point]["id"].asInt()) return jsonList_group[point]["name"].asString();
    }
    return "";
}

/*
    函数名称：group_getJsonByID
    函数功能：根据组ID获取指定组的json
    传入参数：
                int id                          组ID
                Json::Value &jsonList_group     组数据库列表信息
                Json::Value &jsonData           获取到的数据存储到该指针指向的存储空间
    传出数据：
                0   未获取到
                1   获取成功
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-26
*/
static int group_getJsonByID(int id, Json::Value &jsonList_group, Json::Value &jsonData){
    for(int point=0x00; point<jsonList_group.size(); point++){
        if(id != jsonList_group[point]["id"].asInt()) continue;
        jsonData = jsonList_group[point]; return 0x01;
    }

    return 0x00;
}

/*
    函数名称：dev_create_info
    函数功能：创建完整的设备属性信息--用于redis中保存
    传入参数：
                Json::Value &jsonList_dev   设备数据库列表信息
                Json::Value &jsonList_group 组数据库列表信息
    传出数据：保留 0
    注意事项：jonsList_dev中的数据会添加部分键值
    编写人员：王凤龙
    编写时间：2017-05-26
*/
static int dev_create_info(Json::Value &jsonList_dev, Json::Value &jsonList_group){
    Json::Value jsonG;
    for(unsigned int point=0; point<jsonList_dev.size(); point++){
        if(group_getJsonByID(jsonList_dev[point]["gid"].asInt(), jsonList_group, jsonG)){
            jsonList_dev[point]["gname"] = jsonG["name"].asString();
            jsonList_dev[point]["pgid"] = jsonG["pid"].asInt();
            if(jsonG["pid"].asInt() == 0x00) jsonList_dev[point]["pgname"] = "";
            else jsonList_dev[point]["pgname"] = group_getNameByID(jsonG["pid"].asInt(), jsonList_group);
            jsonList_dev[point]["time"] = "0";
            jsonList_dev[point]["period"] = "0";
        }
        else{
            jsonList_dev[point]["gname"] = "";
            jsonList_dev[point]["pgid"] = 0;
            jsonList_dev[point]["pgname"] = "";
            jsonList_dev[point]["time"] = "0";
            jsonList_dev[point]["period"] = "0";
        }
    }
    return 0x00;
}

/*
    函数名称：redis_devUpdate
    函数功能：更新设备信息到redis
    传入参数：Json::Value &jsonData      数据列表
    传出数据：
                 0  运行成功
                 1  redis开启失败
                 2  向redis更新数据失败
    注意事项：该方式仅仅用于添加和修改
    编写人员：王凤龙
    编写时间：2016-12-08
*/
static int redis_devUpdate(Json::Value &jsonData){
    /*创建redis客户端*/
    redis_client _client(REDIS_SERVER_ADDR, REDIS_SERVER_PORT, REDIS_SERVER_PASD);
    if(_client.createClient()) return 0x01;

    /*将数据更新到redis*/
    Json::FastWriter fast_writer;
    string jsonStr = "";
    string key_name = "";

    int json_listSize = jsonData.size();
    for(int i=0x00; i<json_listSize; i++){
         jsonStr = fast_writer.write(jsonData[i]);  
         key_name = string("d_") + jsonData[i]["id"].asString();
         if(_client.setStr(key_name.c_str(), jsonStr.c_str())){ _client.closeClient(); return 0x02; }
    }

    /*关闭redis*/
    _client.closeClient();
    return 0x00;
}

/*
    函数名称：cmd_device_redisUpdate
    函数功能：将设备信息更新到REDIS
    传入参数：
                Json::Value &jsonList_dev   设备数据库列表信息
                Json::Value &jsonList_group 组数据库列表信息
    传出数据：
                 0  运行成功
                 1  redis开启失败
                 2  向redis更新数据失败
    注意事项：jonsList_dev中的数据会添加部分键值
    编写人员：王凤龙
    编写时间：2017-05-26
*/
int cmd_device_redisUpdate(Json::Value &jsonList_dev, Json::Value &jsonList_group){
    /*生成完整的设备属性*/
    dev_create_info(jsonList_dev, jsonList_group);

    /*写入redis*/
    return redis_devUpdate(jsonList_dev);
}

/*
    函数名称：redis_devDel
    函数功能：在设备中删除redis
    传入参数：const char *devID
    传出数据：
                 0  运行成功
                 1  打开redis失败
                 2  操作redis失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-12-08
*/
int redis_devDel(const char *_devID){
    /*参数校验*/
    if(!_devID || strlen(_devID)<=0x00) return -0x01;

    /*创建redis客户端*/
    redis_client _client(REDIS_SERVER_ADDR, REDIS_SERVER_PORT, REDIS_SERVER_PASD);
    if(_client.createClient()) return 0x01;

    _client.delKey( (string("r_") + string(_devID)).c_str() );
    if(_client.delKey( (string("d_") + string(_devID)).c_str() )){ _client.closeClient(); return 0x02; }

    
    //_client.delKey( (string("a_") + string(_devID)).c_str() );

    /*关闭redis*/
    _client.closeClient();
    return 0x00;
}

/*
    函数名称：redis_devListDel
    函数功能：批量删除设备
    传入参数：Json::Value &jsonList [{"id":"设备ID"}]
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-02
*/
int redis_devListDel(Json::Value &jsonList){
    redis_client _client(REDIS_SERVER_ADDR, REDIS_SERVER_PORT, REDIS_SERVER_PASD);
    if(_client.createClient()) return 0x01;

    try{
        int num = jsonList.size(); if(num==0x00) return 0x00;
        for(int point=0x00; point<num; point++){
            _client.delKey( (string("r_") + jsonList[point]["id"].asString()).c_str() );
            _client.delKey( (string("d_") + jsonList[point]["id"].asString()).c_str() );
        }
    }catch(...){}

    _client.closeClient();
    return 0x00;
}