#ifndef __DB_CONT_H__
#define __DB_CONT_H__

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <unistd.h>

#include "conf.h"
#include "dbcontrl.h"
#include "../include/json/json.h"

int get_group_byID(unsigned int id,Json::Value &jsonList);
int get_group_byPID(int id, Json::Value &jsonList);
int get_group_All(Json::Value &jsonList);
int set_group_del(unsigned int id);
int get_device_all(Json::Value &jsonList, int (*callback)(void *, int, char **, char **));
int get_device_all(Json::Value &jsonList);
int get_device_byGID(unsigned int gid, Json::Value &jsonList, int (*callback)(void *, int, char **, char **));
int get_device_byGID(unsigned int gid, Json::Value &jsonList);
int set_device_add(Json::Value &jsonData);
int set_device_del(Json::Value &jsonData);
int set_device_delByGroupID(unsigned int gid);
int dev_create_info(Json::Value &jsonList_dev, Json::Value &jsonList_group);
#endif