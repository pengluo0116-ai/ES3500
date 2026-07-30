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
    jsonTmp["note"] = argv[2] ? string(argv[2]) : "";
    jsonTmp["url"] = argv[3] ? string(argv[3]) : "";
    jsonTmp["x"] = atoi(argv[4]);
    jsonTmp["y"] = atoi(argv[5]);

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
    string sql = "SELECT id, dev_name, note, url, x, y FROM dev_info order by did desc";

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
    函数名称：check_devID_sqlCallback
    函数功能：获取指定ID设备的数量回调函数
    传入参数：
                data 调用该函数的父函数传给该函数的值
                argc 一条查询记录中，列数
                argv 查询到结果以字符串列表的形式存到改变量中
                azColName 一条查询结果中，以字符串列表的形式，存储所有的列的名称
    传出数据：默认 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-16
*/
static int check_devID_sqlCallback(void *data, int argc, char **argv, char **azColName){
    unsigned int *dev_NUM = (unsigned int *)data;

    (*dev_NUM) += 1;

    return 0x00;
}

/*
    函数名称：check_devID
    函数功能：根据设备ID，获取设备的数量
    传入参数：
                const char *_id         设备的ID
                unsigned int *_num      获取到的设备的数量，存储到该指针指向的存储空间
    传出数据：
                 0  运行成功
                -1  开启数据库失败
                -2  查询设备ID操作失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-16
*/
static int check_devID(const char *_id, unsigned int *_num){
    /*参数校验*/
    if(!_id || strlen(_id)<=0x00 || !_num) return -0x01;

    /*查询数据库*/
    *_num = 0x00;
    string sql = "SELECT id FROM dev_info WHERE id=\""+ string(_id) +"\"";

    dbcontrl db((const char *)DBPATH);
    if(db.open()) return -0x02;

    int res = 0x00;
    if(db.exec(sql.c_str(), check_devID_sqlCallback, (void *)_num)) return -0x03;

    return 0x00;
}

/*
    函数名称：cmd_addDev
    函数功能：添加设备
    传入参数：Json::Value &_jsonData
    传出数据：无
    注意实行：无
    编写人员：王凤龙
    编写时间：2016-10-19
*/
static void cmd_addDev(Json::Value &_jsonData){
    /*参数校验*/
    if(
        _jsonData["dev_name"].isNull() || _jsonData["url"].isNull() || _jsonData["id"].isNull() ||
        _jsonData["dev_name"].asString().size() <= 0x00 || _jsonData["id"].asString().size() <= 0x00
    ){ err_msg("上传数据有误"); return; }

    /*ID校验*/
    unsigned int dev_id_num = 0x00;
    switch(check_devID(_jsonData["id"].asString().c_str(), &dev_id_num)){
        case 0: break;
        case -1: err_msg("设备ID格式有误"); return;
        case -2: err_msg("开启数据库失败, 请稍后再试"); return;
        case -3: err_msg("设备ID校验失败，请稍后再试"); return;
        default: err_msg("后台繁忙，请稍后再试"); return;
    }
    if(dev_id_num){ err_msg("该设备ID已经注册到本主机，不可重复添加"); return; }

    /*写入数据库*/
    string sql = "INSERT INTO dev_info(id, dev_name, url) VALUES(\""+ _jsonData["id"].asString() +"\", \""+ _jsonData["dev_name"].asString() +"\", \""+ _jsonData["url"].asString() +"\")";

    dbcontrl db((const char *)DBPATH);
    if(db.open()){ err_msg("404", "开启数据库失败,请稍后再试"); return; }

    int res = 0x00;
    if(db.exec(sql.c_str(), NULL, NULL)){ err_msg("404", "获取设备数据失败，请稍后再试"); return; }

    /*更新redis*/
    redis_devUpdate();

    err_msg("0", ""); return;
}

/*
    函数名称：cmd_delDev
    函数功能：删除设备
    传入参数：Json::Value &_jsonData
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-19
*/
static void cmd_delDev(Json::Value &_jsonData){
    /*参数校验*/
    if(_jsonData["id"].isNull()){ err_msg("上传数据有误"); return; }

    /*从数据库中删除设备*/
    string sql = "DELETE from dev_info WHERE id = \"" + _jsonData["id"].asString() + "\"";

    dbcontrl db((const char *)DBPATH);
    if(db.open()){ err_msg("404", "开启数据库失败,请稍后再试"); return; }

    int res = 0x00;
    if(db.exec(sql.c_str(), NULL, NULL)){ err_msg("404", "获取设备数据失败，请稍后再试"); return; }

    redis_devDel(_jsonData["id"].asString().c_str());

    err_msg("0", ""); return;
}

/*
    函数名称：cmd_updateDev
    函数功能：修改设备信息
    传入参数：Json::Value &_jsonData {"id":"5000", "dev_name":"测试设备", "url":""}
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-19
*/
static void cmd_updataDev(Json::Value &_jsonData){
    /*参数校验*/
    if(
        _jsonData["id"].isNull() ||
        _jsonData["dev_name"].isNull() ||
        _jsonData["url"].isNull()
    ){ err_msg("上传数据有误"); return; }

    /*修改数据库*/
    string sql = "UPDATE dev_info SET id=\""+ _jsonData["id"].asString() +"\", dev_name = \""+ _jsonData["dev_name"].asString() +"\", url = \""+ _jsonData["url"].asString() +"\" WHERE id =\"" + _jsonData["id"].asString() +"\"";

    dbcontrl db((const char *)DBPATH);
    if(db.open()){ err_msg("404", "开启数据库失败,请稍后再试"); return; }

    int res = 0x00;
    if(db.exec(sql.c_str(), NULL, NULL)){ err_msg("404", "获取设备数据失败，请稍后再试"); return; }

    redis_devUpdateOne(_jsonData);
    err_msg("0", ""); return;
}

/*
    函数名称：cmd_updateDev_map
    函数功能：修改设备信息
    传入参数：Json::Value &_jsonData {"id":"5001", "dev_name":"测试设备", "url":"", "x":1, "y":1}
    传出数据：无
    注意事项：地图添加，包含地图的坐标信息
    编写人员：王凤龙
    编写时间：2017-05-03
*/
static void cmd_updateDev_map(Json::Value &_jsonData){
    /*参数校验*/
    if(
        _jsonData["id"].isNull() ||
        _jsonData["dev_name"].isNull() ||
        _jsonData["url"].isNull() ||
        _jsonData["x"].isNull() ||
        _jsonData["y"].isNull()
    ){ err_msg("上传数据有误"); return; }

    /*修改数据库*/
    ostringstream sql; sql.str("");
    sql<< "UPDATE dev_info SET dev_name=\""<< _jsonData["dev_name"].asString() <<"\", url=\""<< _jsonData["url"].asString() <<"\", x="<< _jsonData["x"].asInt() <<", y="<< _jsonData["y"].asInt() <<" WHERE id=\""<< _jsonData["id"].asString() <<"\";";
    
    dbcontrl db((const char *)DBPATH);
    if(db.open()){ err_msg("404", "开启数据库失败,请稍后再试"); return; }

    int res = 0x00;
    if(db.exec(sql.str().c_str(), NULL, NULL)){ err_msg("404", "获取设备数据失败，请稍后再试"); return; }

    redis_devUpdateOne(_jsonData);
    err_msg("0", ""); return;
}

/*
    函数名称：cmd_get_showMode
    函数功能：获取展示模式--地图，列表
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-04
*/
static void cmd_get_showMode(){
    Json::Value jsonData; jsonData.clear();
    if(JreadConf(LIFT_SHOW_CONF_PATH, jsonData)){ err_msg("读取设备模式配置文件失败，请稍后再试"); return; }

    if(jsonData["type"].isNull() || !jsonData["type"].isInt()){ err_msg("设备配置文件数据有误，请稍后再试"); return; }
    switch(jsonData["type"].asInt()){ 
        case 0: jsonData["page"] = LIFT_SHOW_MODE_MAP; break;   //地图模式
        case 1: jsonData["page"] = LIFT_SHOW_MODE_LIST; break;  //列表模式
        default: err_msg("设置显示模式无效，请稍后再试"); return;
    }

    jsonData["error"] = 0x00;
    Json::FastWriter fast_writer;
    string jsonStr = fast_writer.write(jsonData); 
    cout<<"Content-type:text/html\r\n\r\n";
    cout<<jsonStr<<endl;
}

/*
    函数名称：cmd_set_showMode
    函数功能：设置展示模式
    传入参数：Json::Value &jsonData  {"type":1}
    传出数据：无
    注意事项：传入参数中的type键值 0:地图 1:列表
    编写人员：王凤龙
    编写时间：2017-05-04
*/
static void cmd_set_showMode(Json::Value &jsonData){
    /*参数校验*/
    if(jsonData["type"].isNull() || !jsonData["type"].isInt()){ err_msg("上传数据有误"); return; }

    /*组装模板替换指令*/
    ostringstream ostr_cmd; ostr_cmd.str("");
    switch(jsonData["type"].asInt()){
        case 0: ostr_cmd<< "cp -rf " << LIFT_QTMODE_MAP << " " << LIFT_QTSHOW_FILE; break;
        case 1: ostr_cmd<< "cp -rf " << LIFT_QTMODE_LIST << " " << LIFT_QTSHOW_FILE; break;
        default: err_msg("上传数据有误，请稍候再试"); return;
    }

    /*写入配置文件*/
    if(JwriteConf(LIFT_SHOW_CONF_PATH, jsonData)){ err_msg("写入配置文件失败，请稍后再试"); return; }

    /*替换QT展示模板*/
    lsystem(ostr_cmd.str().c_str());

    /*杀死QT进程*/
    lsystem("if [ \"`pidof -x /root/qt_show/qt_real_show`\" != \"\" ]; then kill -9 `pidof -x /root/qt_show/qt_real_show`; fi");

    err_msg("0", "");
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
            case 1: cmd_getDev(); break;            //获取设备信息
            case 2: cmd_addDev(value); break;       //添加设备
            case 3: cmd_delDev(value); break;       //删除设备
            case 4: cmd_updataDev(value); break;    //修改设备信息
            case 5: cmd_updateDev_map(value); break;//修改设备信息-地图坐标
            case 6: cmd_get_showMode(); break;      //获取显示模式-列表，地图
            case 7: cmd_set_showMode(value); break; //设置显示模式-列表，地图
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