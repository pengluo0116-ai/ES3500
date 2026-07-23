#ifndef __DISPLAY_H__
#define __DISPLAY_H__
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include   <sys/ioctl.h> 
#include   <sys/socket.h> 
#include   <netinet/in.h> 
#include   <net/if.h> 

int r(int fanwei);                                                              //获取随机数
int getMac(const char *_wlan_name, unsigned char *_mac);                        //获取MAC地址
#endif