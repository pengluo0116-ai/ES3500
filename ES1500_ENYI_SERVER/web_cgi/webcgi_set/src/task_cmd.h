#ifndef __TASK_CMD_H__
#define __TASK_CMD_H__
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <time.h>

#include "conf.h"
#include "lsystem.h"
#include "jsonConfFile.h"
#include "err_msg.h"

/*
    文档介绍：
    1：分支处理函数在此添加
    2：在task.cpp中的getPostData函数注册
*/

void cmd_getConf();
//void cmd_setConf(Json::Value &jsonData);
void cmd_setTime(Json::Value &jsonData);
void cmd_getTime();
#endif