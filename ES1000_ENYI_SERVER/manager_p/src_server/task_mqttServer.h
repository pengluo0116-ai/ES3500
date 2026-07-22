#ifndef __TASK_MQTTSERVER_H__
#define __TASK_MQTTSERVER_H__

#include "task.h"

class task_mqttServer:public task{
    public:
        task_mqttServer(const char *_task_name, const char *_run_path, const char *_conf_info, int _open_maxNum, int _open_spaceTime);
        ~task_mqttServer();

    private:
        void run_model();
};

#endif