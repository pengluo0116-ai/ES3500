#include "main.h"
using namespace std;


#if 0
int main(int argv, const char **argc){
    if(argv < 0x02){ sleep(1); cout<<"采集器启动失败"<<endl; return -0x01; }

    /*信息容器初始化*/
    if(lift_vector::init()) return -0x01;

    /*推送工作*/
    pthread_t ptmp;
    pthread_create(&ptmp, NULL, post_work, (void *)argc[1]);   

    lift_task _task(argc[0], argc[1]);
    _task.RUN();

    sleep(3);
    return 0x00;
}
#endif

/*

*/

int main(int argv, const char **argc){
    work_enable_cont(argc[1]);
    
    /*信息容器初始化*/
    if(lift_vector::init()) return -0x01;

    /*推送工作*/
    pthread_t ptmp;
    pthread_create(&ptmp, NULL, post_work, (void *)argc[1]);  

    mq_collecter collecter(
        "tcp://118.190.20.24:1883",
        "ExampleClientPub2",
        "TerminalData/#"
    );

    collecter.RUN();

    return 0x00;
}