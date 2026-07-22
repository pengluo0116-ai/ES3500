#ifndef _4G_MODULE_H_
#define _4G_MODULE_H_
#include <iostream>
#include "stc_cont.h"
//#include "gps.h"
#include "jsonConfFile.h"
#include "conf.h"
#include "str_replace.h"
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>
using namespace std;
class FGmodule
{
    public:
        FGmodule();
        ~FGmodule();
    private:
        stc_cont *AtUart;
        unsigned char *dataBuff;                            //接收数据临时缓冲区
       
    public:
        int FGmoduleExitOrNot();
        int SIMExitOrNot();
        int getSIMType();
        int getSignalVolume();
        int getNetType();
        int init();
        int get_gps_pos();
        int get_iccid();
        string ccid;                                        //4G模块的ICCID
};

class FGmoduleL610
{
    public:
        FGmoduleL610();
        ~FGmoduleL610();
    private:
        stc_cont *AtUart;
        unsigned char *dataBuff;                            //接收数据临时缓冲区

       
    public:
        int init();

        int get_iccid();
        string ccid;                                        //4G模块的ICCID
};

#endif