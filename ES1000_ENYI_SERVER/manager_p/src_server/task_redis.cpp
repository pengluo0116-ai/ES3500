#include "task_redis.h"
using namespace std;
#define DEBUG 1

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

    jsonTmp["id"] = argv[0];
    jsonTmp["gid"] = atoi(argv[1]);
    jsonTmp["dev_name"] = argv[2];
    jsonTmp["url"] = argv[3];
    jsonTmp["x"] = atoi(argv[4]);
    jsonTmp["y"] = atoi(argv[5]);
    jsonTmp["note"] = argv[6];
    jsonTmp["madein"] = argv[7];
    jsonTmp["property"] = argv[8];
    jsonTmp["mainter"] = argv[9];
    jsonTmp["mainter_tel"] = argv[10];
    jsonTmp["address"] = argv[11];

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
    string sql = "SELECT id, gid, dev_name, url, x, y, note, madein, property, mainter, mainter_tel, address FROM dev_info;";

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
    函数名称：task_redis
    函数功能：构造函数
    传入参数：
                const char *_task_name      进程的名称
                const char _run_path        进程路径
                const char *_conf_info      进程参数
                int _open_maxNum            最大开启次数
                int _open_spaceTime         开启间隔
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-15
*/
task_redis::task_redis(const char *_task_name, const char *_run_path, const char *_conf_info, int _open_maxNum = 0, int _open_spaceTime = 0):
task(_task_name, _run_path, _conf_info, _open_maxNum, _open_spaceTime){}

/*
    函数名称：~task_redis
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-15
*/
task_redis::~task_redis(){}

/*
    函数名称：run_status_open
    函数功能：进程运行成功后的处理
    传入参数：无
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-15
*/
int task_redis::run_status_open(){
    // int err = 0x00;
    // int tmp = 0x00;
    // Json::Value jsonData;

    // /*从数据库中读取设备信息*/
    // for(tmp=0x00; tmp<task_redis::SQL_READ_MAX_NUM; tmp++){
    //     jsonData.clear();
    //     if(!cmd_getDev(jsonData)) break;
    //     sleep(task_redis::SQL_READ_TIMEOUT);
    // }
    
    // /*将设备信息写入redis*/
    // for(tmp=0x00; tmp<task_redis::REDIS_CONT_MAX_NUM; tmp++){
    //     sleep(task_redis::REDIS_CONT_TIMEOUT);
    //     if(!(err = redis_devUpdate(jsonData)))break;
    // }

    // /*判断写入redis是否成功*/
    // if(err){ 
    //     ostringstream ostr_cmd; ostr_cmd.str("");
    //     ostr_cmd<< "kill -9 `pidof -x " << REDIS_PATH << "`";
    //     lsystem(ostr_cmd.str().c_str());
    // }

    return 0x00;
}