#ifndef __REDIS_CLIENT_H__
#define __REDIS_CLIENT_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>
#include <hiredis/hiredis.h>

class redis_client{
    private:
        std::string sip;            //服务器ip
        int port;                   //服务器端口
        std::string pasd;           //服务器登录密码

        redisContext *client;       //客户端指针
        redisReply *replay;         //命令返回结果

    private:
        int loginSetPasd();                                                                                          //登录服务器
    
    public:
        redis_client(const char *_sip, int _port, const char *_pasd);
        ~redis_client();

    int createClient();                                                                                         //创建客户端
    int closeClient();                                                                                          //关闭客户端
    int setStr(const char *_keys, const char *_data);                                                           //设置key值
    int getStr(const char *_keys, char *_dataBuff, int _len);                                                   //获取key值
    int getKeys(const char *_keys, int (*getKey_callBack)(void *, const char *, int), void *data_callBack);     //检索符合条件的key名称
    int delKey(const char *_keys);                                                                              //删除key键
};

#endif
