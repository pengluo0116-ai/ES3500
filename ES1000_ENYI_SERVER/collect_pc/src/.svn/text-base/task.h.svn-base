#ifndef __TASK_H__
#define __TASK_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>
#include <unistd.h>

#include "conf.h"
#include "shm_cont.h"
#include "stc_cont.h"
#include "gbk_utf8.h"
#include "tcp_sock_client.h"
#include "tcp_sock_pakage.h"
#include "../include/json/json.h"

class task{
    private:
        stc_cont *stc_uart;                                                             //STC串口
        shm_cont *str_shm_list[SHM_MAXNUM];                                                         //共享内存列表
        unsigned char *utf8StrBuff;
    
    public:
        task();                                                                         //构造函数
        ~task();                                                                        //析构函数
    
    private:
        int cmd_contrl(struct stc_package *pkg);                                                               //指令执行函数
        int cmd_showStr(struct stc_package *pkg);

    public:
        int init();                                                                     //初始化
        int RUN();                                                                      //运行函数
};

#endif