#ifndef __DBCONTRL_H__
#define __DBCONTRL_H__

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "../include/sqlite/sqlite3.h"

#if 1
#include <sstream>
#include "lsystem.h"
#endif

/****************************************************************************
    文件说明：sqlite3数据库操作基类
    实现功能：实现数据库的开启，关闭功能
    注意事项：需要调用sqlite库，并且需要引用sqlite头文件
    编译说明：-lsqlite3
****************************************************************************/

class dbcontrl{
    private:
        std::string db_path;     //数据库路径
    public:
        sqlite3 *obj_db;    //数据库结构指针
        
    public:
        dbcontrl(const char * _db_path);          //构造函数
        ~dbcontrl();         //析构函数
        
        int open();         //打开数据库
        int close();        //关闭数据库
        int exec(const char *_sql, int (*_fun)(void*,int,char**,char**),  void *_data);
};

#endif