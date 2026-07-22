#ifndef __RADIO_DOWNLOAD_H__
#define __RADIO_DOWNLOAD_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <unistd.h>

#include "conf.h"
#include "lsystem.h"
#include "../include/curl/curl.h"
#include "../include/json/json.h"

class radio_download{
    private:
        static pthread_mutex_t lock;                    //锁
        static std::string recvStr_head;                //返回报文头信息
        static FILE *Frecv_BODY;                        //返回报文体存储文件指针

    private:
        static size_t _http_whead_callback(void* buffer, size_t size, size_t nmemb, void* argv);
        static size_t _http_wbody_callback(void* buffer, size_t size, size_t nmemb, void* argv);
        static int _http_send(const char *_url, const char *_down_filePath);
        static int _http_chcek();

    public:
        static int init();
        static int http_cont(const char *_url, const char *_down_filePath);
};

#endif