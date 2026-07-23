#ifndef __RADIO_UPDATE_H__
#define __RADIO_UPDATE_H__
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>
#include <unistd.h>

#include<sys/types.h>
#include <dirent.h>

#include "conf.h"
#include "c_md5.h"
#include "lsystem.h"
#include "dbcontrl.h"
#include "chttpclient.h"
#include "radio_download.h"
#include "../include/curl/curl.h"
#include "../include/json/json.h"


class radio_update{
    private:
        Json::Value jsonLift;                   //JSON电梯列表
        Json::Value jsonGroup;                  //JSON组列表
        std::ostringstream tmp_ostr;            //格式化转换器

        std::string http_recv_body;             //HTTP会话--返回的报文体内容
        Json::Value jsonRecv;                   //HTTP会话--返回的报文内容JSON
        Json::Reader reader_json;               //JSON解析器
        CHttpClient *http_cont;                 //HTTP会话客户端 

    public:
        radio_update();
        ~radio_update();

    private:
        int _get_newLiftList();                                                                         //获取最新的电梯列表
        int _get_newGroupList();                                                                        //获取最新的组列表
        int _update();                                                                                  //添加百度语音文件--电梯名称
        int _clear();                                                                                   //删除百度语音文件--电梯名称
        int _group_update();                                                                            //添加百度语音文件--电梯小组
        int _group_clear();                                                                             //删除百度语音文件--电梯小组

    public:
        int init();
        int run();

    private:
        static int _get_newLiftList_sqlCallback(void *data, int argc, char **argv, char **azColName);   //获取最新的电梯列表--数据库操作回调函数
};

#endif