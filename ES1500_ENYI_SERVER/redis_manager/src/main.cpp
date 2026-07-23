#include "main.h"
using namespace std;


int main(){
    Manager manager;
    
    if(int err = manager.init()){ cout<< "缓存管理器初始化失败" <<endl; exit(-1); }
    manager.run();
}