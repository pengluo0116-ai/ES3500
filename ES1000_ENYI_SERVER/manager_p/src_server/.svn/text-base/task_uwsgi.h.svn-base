#ifndef __TASK_UWSGI_H__
#define __TASK_UWSGI_H__
#include "task.h"
#include <sstream>
#include "lsystem.h"

class task_uwsgi:public task{
    public:
        task_uwsgi(const char *_task_name, const char *_run_path, const char *_conf_info, int _open_maxNum, int _open_spaceTime);
        ~task_uwsgi();

    private:
        void run_model();                                   //进程开始方式
        int run_status_close();                             //任务状态关闭
};


#endif