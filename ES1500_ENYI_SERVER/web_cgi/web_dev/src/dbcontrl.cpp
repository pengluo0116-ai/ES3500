#include "dbcontrl.h"
using namespace std;

/*
    编译
    mipsel-openwrt-linux-g++ -o ../obj/dbcontrl.o  -c dbcontrl.cpp
*/

/*
    函数名称：dbcontrl
    函数功能：dbcontrl类构造函数
    传入参数：无
    输出数据：无
    注意事项：无
    编写时间：2016-02-16
    编写人员：王凤龙
*/
dbcontrl::dbcontrl(const char * _db_path){ 
    this->obj_db = (sqlite3 *)0x00; 
    this->db_path = string(_db_path);    
}

/*
    函数名称：~dbcontrl
    函数功能：dbcontrl析构造函数
    传入参数：无
    输出数据：无
    注意事项：无
    编写时间：2016-02-16
    编写人员：王凤龙
*/
dbcontrl::~dbcontrl(){
    this->close();
}

/*
    函数名称：open(void)
    函数功能：打开数据库
    传入参数：无
    输出数据：类型：int 0->打开成功 -1->打开失败
    注意事项：无
    编写时间：2016-02-16
    编写人员：王凤龙
*/
int dbcontrl::open(){
    int res = 0x00;
    
    /*打开数据库*/
    res = sqlite3_open(this->db_path.c_str(), &this->obj_db);
    
    /*判断数据库是否打开成功，成功返回0，失败返回-1*/
    if(res != SQLITE_OK) { this->obj_db = (sqlite3 *)0x00; return -1;}
    
    /*添加超时处理*/
    sqlite3_busy_timeout(this->obj_db, 400);
    return 0;
}

/*
    函数名称：close(void)
    函数功能：关闭数据库
    传入参数：无
    输出数据：类型：int 0->打开成功 -1->打开失败
    注意事项：无
    编写时间：2016-02-16
    编写人员：王凤龙
*/
int dbcontrl::close(){
    try{
        if(this->obj_db != (sqlite3 *)0x00) sqlite3_close(this->obj_db);
    }catch(...){ return -1; }
    
    this->obj_db = (sqlite3 *)0x00;
    return 0;
}

/*
    函数名称：exec
    函数功能：数据库操作
    传入参数：
                void *_fun  传入的回调函数指针
                void *_data 传入的数值
    传出数据：
                 0 运行成功
                -1 数据库打开失败
    编写人员：王凤龙
    编写时间：2016-06-01    
*/
#if 0
int dbcontrl::exec(const char *_sql, int (*_fun)(void*,int,char**,char**), void *_data){
    int res = 0x00;
    char *errMsg = (char *)0x00;
    
    if(this->open()) return -1;
    res = sqlite3_exec(this->obj_db, _sql, _fun, _data, &errMsg);
    if(res != SQLITE_OK){ sqlite3_free(errMsg); this->close(); return 1;}
    
    this->close(); return 0x00;
}
#endif

#if 1
int dbcontrl::exec(const char *_sql, int (*_fun)(void*,int,char**,char**), void *_data){
    int res = 0x00;
    char *errMsg = (char *)0x00;
    
    if(this->open()) return -1;
    res = sqlite3_exec(this->obj_db, _sql, _fun, _data, &errMsg);
    if(res != SQLITE_OK){ 
        ostringstream errlog;
        errlog << "echo '"<< errMsg <<"' > /root/dberrlog_tmp";
        lsystem(errlog.str().c_str());
        sqlite3_free(errMsg); this->close(); return 1;
    }
    
    this->close(); return 0x00;
}
#endif