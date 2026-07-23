#ifndef __TASK_REDIS_H__
#define __TASK_REDIS_H__
#include "task.h"
#include <sstream>
#include "lsystem.h"
#include "dbcontrl.h"
#include "redis_client.h"
#include "../include/json/json.h"

class task_redis:public task{
    public:
        task_redis(const char *_task_name, const char *_run_path, const char *_conf_info, int _open_maxNum, int _open_spaceTime);
        ~task_redis();

    private:
        enum{ 
            SQL_READ_MAX_NUM = 5,                               //数据库最大读取次数
            SQL_READ_TIMEOUT = 1,                               //数据库读取周期
            REDIS_CONT_MAX_NUM = 5,                             //REDIS服务器尝试操作最大次数
            REDIS_CONT_TIMEOUT = 2                              //REDIS重复操作周期
        };

    private:
        int run_status_open();                                  //任务状态开启
};


#endif
