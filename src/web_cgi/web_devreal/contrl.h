#ifndef __CONTRL_H__
#define __CONTRL_H__
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <unistd.h>
#include "../include/json/json.h"
#include "dbcontrl.h"
#include "conf.h"
#include "redis_client.h"
#include "dateTime_cont.h"

void getPostData();
void err_msg(const char *_err_msg);
void err_msg(const char *_code, const char *_err_msg);
#endif