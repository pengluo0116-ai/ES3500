#include "main.h"
using namespace std;

int main(){

    /*任务进程管理器*/
    task_manager::init();
    task_manager::work();

    for(;;){ sleep(10000); }
    
    pthread_exit(NULL);
    return 0x00;
}
