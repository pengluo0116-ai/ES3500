#include "main.h"
using namespace std;

int main(){
    int err = radio_download::init();
    if(err){ 
        cout<< "下载器初始化失败:" << err <<endl;
    }

    radio_update tts_real;
    if(err = tts_real.init()){
        cout<< "实时更新任务失败:"<< err <<endl;
        return 0x01;
    }

    tts_real.run();

    return 0x00;
}