#ifndef __TASK_CMD_H__
#define __TASK_CMD_H__
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <unistd.h>

#include "conf.h"
#include "lsystem.h"
#include "tcp_sock_client.h"
#include "tcp_sock_pakage.h"
#include "jsonConfFile.h"
#include "err_msg.h"
#include "getNet_wan.h"
#include "net_cont.h"
#include "getMac.h"

/*
    文档介绍：
    1：分支处理函数在此添加
    2：在task.cpp中的getPostData函数注册
*/

void getGateway();                                  //获取设备网络信息
void getHexVersion();                               //获取设备版本信息
void cmd_setDevNet(Json::Value &jsonData);          //设置设备网络信息
void sysReboot();                                   //重启设备
void getDNS();                                      //获取设备的DNS
void setDNS(Json::Value &jsonData);                 //设置设备的DNS
void cmd_setPostPswd(Json::Value &_jsonData);
#endif