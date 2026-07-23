#ifndef __DB_ALARM_H__
#define __DB_ALARM_H__

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <unistd.h>

#include "dbcontrl.h"
#include "../include/json/json.h"

class DB_alarm{
    private:
        std::string dbPath;

    public:
        DB_alarm(const char *_dbPath);   
        ~DB_alarm();

    public:
        int init();
        int num(std::string &_snum, int (*_fun)(void*,int,char**,char**) = DB_alarm::_num_callback);                                                                                    //获取报警信息数量                           
        int getPData(Json::Value &jsonList, unsigned int _page, unsigned int _pageNum, int (*_fun)(void*,int,char**,char**) = DB_alarm::_getPData_callback);            //获取报警信息
    
    private:
        static int _num_callback(void *data, int argc, char **argv, char **azColName);                                                                              //回调函数--获取报警信息数量
        static int _getPData_callback(void *data, int argc, char **argv, char **azColName);                                                                          //回调函数--获取报警信息
};

#endif