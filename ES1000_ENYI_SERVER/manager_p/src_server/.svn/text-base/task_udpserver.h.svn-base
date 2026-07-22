#ifndef __TASK_UDPSERVER_H__
#define __TASK_UDPSERVER_H__
#include "task.h"

class task_udpserver:public task{
    private:
        static int RunStatus;                                   //标志位 设置udpserver关闭还是开启 0 开启 1关闭
        static pthread_mutex_t lock;                            //锁
    public:
        task_udpserver(const char *_task_name, const char *_run_path, const char *_conf_info, int _open_maxNum, int _open_spaceTime);
        ~task_udpserver();

        static int setUdpserver_RunStatus(int _status_num);            //设置udpserver运行控制状态
        static int getUdpserver_RunStatus(int &_status_num);           //获取udpserver运行控制状态

    private:
        int init();
};

#endif