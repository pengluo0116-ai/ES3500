#ifndef __TASK_WEB_H__
#define __TASK_WEB_H__
#include "task.h"
#include "lsystem.h"

class task_web:public task{
    private:
        int open_num;

    public:
        task_web();
        ~task_web();

    private:
        int init();                                         //初始化
        void run_model();                                   //进程开始方式
        int run_status_close();                             //任务状态关闭
};
#endif
