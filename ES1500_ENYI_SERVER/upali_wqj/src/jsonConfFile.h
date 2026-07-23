#ifndef __JSONCONFFILE_H__
#define __JSONCONFFILE_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include <unistd.h>
#include "../include/json/json.h" 
#include <string.h>
using namespace std;
/*
    实现功能：读取和写入配置文件
    备注信息：
                1：配置文件中的内容需为json格式
                2：需要调用json库
    编写人员：王凤龙
*/

int JreadConf(const char *conf_path, Json::Value &val);
int JwriteConf(const char *conf_path, Json::Value &val);
int JreadStrConf(const char *conf_path, string &val);
int JreadExeConf(const char *conf_path, string &val);
#endif
