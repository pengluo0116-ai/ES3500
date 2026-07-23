#include "task.h"
using namespace std;
#define DEBUG 1

/*
    函数名称：task
    函数功能：构造函数
    传入参数：const char *_task_name 进程的名称
    传出数据：无
    注意事项：无参数构造函数
    编写人员：王凤龙
    编写时间：2016-08-29
*/
task::task(const char *_task_name):task_name(_task_name), run_path(""), conf_info(""), open_maxNum(0), open_spaceTime(0), task_pid(0){ pthread_mutex_init(&this->task_pid_lock, NULL); }

/*
    函数名称：task
    函数功能：构造函数
    传入参数：
                const char *_task_name      进程的名称
                const char _run_path        进程路径
                const char *_conf_info      进程参数
                int _open_maxNum            最大开启次数
                int _open_spaceTime         开启间隔
    传出数据：无
    注意事项：
                最大开启次数(_open_maxNum)仅仅针对于开启进程失败的情况：
                如果进程开启成功，开启次数计数为0，如果开启失败，开启次数累加，当累加到最大开启次数时，系统将重启
                如果最大开启次数为0的话，可以任意次数的开启，知道开启进程为止

                开启间隔(_open_spaceTime)，开启进程退出后，进过间隔时间后重新开启；如果开启间隔时间设置为0mai，则间隔时间为1秒
    编写人员：王凤龙
    编写时间：2016-08-29
*/
task::task(const char *_task_name, const char *_run_path, const char *_conf_info, int _open_maxNum = 0, int _open_spaceTime = 0):
task_name(_task_name), run_path(_run_path), conf_info(_conf_info), open_maxNum(_open_maxNum), open_spaceTime(_open_spaceTime){ pthread_mutex_init(&this->task_pid_lock, NULL); }

/*
    函数名称：~task
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-08-29
*/
task::~task(){}

/*
    函数名称：run_model
    函数功能：进程开启模式
    传入参数：无
    传出数据：无
    注意事项：默认进程开启方式是 程序名称 参数1
    编写人员：王凤龙
    编写时间：2016-08-29
*/
void task::run_model(){
    execlp(this->run_path.c_str(), this->conf_info.c_str(), NULL);
}

/*
    函数名称：init
    函数功能：任务初始化函数
    传入参数：无
    传出数据：默认 0
    注意事项：接口函数，默认函数
    编写人员：王凤龙
    编写时间：2016-08-29
*/
int task::init(){ return 0x00; }

/*
    函数名称：run_status_open
    函数功能：任务运行成功函数
    传入参数：无
    传出数据：默认 0
    注意事项：接口函数，默认； 当任务开启后，运行此函数
    编写人员：王凤龙
    编写时间：2016-08-29
*/
int task::run_status_open(){ return 0x00; }

/*
    函数名称：run_status_close
    函数功能：任务结束函数
    传入参数：无
    传出数据：默认 0
    注意事项：接口函数，默认； 当任务结束后，运行此函数
    编写人员：王凤龙
    编写时间：2016-08-29
*/
int task::run_status_close(){ return 0x00; }

/*
    函数名称：get_task_pid
    函数功能：获取子进程号
    传入参数：无
    传出数据：pid_t 子进程号
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-08-29
*/
pid_t task::get_task_pid(){
    pid_t tmp_pid = 0x00;

    pthread_mutex_lock(&this->task_pid_lock);
    try{ tmp_pid = this->task_pid; }catch(...){ tmp_pid = 0x00;}
    pthread_mutex_unlock(&this->task_pid_lock);

    return tmp_pid;
}

/*
    函数名称：run_status_error
    函数功能：创建进程出错的情况处理方式
    传入参数：无
    传出数据：默认 0
    注意事项：此函数运行在catch中，函数体中最好添加try catch
    编写人员：王凤龙
    编写时间：2016-09-02
*/
int task::run_status_error(){ return 0x00; }

/*
    函数名称：set_task_pid
    函数功能：设置子进程号
    传入参数：pid_t _pid 子进程号
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-08-29
*/
int task::set_task_pid(pid_t _pid){
    int err = 0x00;

    pthread_mutex_lock(&this->task_pid_lock);
    try{ this->task_pid = _pid; }catch(...){ err = -0x01;}
    pthread_mutex_unlock(&this->task_pid_lock);

    return err;
}

/*
    函数名称：run
    函数功能：任务运行函数
    传入参数：无
    传出数据：默认 0
    注意事项：接口函数，默认
    编写人员：王凤龙
    编写时间：2016-08-29
*/
int task::run(){
    int _opNum = 0X00;      //开启计数
    this->open_maxNum = (this->open_maxNum > 0) ? this->open_maxNum : 0x00;
    this->open_spaceTime = (this->open_spaceTime > 0)? this->open_spaceTime : 0x01;

    for(;;){
        /*初始化*/
        if(this->init()){
            #if DEBUG
            cout<<"开启进程初始化失败"<<endl;
            #endif
            sleep(this->open_spaceTime); continue;
        }

        /*校验参数*/
        if(this->run_path == "" || this->run_path.size() <= 0x00){
            #if DEBUG
            cout<< "进程名称为空" <<endl;
            #endif
            return -2;
        }

        /*开启进程*/
        pid_t tmp_pid = -1;
        try{
            tmp_pid = fork();
            if(tmp_pid == 0x00){
                /*子进程开启成功*/
                this->run_model();

                /*如果子进程指向其它程序成功，不会执行此步*/
                exit(-1);
            }else if(tmp_pid < 0x00){
                /*父进程检测到子进程开启失败*/
                throw("");
            }
        }catch(...){
            if(!this->open_maxNum){ sleep(this->open_spaceTime); continue; }
            if(_opNum++ > this->open_maxNum) return 0x01; 
            run_status_error();
            sleep(this->open_spaceTime); continue;
        }
        _opNum = 0x00;

        /*子进程开启成功*/
        this->set_task_pid(tmp_pid);
        if(this->run_status_open()) return 0x02;

        /*等待子进程结束*/
        waitpid(tmp_pid, NULL, 0x00);

        /*子进程结束*/
        #if DEBUG
        cout<<"进程关闭:" << this->task_name.c_str() <<endl;
        #endif
        this->set_task_pid((pid_t)0x00);
        if(this->run_status_close()) return 0x03;

        sleep(this->open_spaceTime);
    }
}