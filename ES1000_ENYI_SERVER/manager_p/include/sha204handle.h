#ifndef __SHA204HANDLE_H
#define __SHA204HANDLE_H


#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include "includes.h"
//宏定义
//#define uint8_t unsigend char
//#define uint16_t unisgned int

#define SHA204_KEY_SIZE 32
#define SHA203_ID_SIZE 9
#define SHA_BUFF_SIZE 88

/*设备初始化函数，错误列表*/
typedef enum{
    ERR_NONE = 0X00,                        //无错误
    ERR_IIC_COMM_FAIL = 0X01,               //连接iic设备失败
    ERR_IIC_ADDR_FAIL = 0X02,               //设置iic设备地址失败
    ERR_GETID_FAIL = 0X03,                  //获取id失败
}SHA204_ERR;


/*SHA204操作结构体*/
typedef struct{
    uint8_t mypasd[SHA204_KEY_SIZE];            //用户密钥
    
    uint8_t id[SHA203_ID_SIZE];                 //芯片ID
    uint8_t key[SHA204_KEY_SIZE];               //芯片KEY
    
    uint8_t clgData[SHA204_KEY_SIZE];           //挑战数据
    uint8_t devResault[SHA204_KEY_SIZE];        //设备计算结果
    uint8_t sha256Resault[SHA204_KEY_SIZE];     //256算法结果
    
}SHA204_ctrol;

uint8_t SHA204_getID(SHA204_ctrol *_dev);
void SHA204_createKey(SHA204_ctrol *_dev);
uint8_t SHA204_writeKey(SHA204_ctrol *_dev);
uint8_t SHA204_CLG(SHA204_ctrol *_dev);
uint8_t sha256_CLG(SHA204_ctrol *_dev);
SHA204_ERR SHA204_init(SHA204_ctrol *_dev);
void SHA204_setmyPasd(SHA204_ctrol *_dev, const uint8_t *_mypasd);
void SHA204_setKeyToStruct(SHA204_ctrol *_dev, const uint8_t *_key);
uint8_t SHA204_DATAcheck(SHA204_ctrol *_dev);
void SHA204_WORK_END(void);
#endif 
