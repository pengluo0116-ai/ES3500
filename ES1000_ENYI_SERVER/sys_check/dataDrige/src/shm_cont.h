#ifndef __SHM_CONT_H__
#define __SHM_CONT_H__

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "conf.h"
#include "../include/json/json.h"

/*
    创建共享变量，及相关操作
*/

#define SHM_CONT_LOCK_ENABLE  1

class shm_cont{
    private:
        int fd;                         //共享变量文件描述符
        std::string name;               //共享变量名称
        unsigned char *data;            //共享变量指针
        unsigned int shm_len;          //共享变量区域长度

    public:
        shm_cont(const char *_name, unsigned int _shm_len);                         //构造函数
        ~shm_cont();                                                                //析构函数

        int shm_create();                                                               //创建共享变量
        void shm_close();                                                               //关闭共享变量
        int writeData(const unsigned char *_data, unsigned int _data_len);                //写入数据
        int readData(unsigned char *_data_buff, unsigned int _read_len);            //读取数据
        int writeStr(const char *_dataStr);                                         //写入字符串
        int readStr(std::string &_data);                                            //读取字符串
};

#endif

