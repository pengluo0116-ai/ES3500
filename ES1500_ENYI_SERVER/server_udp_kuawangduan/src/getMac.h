#ifndef __GETMAC_H__
#define __GETMAC_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h> 
#include <unistd.h>

int dev_getMac(unsigned char *macBuff, int buffLen);

#endif