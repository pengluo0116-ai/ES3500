#include "redis_devupdate.h"
using namespace std;

/*
    函数名称：cmd_getDev_sqlCallback
    函数功能：获取所有设备回调函数
    传入参数：
                data 调用该函数的父函数传给该函数的值
                argc 一条查询记录中，列数
                argv 查询到结果以字符串列表的形式存到改变量中
                azColName 一条查询结果中，以字符串列表的形式，存储所有的列的名称
    传出数据：默认 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-18
*/
static int cmd_getDev_sqlCallback(void *data, int argc, char **argv, char **azColName){
    Json::Value *_jsonData = (Json::Value *)data;
    Json::Value jsonTmp;

    jsonTmp["id"] = string(argv[0]);
    jsonTmp["dev_name"] = string(argv[1]);
    jsonTmp["note"] = argv[2] ? string(argv[2]) : "";

    _jsonData->append(jsonTmp);

    return 0x00;
}

/*
    函数名称：cmd_getDev
    函数功能：获取所有设备信息
    传入参数：Json::Value &jsonData 获取到的设备信息存储到该引用指向的存储空间
    传出数据：
                 0  运行成功
                -1  打开数据库失败
                -2  数据库操作失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-12-08
*/
static int cmd_getDev(Json::Value &jsonData){

    /*搜索数据*/
    string sql = "SELECT id, dev_name, note FROM dev_info order by id desc";

    dbcontrl db((const char *)DBPATH);
    if(db.open()){ return -0x01; }

    if(db.exec(sql.c_str(), cmd_getDev_sqlCallback, (void *)&jsonData)){ return -0x02; }

    return 0x00;
}


/*
    函数名称：redis_devUpdate
    函数功能：更新设备信息到redis
    传入参数：Json::Value &jsonData
    传出数据：
                 0  运行成功
                 1  redis开启失败
                 2  向redis更新数据失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-12-08
*/
static int redis_devUpdate(Json::Value &jsonData){
    /*创建redis客户端*/
    redis_client _client(MQTT_SERVER_IP, MQTT_SERVER_PORT, NULL);
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
    函数名称：redis_devUpdate
    函数功能：更新设备信息到redis
    传入参数：无
    传出数据：
                 0  运行成功
                -1  打开数据库失败
                -2  数据库操作失败
                 1  redis开启失败
                 2  向redis更新数据失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-12-08
*/
int redis_devUpdate(){
    int err = 0x00;
    Json::Value jsonData;

    /*从数据库中获取设备信息*/
    if(err = cmd_getDev(jsonData)) return err;

    /*更新到redis*/
    return redis_devUpdate(jsonData);
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
    redis_client _client(MQTT_SERVER_IP, MQTT_SERVER_PORT, NULL);
    if(_client.createClient()) return 0x01;

    _client.delKey( (string("r_") + string(_devID)).c_str() );
    if(_client.delKey( (string("d_") + string(_devID)).c_str() )){ _client.closeClient(); return 0x02; }

    
    //_client.delKey( (string("a_") + string(_devID)).c_str() );

    /*关闭redis*/
    _client.closeClient();
    return 0x00;
}

/*
    函数名称：redis_devUpdateOne
    函数功能：修改一个设备的redis信息
    传入参数：Json::Value &jsonData
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-12-08
*/
int redis_devUpdateOne(Json::Value &jsonData){
    /*创建redis客户端*/
    redis_client _client(MQTT_SERVER_IP, MQTT_SERVER_PORT, NULL);
    if(_client.createClient()) return 0x01;

    string key_name = "d_" + jsonData["id"].asString();
    Json::FastWriter fast_writer;
    string jsonStr = fast_writer.write(jsonData);

    if(_client.setStr(key_name.c_str(), jsonStr.c_str())){ _client.closeClient(); return 0x02; }

    /*关闭redis*/
    _client.closeClient();
    return 0x00;
}