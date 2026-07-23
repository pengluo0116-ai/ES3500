#include "task_cmd.h"
using namespace std;

static int redis_update_devByGroup(Json::Value &jsonGroup);
static int get_device_callback_1(void *data, int argc, char **argv, char **azColName);
static int get_device_byGID(unsigned int gid, Json::Value &jsonList, int (*callback)(void *, int, char **, char **));
static int device_redisUpdate();
static int set_device_delByGroupID(unsigned int gid);
/*******************************************************************************/

/*
    函数名称：get_group_callback
    函数功能：获取小组属性信息回调函数
    传入参数：
                data 调用该函数的父函数传给该函数的值
                argc 一条查询记录中，列数
                argv 查询到结果以字符串列表的形式存到改变量中
                azColName 一条查询结果中，以字符串列表的形式，存储所有的列的名称
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-22
*/
static int get_group_callback(void *data, int argc, char **argv, char **azColName){
    Json::Value *jsonList = (Json::Value *)data;
    Json::Value jsonTmp;

    jsonTmp["id"] = atoi(argv[0]);
    jsonTmp["pid"] = atoi(argv[1]);
    jsonTmp["name"] = argv[2] ? argv[2] : "";
    jsonTmp["note"] = argv[3] ? argv[3] : "";

    jsonList->append(jsonTmp);
    return 0x00;
}

/*
    函数名称：get_group_byID
    函数功能：根据组ID，获取组信息
    传入参数：Json::Value &jsonList 获取到的组信息存储到该引用指向的存储空间
    传出数据：0 运行成功 非0 运行失败
    注意事项：内部函数
    编写人员：王凤龙
    编写时间：2017-05-22
*/
static int get_group_byID(unsigned int id,Json::Value &jsonList){
    ostringstream sql; sql.str("");
    sql << "SELECT id, pid, name, note FROM group_info WHERE id = " << id << ";";

    jsonList.clear();
    dbcontrl db(DBPATH);
    if(db.exec(sql.str().c_str(), get_group_callback, (void *)&jsonList)) return -0x01;
    return 0x00;
}

/*
    函数名称：get_group_byPID
    函数功能：根据父组ID，获取组信息
    传入参数：Json::Value &jsonList 获取到的组信息存储到该引用指向的存储空间
    传出数据：0 运行成功 非0 运行失败
    注意事项：内部函数
    编写人员：王凤龙
    编写时间：2017-05-24
*/
static int get_group_byPID(int id, Json::Value &jsonList){
    ostringstream sql; sql.str("");
    sql << "SELECT id, pid, name, note FROM group_info WHERE pid = " << id << ";";

    jsonList.clear();
    if(id == 0x00) return 0x00;         //根组

    dbcontrl db(DBPATH);
    if(db.exec(sql.str().c_str(), get_group_callback, (void *)&jsonList)) return -0x01;
    return 0x00;
}

/*
    函数名称：get_group_All
    函数功能：获取所有的小组信息
    传入参数：Json::Value &jsonList  获取到的组信息存储到该引用指向的存储空间
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
static int get_group_All(Json::Value &jsonList){
    ostringstream sql; sql.str("");
    sql << "SELECT id, pid, name, note FROM group_info;";

    jsonList.clear();
    dbcontrl db(DBPATH);
    if(db.exec(sql.str().c_str(), get_group_callback, (void *)&jsonList)) return -0x01;
    return 0x00;
}

/*
    函数名称：set_group_add_callback
    函数功能：添加小组，回调函数
    传入参数：
                data 调用该函数的父函数传给该函数的值
                argc 一条查询记录中，列数
                argv 查询到结果以字符串列表的形式存到改变量中
                azColName 一条查询结果中，以字符串列表的形式，存储所有的列的名称
    传出数据：保留 0
    注意事项：回调函数用于获取新添加小组的ID
    编写人员：王凤龙
    编写时间：2017-05-22
*/
static int set_group_add_callback(void *data, int argc, char **argv, char **azColName){
    Json::Value *jsonGroup = (Json::Value *)data;
    (*jsonGroup)["id"] = atoi(argv[0]);
    return 0x00;
}

/*
    函数名称：set_group_add
    函数功能：添加小组
    传入参数：jsonGrop { "pid":根组ID, "name":"组名称" }
    传出数据：
    注意事项：内部函数
    编写人员：王凤龙
    编写时间：2017-05-22
*/
static int set_group_add(Json::Value &jsonGroup){
    ostringstream sql; sql.str("");
    sql << "INSERT INTO group_info(pid, name) VALUES("<< jsonGroup["pid"].asInt() <<", \""<< jsonGroup["name"].asString() <<"\"); SELECT MAX(id) FROM group_info;";

    dbcontrl db(DBPATH);
    if(db.exec(sql.str().c_str(), set_group_add_callback, (void *)&jsonGroup)){ return -0x01; }
    return 0x00;
}

/*
    函数名称：set_group_del
    函数功能：删除小组--根据组ID
    传入参数：unsigned int id
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
static int set_group_del(unsigned int id){
    ostringstream sql;
    sql << "DELETE FROM group_info WHERE id = " << id << ";";

    dbcontrl db(DBPATH);
    if(db.exec(sql.str().c_str(), NULL, NULL)) return -0x01;

    return 0x00;
}

/*
    函数名称：cmd_add_group
    函数功能：添加小组
    传入参数：jsonGroup { "pid":根组ID, "name":"组名称" }
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-22
*/
void cmd_add_group(Json::Value &jsonGroup){
    /*参数校验*/
    if(jsonGroup["pid"].isNull() || jsonGroup["name"].isNull() || !jsonGroup["pid"].isInt()){ err_msg("上传数据有误"); return; }

    /*检测父组*/
    if(jsonGroup["pid"].asInt()){
        Json::Value jsonList; 
        if(get_group_byID(jsonGroup["pid"].asInt(), jsonList)){ err_msg("服务器故障[G1]，请稍后再试"); return; }
        if(jsonList.size() <= 0x00){ err_msg("根目录已经不存在，请刷新页面后再操作"); return; }
    }

    /*添加小组*/
    if(set_group_add(jsonGroup)){ err_msg("添加小组失败，请稍后再试"); return; }

    /*返回数据*/
    Json::Value jsonRSP;
    jsonRSP["error"] = 0;
    jsonRSP["data"] = jsonGroup;
    cout<<"Content-type:text/html\r\n\r\n";
    cout<<jsonRSP.toStyledString()<<endl;
}

/*
    函数名称：cmd_update_group
    函数功能：修改小组信息
    传入参数：Json::Value jsonGroup { "id":设备ID "name":"组名称" }
    传出数据：无
    注意事项：修改属性信息--组名称
    编写人员：王凤龙
    编写时间：2017-05-24
*/
void cmd_update_group(Json::Value &jsonGroup){
    /*参数校验*/
    if(jsonGroup["id"].isNull() || !jsonGroup["id"].isInt() || jsonGroup["name"].isNull()){ err_msg("上传数据有误"); return; }

    /*数据库操作*/
    ostringstream sql;
    sql << "UPDATE group_info SET name = \""<< jsonGroup["name"].asString() <<"\" WHERE id="<< jsonGroup["id"].asInt() <<";";

    dbcontrl db(DBPATH);
    if(db.exec(sql.str().c_str(), NULL, NULL)){ err_msg("服务器故障[G2]，请稍候再试"); return; }

    /*REDIS更新*/
    redis_update_devByGroup(jsonGroup);

    err_msg("0", "");
}

/*
    函数名称：cmd_get_groupAll
    函数名称：获取所有小组信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
void cmd_get_group(){
    Json::Value jsonList;
    if(get_group_All(jsonList)){ err_msg("获取小组信息失败，请稍后再试"); return; }

    if(jsonList.size() <= 0x00){
        string err_msg = "{\"error\":0, \"data\":[]}";
        cout<<"Content-type:text/html\r\n\r\n";
        cout<<err_msg<<endl;
        return;
    }

    Json::Value jsonRSP;
    jsonRSP["error"] = 0;
    jsonRSP["data"] = jsonList;
    cout<<"Content-type:text/html\r\n\r\n";
    cout<<jsonRSP.toStyledString()<<endl;
}

/*
    函数名称：cmd_del_group
    函数功能：删除组
    传入参数：Json::Value &jsonDta {"id":"组ID"}
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
void cmd_del_group(Json::Value &jsonData){
    Json::Value jsonList;
    
    /*参数校验*/
    if(jsonData["id"].isNull() || !jsonData["id"].isInt()){ err_msg("上传数据有误，请稍后再试"); return; }

    /*校验该组下是否含有子组*/
    if(get_group_byPID(jsonData["id"].asInt(), jsonList)){ err_msg("服务器故障[GD1]"); return; }
    if(jsonList.size() > 0x00){ err_msg("该组下含有其它组或者设备，无法进行删除"); return; }

    /*校验该组下是否含有设备*/
    if(get_device_byGID(jsonData["id"].asInt(), jsonList, get_device_callback_1)){ err_msg("服务器故障[GD2]"); return; }
    if(jsonList.size() > 0X00){ err_msg("该组下含有其它组或者设备，无法进行删除"); return; }

    /*删除组*/
    if(set_group_del(jsonData["id"].asInt())){ err_msg("删除失败，请稍后再试"); return; }

    err_msg("0", "");
}

/*
    函数名称：set_groupDev_del
    函数功能：删除组及组下的设备
    传入参数：unsigned int gid
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-02
*/
static int set_groupDev_del(unsigned int gid){
    /*获取组所有设备的信息*/
    Json::Value jsonList;
    if(get_device_byGID(gid, jsonList, get_device_callback_1)) return -0x01;

    /*删除设备*/
    if(set_device_delByGroupID(gid)) return -0x02;

    /*删除组*/
    if(set_group_del(gid)) return -0x03;

    /*更新redis*/
    if(redis_devListDel(jsonList)) return -0x04;

    return 0x00;
}

/*
    函数名称：cmd_del_group_r
    函数功能：强制删除组及该组下所有的子组和设备
    传入参数：Json::Value &jsonDta {"id":"组ID"}
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-02
*/
void cmd_del_group_r(Json::Value &jsonData){
    Json::Value jsonList;
    
    /*参数校验*/
    if(jsonData["id"].isNull() || !jsonData["id"].isInt()){ err_msg("上传数据有误，请稍后再试"); return; }

    /*获取子组信息*/
    if(get_group_byPID(jsonData["id"].asInt(), jsonList)){ err_msg("服务器故障[GGD1]"); return; }

    /*删除子组及子组设备*/
    for(unsigned int point=0x00; point<jsonList.size(); point++){
        if(int err = set_groupDev_del(jsonList[point]["id"].asInt())){ err_msg("服务器故障[GGD2]"); return; }
    }
    
    /*删除组及设备*/
    if(int err = set_groupDev_del(jsonData["id"].asInt())){ err_msg("服务器故障[GGD3]"); return; }

    err_msg("0", "");
}

/*******************************************************************************/

/*
    函数名称：redis_update_devByGroup
    函数功能：REDIS修改设备信息，组修改联动
    传入参数：Json::Value &jsonGroup
    传出数据：
    注意事项：当组信息修改后，会修改REDIS中的设备信息
    编写人员：王凤龙
    编写时间：2017-05-24
*/
static int redis_update_devByGroup(Json::Value &jsonGroup){
    return 0x00;
}

/*******************************************************************************/

/*
    函数名称：get_device_callback_1
    函数功能：获取设备信息回调函数
    传入参数：
                data 调用该函数的父函数传给该函数的值
                argc 一条查询记录中，列数
                argv 查询到结果以字符串列表的形式存到改变量中
                azColName 一条查询结果中，以字符串列表的形式，存储所有的列的名称
    传出数据：保留 0
    注意事项：配置客户端
    编写人员：王凤龙
    编写时间：2017-05-24
*/
static int get_device_callback_1(void *data, int argc, char **argv, char **azColName){
    Json::Value *jsonList = (Json::Value *)data;
    Json::Value jsonTmp;

    jsonTmp["id"]             = argv[0];
    jsonTmp["gid"]            = atoi(argv[1]);
    jsonTmp["name"]           = argv[2];
    jsonTmp["url"]            = argv[3];
    jsonTmp["x"]              = atoi(argv[4]);
    jsonTmp["y"]              = atoi(argv[5]);
    jsonTmp["note"]           = argv[6];
    jsonTmp["madein"]         = argv[7];
    jsonTmp["property"]       = argv[8];
    jsonTmp["mainter"]        = argv[9];
    jsonTmp["mainter_tel"]    = argv[10];
    jsonTmp["address"]        = argv[11];
    jsonTmp["mainter_time"]   = argv[12];
    jsonTmp["mainter_period"] = atoi(argv[13]);

    jsonList->append(jsonTmp);
    return 0x00;
}

/*
    函数名称：get_device_callback_2
    函数功能：获取设备信息回调函数
    传入参数：
                data 调用该函数的父函数传给该函数的值
                argc 一条查询记录中，列数
                argv 查询到结果以字符串列表的形式存到改变量中
                azColName 一条查询结果中，以字符串列表的形式，存储所有的列的名称
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
static int get_device_callback_2(void *data, int argc, char **argv, char **azColName){
    Json::Value *jsonList = (Json::Value *)data;
    Json::Value jsonTmp;

    jsonTmp["id"]             = argv[0];
    jsonTmp["gid"]            = atoi(argv[1]);
    jsonTmp["dev_name"]       = argv[2];
    jsonTmp["url"]            = argv[3];
    jsonTmp["x"]              = atoi(argv[4]);
    jsonTmp["y"]              = atoi(argv[5]);
    jsonTmp["note"]           = argv[6];
    jsonTmp["madein"]         = argv[7];
    jsonTmp["property"]       = argv[8];
    jsonTmp["mainter"]        = argv[9];
    jsonTmp["mainter_tel"]    = argv[10];
    jsonTmp["address"]        = argv[11];
    jsonTmp["mainter_time"]   = argv[12];
    jsonTmp["mainter_period"] = argv[13];

    jsonList->append(jsonTmp);
    return 0x00;
}

/*
    函数名称：get_device_all
    函数功能：获取所有设备信息
    传入参数：
                Json::Value &josnList    获取到的数据存储到该引用指向的存储空间
                int callback(void *data, int argc, char **argv, char **azColName)
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
static int get_device_all(Json::Value &jsonList, int (*callback)(void *, int, char **, char **)){
    ostringstream sql;
    sql << "SELECT id, gid, dev_name, url, x, y, note, madein, property, mainter, mainter_tel, address, mainter_time, mainter_period FROM dev_info;";

    jsonList.clear();
    dbcontrl db(DBPATH);
    if(db.exec(sql.str().c_str(), callback, (void *)&jsonList)){ return -0x01; }

    return 0x00;
}

/*
    函数名称：get_device_byID
    函数功能：根据设备ID获取设备信息
    传入参数：
                const char *id          设备ID
                Json::Value &jsonList   获取到的数据存储到该引用指向的存储空间
                int callback(void *data, int argc, char **argv, char **azColName)
    传出数据：
                 0  运行成功
                -1  参数校验有误
                -2  数据库操作失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
static int get_device_byID(const char *id, Json::Value &jsonList, int (*callback)(void *, int, char **, char **)){
    /*参数校验*/
    if(!id || strlen(id)<=0x00) return -0x01;

    ostringstream sql;
    sql << "SELECT id, gid, dev_name, url, x, y, note, madein, property, mainter, mainter_tel, address, mainter_time, mainter_period FROM dev_info WHERE id = \""<< id <<"\";";

    jsonList.clear();
    dbcontrl db(DBPATH);
    if(db.exec(sql.str().c_str(), callback, (void *)&jsonList)){ return -0x02; }

    return 0x00;
}

/*
    函数名称：get_device_byGID
    函数功能：获取指定组ID下面的所有设备
    传入参数：
                unsigned int gid            组ID
                Json::Value &jsonList       获取到的数据存储到该引用指向的存储空间
                int callback(void *data, int argc, char **argv, char **azColName)
    传出数据：0  运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
static int get_device_byGID(unsigned int gid, Json::Value &jsonList, int (*callback)(void *, int, char **, char **)){
    ostringstream sql;
    sql << "SELECT id, gid, dev_name, url, x, y, note, madein, property, mainter, mainter_tel, address, mainter_time, mainter_period FROM dev_info WHERE gid = " << gid << ";";

    jsonList.clear();
    dbcontrl db(DBPATH);
    if(db.exec(sql.str().c_str(), callback, (void *)&jsonList)){ return -0x01; }

    return 0x00;
}

/*
    函数名称：set_device_add
    函数功能：添加设备
    传入参数：Json::Value &jsonData
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
static int set_device_add(Json::Value &jsonData){
    ostringstream sql;
    sql << "INSERT INTO dev_info(id, gid, dev_name, url, x, y, madein, property, mainter, mainter_tel, address, note, mainter_time, mainter_period) "
        << "VALUES(\"" << jsonData["id"].asString() << "\", " << jsonData["gid"].asInt() <<", "
        <<"\""<< jsonData["name"].asString() <<"\", \""<< jsonData["url"].asString() <<"\", "<< jsonData["x"].asInt() <<", "<< jsonData["y"].asInt() 
        <<", \""<< jsonData["madein"].asString() <<"\", \""<< jsonData["property"].asString() <<"\", \""<< jsonData["mainter"].asString() <<"\", \""<< jsonData["mainer_tel"].asString() 
        <<"\", \""<< jsonData["address"].asString() <<"\", \""<< jsonData["note"].asString() << "\", \"0\", \"" << jsonData["mainter_period"].asString() << "\")";

    dbcontrl db(DBPATH);
    if(db.exec(sql.str().c_str(), NULL, NULL)) return -0x01;
    return 0x00;
}

/*
    函数名称：set_device_update
    函数功能：修改设备属性
    传入参数：Json::Value &jsonData
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-25
*/
static int set_device_update(Json::Value &jsonData){
    ostringstream sql;
    sql << "UPDATE dev_info set "
        << "dev_name = \""      << jsonData["name"].asString()           <<"\", "
        << "url = \""           << jsonData["url"].asString()            <<"\", "
        << "x ="                << jsonData["x"].asInt()                 <<", "
        << "y ="                << jsonData["y"].asInt()                 <<", "
        << "madein = \""        << jsonData["madein"].asString()         <<"\", "
        << "property = \""      << jsonData["property"].asString()       <<"\", "
        << "mainter = \""       << jsonData["mainter"].asString()        <<"\", "
        << "mainter_tel = \""   << jsonData["mainter_tel"].asString()    <<"\", "
        // << "mainter_time = "    << jsonData["mainter_time"].asString()   << ", "
        << "mainter_period = \""<< jsonData["mainter_period"].asString() <<"\", "
        << "address = \""       << jsonData["address"].asString()        <<"\", "
        << "note = \""          << jsonData["note"].asString()           <<"\" "
        << "WHERE id = \""      << jsonData["id"].asString()             <<"\";";

    dbcontrl db(DBPATH);
    if(db.exec(sql.str().c_str(), NULL, NULL)) return -0x01;
    return 0x00;
}

/*
    函数名称：set_device_del
    函数功能：删除设备
    传入参数：Json::Value &jsonData
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-25
*/
static int set_device_del(Json::Value &jsonData){
    ostringstream sql;
    sql << "DELETE FROM dev_info WHERE id = \""<< jsonData["id"].asString() <<"\";";

    dbcontrl db(DBPATH);
    if(db.exec(sql.str().c_str(), NULL, NULL)) return -0x01;
    return 0x00;
}

/*
    函数名称：updata_device_maintain()     函数功能:更新设备的维保时间
    传入参数：jsonData  返回值：0成功 非零 不成功
    编写人：周广阔  编写时间：2020-12-11
*/
static int updata_device_maintain(Json::Value &jsonData)
{
    ostringstream sql;
    sql << "UPDATE dev_info set maintain = \"" << jsonData["maintain"] << "\" WHERE id = \"" << jsonData["id"].asString().c_str() << "\";";
    dbcontrl db(DBPATH);
    if(db.exec(sql.str().c_str(), NULL, NULL)) return -0x01;
    return 0x00;
}

/*
    函数名称：cmd_updata_Maintain()     函数功能:更新设备的维保时间
    传入参数：jsonData  返回值：0成功 非零 不成功
    编写人：周广阔  编写时间：2020-12-11
*/
void cmd_updata_Maintain(Json::Value &jsonData)
{
    if(updata_device_maintain(jsonData)){ err_msg("更改维保时间错误"); return; }
    Json::Value jsonRSP;
    jsonRSP["error"] = 0;
    cout<<"Content-type:text/html\r\n\r\n";
    cout<<jsonRSP.toStyledString()<<endl;
    return ;
}

/*
    函数名称：set_device_delByGroupID
    函数功能：删除指定组ID下的设备
    传入参数：unsigned int gid
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-02
*/
static int set_device_delByGroupID(unsigned int gid){
    ostringstream sql;
    sql << "DELETE FROM dev_info WHERE gid = "<< gid <<";";

    dbcontrl db(DBPATH);
    if(db.exec(sql.str().c_str(), NULL, NULL)) return -0x01;
    return 0x00;
}

/*
    函数名称：cmd_device_getAll
    函数功能：获取所有设备信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
*/
void cmd_device_getAll(){
    Json::Value jsonList;
    if(get_device_all(jsonList, get_device_callback_1)){ err_msg("服务器故障[D1]，请稍后再试"); return; }

    if(jsonList.size() <= 0x00){
        string err_msg = "{\"error\":0, \"data\":[]}";
        cout<<"Content-type:text/html\r\n\r\n";
        cout<<err_msg<<endl;
        return;
    }

    Json::Value jsonRSP;
    jsonRSP["error"] = 0;
    jsonRSP["data"] = jsonList;
    cout<<"Content-type:text/html\r\n\r\n";
    cout<<jsonRSP.toStyledString()<<endl;
}

/*
    函数名称：cmd_device_getByID
    函数功能：根据设备ID获取设备信息
    传入参数：Json::Value &jsonData
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-25
*/
void cmd_device_getByID(Json::Value &jsonData){
    Json::Value jsonList;
    if(get_device_byID(jsonData["id"].asString().c_str(), jsonList, get_device_callback_1)){ err_msg("获取信息失败，请稍后再试"); return; }

    Json::Value jsonRSP;
    jsonRSP["error"] = 0;
    jsonRSP["data"] = jsonList;
    cout<<"Content-type:text/html\r\n\r\n";
    cout<<jsonRSP.toStyledString()<<endl;
}

/*
    函数名称：cmd_device_add
    函数功能：添加设备
    传入参数：Json::Value &jsonData
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
void cmd_device_add(Json::Value &jsonData){
    Json::Value jsonList;

    /*参数校验*/

    /*校验组是否存在*/
    if(get_group_byID(jsonData["gid"].asInt(), jsonList)){ err_msg("服务器故障[DA1]，请稍后再试"); return; }
    if(jsonList.size() <= 0x00){ err_msg("添加设备的组不存在，请刷新页面后操作"); return; }

    /*校验ID是否重复*/
    if(get_device_byID(jsonData["id"].asString().c_str(), jsonList, get_device_callback_1)){ err_msg("服务器故障[DA2]，请稍后再试"); return; }
    if(jsonList.size() > 0x00){ err_msg("设备ID重复，相同ID的设备不可以重复添加"); return; }

    /*添加设备*/
    if(set_device_add(jsonData)){ err_msg("设备添加失败，请稍后再试"); return; }
    
    /*写入缓存*/
    device_redisUpdate();

    err_msg("0", "");
}

/*
    函数名称：cmd_device_update
    函数功能：修改设备信息
    传入参数：Json::Value &jsonData
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-25
*/
void cmd_device_update(Json::Value &jsonData){
    Json::Value jsonList;

    /*参数校验*/

    /*校验组是否存在*/
    if(get_group_byID(jsonData["gid"].asInt(), jsonList)){ err_msg("服务器故障[DA1]，请稍后再试"); return; }
    if(jsonList.size() <= 0x00){ err_msg("修改设备的组不存在，请刷新页面后操作"); return; }

    /*校验ID是否重复*/
    if(get_device_byID(jsonData["id"].asString().c_str(), jsonList, get_device_callback_1)){ err_msg("服务器故障[DA2]，请稍后再试"); return; }
    if(jsonList.size() <= 0x00){ err_msg("该设备已不存在，无法提交修改信息"); return; }

    /*修改信息*/
    if(set_device_update(jsonData)){ err_msg("修改设备信息失败，请稍后再试"); return; }
    
    /*写入缓存*/
    device_redisUpdate();
    
    err_msg("0", "");
    return ;
}

/*
    函数名称：cmd_device_del
    函数功能：删除设备
    传入参数：Json::Value &jsonData
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-25
*/
void cmd_device_del(Json::Value &jsonData){
    /*参数校验*/

    /*删除设备*/
    if(set_device_del(jsonData)){ err_msg("设备删除失败，请稍后再试"); return; }

    /*更新REDIS*/
    redis_devDel(jsonData["id"].asString().c_str());

    err_msg("0", "");
}

/*
    函数名称：cmd_map_getPath
    函数功能：获取地图图片目录
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-25
*/
void cmd_get_map_getPath(){
    Json::Value jsonData;
    if(JreadConf(MAP_CONF_PATH, jsonData)){ err_msg("获取地图图片信息失败，可能未上传"); return; }

    jsonData["error"] = 0;
    cout<<"Content-type:text/html\r\n\r\n";
    cout<<jsonData.toStyledString()<<endl;
}

/*
    函数名称：cmd_MqttInfo
    函数功能：获取MQTT服务器端配置信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
void cmd_MqttInfo(){
    getNet_wan getWan;
    if(getWan.getNet_wan_data()){ err_msg("获取服务器连接信息失败[M0]"); return; }

    Json::Value jsonData; jsonData.clear();
    jsonData["ip"] = getWan.ip;
    jsonData["port"] = MQTT_PORT;
    jsonData["t"] = MQTT_TOPIC;
    jsonData["error"] = 0;

    cout<<"Content-type:text/html\r\n\r\n";
    cout<<jsonData.toStyledString()<<endl;
}

/*
    函数名称：device_redisUpdate
    函数功能：设备信息更新至REDIS
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-26
*/
static int device_redisUpdate(){
    Json::Value jsonList_dev, jsonList_group;
    
    /*获取设备列表*/
    if(get_device_all(jsonList_dev, get_device_callback_2)) return -0x01;

    /*获取组列表*/
    if(get_group_All(jsonList_group)) return -0x02;

    /*更新至REDIS*/
    if(cmd_device_redisUpdate(jsonList_dev, jsonList_group)) return -0x03;

    return 0x00;
}