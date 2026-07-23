#ifndef __SERVER_H__
#define __SERVER_H__
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <unistd.h> 
#include "conf.h"
#include "udp_sock_client.h"
#include "udp_sock_server.h"
#include "gbk_utf8.h"
#include "../include/json/json.h"
#include "getNet_wan.h"
#include "display.h"
#include "lsystem.h"
#include "tcp_sock_client.h"
#include "tcp_sock_pakage.h"
#include "getMac.h"
#include "udp_radio.h"
#include "net_cont.h"

void SERVER_RUN(udp_sock_server *_server);

#endif