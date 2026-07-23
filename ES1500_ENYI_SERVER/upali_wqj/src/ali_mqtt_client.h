#ifndef _ALI_MQTT_CLIENT_H_
#define _ALI_MQTT_CLIENT_H_

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string>
#include <sstream>
#include <time.h>  
#include <sys/time.h> 
#include <vector>
#include <iomanip>

#include "../include/exports/linkkit_export.h"
#include "../include/iot_import.h"
#include "../include/iot_export.h"
#include "lsystem.h"
#include "conf.h"
#include "tcp_sock_pakage.h"
#include "tcp_sock_client.h"
#include "jsonConfFile.h"
#include "Event_vector.h"
#include "dbcontrl.h"
#include "getNet_wan.h"
#include "net_cont.h"
#include "str_replace.h"
#include "shm_cont.h"
#include "shm_cont.h"
#include "4Gmodule.h"
#include "getMac.h"

using namespace std;
#define OTA_BUFFER_SIZE                  (512+1)

class Ali_Mqtt_Post
{
    public:
        Ali_Mqtt_Post(const char *_post_path);
        ~Ali_Mqtt_Post();

    private:
        int _loadConf();
        int deal_with_com_config();
        void _create_send_data();
        int get_exeStr();//执行项数据判重

    public:
        int init();
        int sendData();
        int recvData();
        void closeThread();
        static void* link_yun_thread(void*);
        static void* up_event_thread(void*);
        int work();
        int getSendData_from_AliVector();

    private:
        pthread_t linkPt;
        pthread_t linkEv;

        Json::Value enconf;
        Json::Value en_execonf;
        Json::Value jsonSendData;                     //发送数据--JSON格式
        Json::FastWriter JWriter;   
        Json::Reader JReader;    

        int flag_r;
        int up_time;

        string post_path;
        string product_key;
        string device_name;
        string device_secret;
        string product_secret;
        string jsonSendExeData;                       //发送数据--JSON执行项数据格式
        string ExeStateStr;  
        ostringstream ostr;
        vector<string> State;
        vector<string> Alarm;
        shm_cont * shm_ali;
};

#endif