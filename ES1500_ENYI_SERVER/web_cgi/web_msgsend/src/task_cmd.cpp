#include "task_cmd.h"
using namespace std;
static int getUserList_sqlCallback(void *data, int argc, char **argv, char **azColName){
    Json::Value _jsonData_;
    _jsonData_["id"] = atoi(argv[0]);
    _jsonData_["name"] = string(argv[1]);
    _jsonData_["tel"] = string(argv[2]);
    _jsonData_["wchart"] = string(argv[3]);

    ((Json::Value *)data)->append(_jsonData_);

    return 0x00;
}

/*
    函数名称：getUserList
    函数功能：获取消息推送人员列表
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-09
*/
void getUserList(){
    Json::Value jsonList;

    string sql = "SELECT id, wname, tel, wchart FROM wperson";

    dbcontrl db(DBPATH);
    if(db.open()){ err_msg("打开数据库失败，请稍候再试"); return; }

    if(db.exec(sql.c_str(), getUserList_sqlCallback, (void *)&jsonList)){ err_msg("获取推送人员信息列表信息失败，请稍后再试"); return; }

    /*返回数据*/
    if(jsonList.size() <= 0x00){ err_msg("401", "推送人员信息为空"); return; }

    Json::Value jsonPost;
    jsonPost["error"] = 0;
    jsonPost["data"] = jsonList;

    Json::FastWriter fast_writer;
    string jsonStr = fast_writer.write(jsonPost); 

    cout<<"Content-type:text/html\r\n\r\n";
    cout<<jsonStr<<endl;
    return;
}

/*
    函数名称：addUser
    函数功能：添加用户
    传入参数：Json::Value &_jsonData {"display":2, "name":"xxx", "tel":"xxx", "wchart":"xxx"}
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-09
*/
void addUser(Json::Value &_jsonData){
    /*参数校验*/
    if(
        _jsonData["name"].isNull() ||
        _jsonData["tel"].isNull() ||
        _jsonData["wchart"].isNull() ||

        _jsonData["name"].asString().size() <= 0x00 ||
        (_jsonData["tel"].asString().size()<=0x00 && _jsonData["wchart"].asString().size()<=0x00)
    ){ err_msg("上传数据有误"); return; }

    /*添加到数据库*/
    ostringstream sql; sql.str("");
    sql << "INSERT INTO wperson(wname, tel, wchart) VALUES(\""<< _jsonData["name"].asString().c_str() <<"\", \""<< _jsonData["tel"].asString().c_str() <<"\", \""<< _jsonData["wchart"].asString().c_str() <<"\");";

    dbcontrl db(DBPATH);
    if(db.open()){ err_msg("打开数据库失败，请稍候再试"); return; }

    if(db.exec(sql.str().c_str(), NULL, NULL)){ err_msg("推送人员信息写入数据库失败，请稍后再试"); return; }

    err_msg("0", "ok");
}

/*
    函数名称：delUser
    函数功能：删除指定用户
    传入参数：Json::Value &_jsonData {"display":3, "id":"xxx"}
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-10
*/
void delUser(Json::Value &_jsonData){
    /*参数校验*/
    if(_jsonData["id"].isNull() || _jsonData["id"].asString().size()<=0x00){ err_msg("上传数据有误"); return; }

    /*数据库操作*/
    ostringstream sql; sql.str("");
    sql<< "DELETE FROM wperson WHERE id=" << _jsonData["id"].asString().c_str() << ";";

    dbcontrl db(DBPATH);
    if(db.open()){ err_msg("打开数据库失败，请稍候再试"); return; }

    if(db.exec(sql.str().c_str(), NULL, NULL)){ err_msg("删除失败"); return; }

    err_msg("0", "ok");
}