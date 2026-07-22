#ifndef __POST_HTTP_H__
#define __POST_HTTP_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <unistd.h>

#include "post_ports.h"
#include "chttpclient.h"

class post_http:public post_ports{
    private:
        std::string recv_dataBuff;      //从服务器接收到的数据
        CHttpClient *httpClient;
        std::string post_url;           //服务器URL

    public:
        post_http(const char *_ip, const int _port);
        ~post_http();

    private:
        int init();
        int sendData();
        int recvData();
};

#endif