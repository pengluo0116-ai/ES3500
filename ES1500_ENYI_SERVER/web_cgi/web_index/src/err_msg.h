#ifndef __ERR_MSG_H__
#define __ERR_MSG_H__
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

void err_msg(const char *_err_msg);
void err_msg(const char *_code, const char *_err_msg);

#endif