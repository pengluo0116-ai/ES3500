#ifndef __TASK_H__
#define __TASK_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "conf.h"

class task{
    public:
        std::string task_name;          //进程名称

    private:
        pid_t task_pid;                 //任务进程号
        pthread_mutex_t task_pid_lock;  //进程锁

    protected:
        std::string run_path;           //子程序运行路径
        std::string conf_info;          //配置信息
        int open_maxNum;                //重开最大次数，一直开启不了的情况下
        int open_spaceTime;             //重开间隔时间    
    
    protected:
        virtual int init();                                         //初始化
        virtual void run_model();                                   //进程开始方式
        virtual int run_status_error();                             //开启线程出现错误
        virtual int run_status_open();                              //任务状态开启
        virtual int run_status_close();                             //任务状态关闭

    private:
        int set_task_pid(pid_t _pid);                               //设置进程号

    public:
        task(const char *_task_name);
        task(const char *_task_name, const char *_run_path, const char *_conf_info, int _open_maxNum, int _open_spaceTime);
        ~task();
        
        pid_t get_task_pid();                                       //获取进程号
        int run();                                                  //任务工作
        
};

#endif
