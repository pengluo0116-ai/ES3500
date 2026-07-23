#ifndef __TASK_CMD_H__
#define __TASK_CMD_H__
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "conf.h"
#include "lsystem.h"
#include "tcp_sock_client.h"
#include "tcp_sock_pakage.h"
#include "jsonConfFile.h"
#include "err_msg.h"
#include "dbcontrl.h"

#include "file_opt.h"

/*
    文档介绍：
    1：分支处理函数在此添加
    2：在task.cpp中的getPostData函数注册
*/

// void cmd_getAlarmConf();
// void cmd_setAlarmConf(Json::Value &jsonData);


void cmd_get_filelist();
void cmd_get_tar_file(Json::Value &jsonData);


#endif