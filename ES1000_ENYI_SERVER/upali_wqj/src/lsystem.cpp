#include "lsystem.h"

/*
    函数名称：lsystem
    函数功能：执行linux shell指令
    传入参数：const char *_cmd_ -- shell指令
    传出数据：
            0   运行成功
            -1  shell指令运行成功后的返回值不为0
            -2  系统无法调用shell环境
            -3  调用shell环境异常结束
    编写人员：王凤龙
    编写时间：2016-04-09
*/
int lsystem(const char *_cmd_){
    pid_t status;
    status = system(_cmd_);
    
    if (-1 == status) return -2;
    else{  
        if (WIFEXITED(status)){  
            if (0 == WEXITSTATUS(status)) return 0x00; 
            else return -1; 
        }  
        else return -3;  
    }   
}