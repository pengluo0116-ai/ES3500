#ifndef __REDIS_DEVUPDATE_H__
#define __REDIS_DEVUPDATE_H__
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>

#include "conf.h"
#include "redis_client.h"
#include "../include/json/json.h"

int cmd_device_redisUpdate(Json::Value &jsonList_dev, Json::Value &jsonList_group);
int redis_devDel(const char *_devID);
int redis_devListDel(Json::Value &jsonList);
#endif