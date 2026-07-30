#include "contrl.h"
using namespace std;

/*
    所用模板：VOIDAR_WEB_MD_1
    编写人员：王凤龙
    备注信息：
                无需关心底层是如何传递过来数据，以及如何提取和解析数据！
                终端传入参数模型(JSON)：{"display":1, ...}
                除了"相关操作函数添加处"和"分支操作"处外，其它地方不可做任何修改，包括注释
*/

#define GET_DATA_BUFF_LEN 2048              //获取终端传输数据缓冲区的大小
static char msg[GET_DATA_BUFF_LEN] = {0x00};

/*========================================================================================*/
/*相关操作函数添加处*/

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

    _jsonData->append(jsonTmp);

    return 0x00;
}

/*
    函数名称：cmd_getDev
    函数功能：获取所有设备信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-18
*/
static void cmd_getDev(){
    Json::Value jsonData;

    /*搜索数据*/
    string sql = "SELECT id, dev_name FROM dev_info";

    dbcontrl db((const char *)DBPATH);
    if(db.open()){ err_msg("404", "开启数据库失败,请稍后再试"); return; }

    int res = 0x00;
    if(db.exec(sql.c_str(), cmd_getDev_sqlCallback, (void *)&jsonData)){ err_msg("404", "获取设备数据失败，请稍后再试"); return; }

    /*返回数据*/
    if(jsonData.size() <= 0x00){ err_msg("101", "注册设备为空"); return; }

    Json::Value jsonPost;
    jsonPost["error"] = 0;
    jsonPost["data"] = jsonData;
    

    Json::FastWriter fast_writer;
    string jsonStr = fast_writer.write(jsonPost); 

    cout<<"Content-type:text/html\r\n\r\n";
    cout<<jsonStr<<endl;

    return;
}


/*
    函数名称：cmd_getDevReal
    函数功能：获取设备实时信息
    传入参数：Json::Value &_jsonData {"display":2, "dev":[11111,22222]}
    传出数据：无
    注意事项：如果不存在某个设备的键值，该设备的信息不返回
    编写人员：王凤龙
    编写时间：2016-10-18
*/
static void cmd_getDevReal(Json::Value &_jsonData){
    /*校验参数*/
    if(_jsonData["dev"].isNull()){ err_msg("上传数据有误"); return; }
    if(_jsonData["dev"].size() <= 0x00){ err_msg("上传设备数据为空"); return; }

    /*开启redis*/
    redis_client _redis(MQTT_SERVER_IP, MQTT_SERVER_PORT, NULL);
    if(_redis.createClient()){ err_msg("获取实时数据失败，请稍后再试"); return;}

    /*获取数据*/
    ostringstream osKey;
    char *getredisBuff = new char[2048];

    int devLen = 0x00;
    devLen = _jsonData["dev"].size();
    
    Json::Value jsonList;
    for(int i=0x00; i<devLen; i++){
        osKey.str("");
        int err = 0;
        /*设置key名称*/
        try{ osKey << "r_" << _jsonData["dev"][i].asString(); }catch(...){ _redis.closeClient(); delete getredisBuff; err_msg("上传数据有误"); return; }

        /*获取key值*/
        if( (err = _redis.getStr(osKey.str().c_str(), getredisBuff, 2048)) <= 0x00) continue;

        /*添加进列表*/
        jsonList.append(string(getredisBuff));
    }
    _redis.closeClient(); delete getredisBuff;

    /*返回数据*/
    Json::Value jsonPost;
    jsonPost["error"] = 0;
    jsonPost["data"] = jsonList;

    char serverDate[20] = {0x00};
    get_newDateTime((unsigned char *)serverDate, 20);
    jsonPost["serverDate"] = string(serverDate);

    Json::FastWriter fast_writer;
    string jsonStr = fast_writer.write(jsonPost); 

    cout<<"Content-type:text/html\r\n\r\n";
    cout<<jsonStr<<endl;

}
/*========================================================================================*/

/*
    函数名称：getPostData
    函数功能：获取终端传递过来的数据，并且进行解析，处理
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-06
*/
void getPostData(){
    #if 0
    /*用户登录信息cookie的校验*/
    ckuser_cookie _user_ck;
    if(_user_ck.ckUser()){ err_msg("399", ""); return;}
    #endif

    int len = 0x00;
    char *lenStr = (char *)0x00;
    memset(msg, 0, GET_DATA_BUFF_LEN);
    
    /*获取post上传数据长度*/
    lenStr =  getenv("CONTENT_LENGTH");
    if(lenStr == (char *)0x00){ err_msg("上传数据为空"); return;}
    len = atoi(lenStr);
    if(len > (GET_DATA_BUFF_LEN-1)) { err_msg("上传数据错误"); return;} //上传数据量超限，返回错误信息
    
    /*获取上传数据*/
    fgets(msg, len+1, stdin);
    
    Json::Reader reader;
    Json::Value value;
    int display = 0x00;
    try{
        if(!reader.parse(string(msg), value)) throw("");
        display = value["display"].asInt();
        
        switch(display){
            /*分支操作，添加此处*/
            /*==========================================*/
            case 1: cmd_getDev(); break;
            case 2: cmd_getDevReal(value); break;
            /*==========================================*/
            default: err_msg("上传数据格式错误"); break;
        }
    }catch(...){
        err_msg("上传数据格式错误");
    }
}


/*
    函数名称：err_msg
    函数功能：返回错误信息
    传入参数：_code->错误编码 _err_msg->错误信息
    输出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-16
*/
void err_msg(const char *_err_msg){
    string err_msg = "{\"error\":404, \"data\":\""+ string(_err_msg) +"\"}";
    cout<<"Content-type:text/html\r\n\r\n";
    cout<<err_msg<<endl;
}

void err_msg(const char *_code, const char *_err_msg){
    string err_msg = "{\"error\":"+ string(_code) +", \"data\":\""+ string(_err_msg) +"\"}";
    cout<<"Content-type:text/html\r\n\r\n";
    cout<<err_msg<<endl;
}