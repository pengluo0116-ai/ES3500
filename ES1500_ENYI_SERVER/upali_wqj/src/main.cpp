#include "main.h"
using namespace std;
#define DEBUG 1

int main(int argc, const char **argv)
{   
    /*参数校验*/
    if(!argv){
        #if DEBUG
        cout<< "阿里推送管理器：输入参数为空" <<endl;
        #endif
        exit(-0x01);
    }

    sleep(1);

    /*开启推送*/
    signal(SIGPIPE, SIG_IGN);
    for(;;){
        Ali_Mqtt_Post* task = new Ali_Mqtt_Post((const char*)argv[0]); 
        
        #if DEBUG
        cout << "......UPALI......我要建立task_work了..." << endl;
        #endif

        int err = (int)task->work();
        #if DEBUG
        cout << "......UPALI......task_work()！return: "<< err << endl;
        #endif

        delete task; task = (Ali_Mqtt_Post *)0x00;
        sleep(3);
    }
    while(1){sleep(100000);}
    return 0x00;
}
