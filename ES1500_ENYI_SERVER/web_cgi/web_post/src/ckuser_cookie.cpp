#include "ckuser_cookie.h"
using namespace std;

/*
    编译
    mipsel-openwrt-linux-g++ -o ../obj/ckuser_cookie.o -c ckuser_cookie.cpp
    
*/

/*
    函数名称：ckuser_cookie
    函数功能：构造函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-20
*/
ckuser_cookie::ckuser_cookie(){
    
}

/*
    函数名称：~ckuser_cookie
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-20
*/
ckuser_cookie::~ckuser_cookie(){
    
}

/**********************************************************************************************/
/*
    函数名称：ckUser_callback
    函数功能：数据库查询回调函数
    
    传入参数：
        data 调用该函数的父函数传给该函数的值
        argc 一条查询记录中，列数
        argv 查询到结果以字符串列表的形式存到改变量中
        azColName 一条查询结果中，以字符串列表的形式，存储所有的列的名称
        
    输出数据：-1：代表查询失败 0:代表登录信息错误 >0代表有多少个复合登录信息的条数
    注意事项：在查询时候，有几条查询结果，该回调函数就被调用几次
    编写时间：2016-02-16
    编写人员：王凤龙
*/
static int ckUser_callback(void *data, int argc, char **argv, char **azColName){
   
   int *_data = (int *)data;
   char chr_num[8] = {0};
   sprintf(chr_num, "%s", argv[0]);
   if(sscanf(chr_num, "%d", _data) < 1) *_data = -1;
   
   return 0;
}
/**********************************************************************************************/

/*
    函数名称：ckUser
    函数功能：从cookie中读取用户名和密码并进行校验
    传入参数：无
    传出数据：
            0 校验成功
            1 校验失败
            2 未登录 客户端不存在cookie
            3 cookie数据有误
            -1 数据库操作错误
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-20
*/
int ckuser_cookie::ckUser(){
    /*获取cookie串*/
    string usr_str = "";
    if(cookie::getCookie(usr_str)) return 0x02;
    
    /*从cookie串中解析出用户名和密码*/
    string jsonStr = base64_decode(usr_str);        //解码为json串
    Json::Reader reader;
    Json::Value jsonData;
    string _name = "";
    string _pasd = "";
    try{
        if(!reader.parse(jsonStr, jsonData)) throw(""); 
        _name = jsonData["name"].asString();
        _pasd = jsonData["pasd"].asString();
        if(
            _name.size() <= 0x00 ||
            _pasd.size() <= 0x00
        )throw("");
    }catch(...){ return 3;}
    
    /*校验用户名密码是否正确*/
    int ck_res = 0x00;
    for(int i=0x00; i<4; i++){
        ck_res = this->ckUserBySql(_name.c_str(), _pasd.c_str());
        if(ck_res == 1) return 0x00; //校验成功
        if(ck_res > 1) return 0x01;  //校验失败
        if(ck_res == -1){ sleep(1); continue;} //数据库操作失败
    }
    return ck_res;
}

/*
    函数名称：ckUser
    函数功能：从cookie中读取用户名和密码并进行校验
    传入参数：const char *_name->用户名 const char *_pasd->密码
    传出数据：
            1 存在
            0 用户名或密码错误
            -1 数据库操作错误
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-20
*/
int ckuser_cookie::ckUserBySql(const char *_name, const char *_pasd){
    int res = 0x00;
    int isHave = 0x00; //存储返回查询信息
    char *errMsg = (char *)0x00;
    dbcontrl db((const char *)DBPATH);
    
    string sql = "SELECT COUNT(*) as usernum FROM usr_log WHERE username='"+ string(_name) +"' and userpasd='"+ string(_pasd) +"';";
    res = db.open();
    res = sqlite3_exec(db.obj_db, sql.c_str(), ckUser_callback, (void *)&isHave, &errMsg);
    if(res != SQLITE_OK) {sqlite3_free(errMsg); isHave = -1; }
    db.close();
    
    return isHave;
}

/*
    函数名称：writeUserToCookie
    函数功能：将登录信息写入cookie
    传入参数：const char *_name->用户名 const char *_pasd->密码
    传出数据：0 保留
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-20
*/
int ckuser_cookie::writeUserToCookie(const char *_name, const char *_pasd){
    string jsonStr = "{\"name\":\""+ string(_name) +"\", \"pasd\":\""+ string(_pasd) +"\"}";
    string base64_str = base64_encode((unsigned char const*)jsonStr.c_str(), ( unsigned int)strlen(jsonStr.c_str()));
    cookie::setCookie(base64_str.c_str());
    return 0x00;
}