#ifndef __TCP_SOCK_PAKAGE_H__
#define __TCP_SOCK_PAKAGE_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstring>
#include <unistd.h>
#include "crc8.h"
#include "../include/json/json.h"

struct tcp_package_modle{
    unsigned short head;        
    unsigned char crc8;
    unsigned char revd;         //保留
    unsigned char data[1000];
};

int dataToPakage(const char *str, struct tcp_package_modle &_pakage);           //将字符串转换成发送包
int dataToPakage(Json::Value &jsonData, struct tcp_package_modle &_pakage);     //将json转换成发送包
int pakageToJson(unsigned char *getData, int len, Json::Value &jsonData);       //将发送包解析成json

#endif
