#ifndef __TASK_MANAGER_H__
#define __TASK_MANAGER_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include "task.h"

#include "task_udpserver.h"
#include "task_web.h"
#include "task_redis.h"
#include "task_qt.h"
#include "task_uwsgi.h"
#include "task_mqttServer.h"

class task_manager{
    private:
        static std::vector<task *> task_vct;            //进程组

    private:
        static void *thread_work(void *argvs);          //进程启动器

    public:
        static int init();                              //初始化
        static int work();                              //工作
        static int getPidByName(const char *pName);     //根据任务名称获取进程号
};

#endif
