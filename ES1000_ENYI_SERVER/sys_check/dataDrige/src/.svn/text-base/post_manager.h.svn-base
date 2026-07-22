#ifndef __POST_MANAGER_H__
#define __POST_MANAGER_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <pthread.h>
#include <unistd.h>

#include "conf.h"
#include "jsonConfFile.h"
#include "post_ports.h"
#include "../include/json/json.h"

#include "post_http.h"
#include "post_tcpshort.h"
#include "post_tcplong.h"

void *post_work(void *argv);
void work_enable_cont(const char *_path);
#endif