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
    函数名称：cmd_get_allAlarmData_sqlCallback
    函数功能：获取所有设备报警信息回调函数
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
static int cmd_get_allAlarmData_sqlCallback(void *data, int argc, char **argv, char **azColName){
    Json::Value *_jsonData = (Json::Value *)data;
    Json::Value jsonTmp;

    try{
        jsonTmp["dev_id"] = argv[0];
        jsonTmp["dev_name"] = string(argv[1]);
        jsonTmp["ds_dx"] = string(argv[2]);
        jsonTmp["lou_ceng"] = string(argv[3]);
        jsonTmp["status"] = string(argv[4]);
        jsonTmp["people"] = string(argv[5]);
        jsonTmp["kaceng"] = string(argv[6]);
        jsonTmp["kunren"] = string(argv[7]);
        jsonTmp["up_alarm"] = string(argv[8]);
        jsonTmp["run_door"] = string(argv[9]);
        jsonTmp["down_alarm"] = string(argv[10]);
        jsonTmp["speeding"] = string(argv[11]);
        jsonTmp["power"] = string(argv[12]);
        jsonTmp["time"] = string(argv[13]);
    }catch(...){ return 0x00; }
    

    _jsonData->append(jsonTmp);

    return 0x00;
}

/*
    函数名称：cmd_get_allAlarmData
    函数功能：获取某个年月的报警记录
    传入参数：Json::Value &_jsonData
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-21
*/
static void cmd_get_allAlarmData(Json::Value &_jsonData){
    /*参数校验*/
    if(_jsonData["alarm_data"].isNull() || !_jsonData["alarm_data"].isString() || _jsonData["alarm_data"].asString().size() <= 0x00){ err_msg("上传数据有误"); return; }

    /*判断文件是否存在*/
    if(!(access(_jsonData["alarm_data"].asString().c_str(), F_OK))){ err_msg("401", "该月份无报警信息"); return; }

    /*生成数据库文件*/
    string db_path = string(ALARM_DATAPATH) + _jsonData["alarm_data"].asString() + ".db";

    /*查询数据库*/
    Json::Value jsonData;
    string sql = "SELECT dev_id, dev_name, ds_dx, lou_ceng, status, people, kaceng, kunren, up_alarm, down_alarm, run_door, speeding, power, alarm_time FROM alarm_info ORDER BY id desc";

    dbcontrl db((const char *)db_path.c_str());
    if(db.open()){ err_msg("404", "开启数据库失败,请稍后再试"); return; }

    int res = 0x00;
    if(db.exec(sql.c_str(), cmd_get_allAlarmData_sqlCallback, (void *)&jsonData)){ err_msg("404", "获取设备数据失败，请稍后再试"); return; }

    /*返回数据*/
    if(jsonData.size() <= 0x00){ err_msg("401", "该月份无报警信息"); return; }

    Json::Value jsonPost;
    jsonPost["error"] = 0;
    jsonPost["data"] = jsonData;

    Json::FastWriter fast_writer;
    string jsonStr = fast_writer.write(jsonPost); 

    cout<<"Content-type:text/html\r\n\r\n";
    cout<<jsonStr<<endl;

    return;
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
            case 2: cmd_get_allAlarmData(value); break;         //添加设备
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