#include "4Gmodule.h"
#define DEBUG 0
/*
    函数名称:4Gmodule
    函数功能:构造函数
    传入参数:无
    传出数据:无
    编写人员:王清杰
    编写时间:2018-3-1
*/
FGmodule::FGmodule()
{
    this->AtUart = NULL;
    this->dataBuff = NULL;
}

int FGmodule::init()
{
    if(access("/dev/ttyUSB2",F_OK) != 0)
    {
        return -1;
    }

    this->AtUart = new stc_cont("/dev/ttyUSB2",115200,'N',8,1);
    this->AtUart->stc_init();
    /*分配缓冲区内存*/
    if(!this->dataBuff){this->dataBuff = new unsigned char[1024];}
#if 0
    char ATCLOSEGPS[] = "AT$MYGPSPWR=0\r\n";
    int err = this->AtUart->writeData((unsigned char*)ATCLOSEGPS,sizeof(ATCLOSEGPS));
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "关闭GPS:写串口数据失败!" << endl;
        out.close();
        #endif
        //cout << "判断SIM卡是否存在:写串口数据失败,错误码: " << err << endl;
        return -1;
    }
    usleep(300000);
    memset(this->dataBuff,0,1024);
    err = this->AtUart->getData(this->dataBuff,1024);
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "关闭GPS:读取串口数据失败!" << endl;
        out.close();
        #endif
        return -2;
    }
    if(strstr((char*)this->dataBuff,"OK"))
    {
        cout << "关闭GPS成功!" << endl;
    }else{
        cout << "关闭GPS失败!" << endl;
    }
#if 1
    char ATCPIN[] = "AT$MYGPSPWR=1\r\n";
    err = this->AtUart->writeData((unsigned char*)ATCPIN,sizeof(ATCPIN));
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "获取GPS位置:写串口数据失败!" << endl;
        out.close();
        #endif
        //cout << "判断SIM卡是否存在:写串口数据失败,错误码: " << err << endl;
        return -1;
    }
    usleep(200000);
    memset(this->dataBuff,0,1024);
    err = this->AtUart->getData(this->dataBuff,1024);
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "获取GPS位置:读取串口数据失败!" << endl;
        out.close();
        #endif
        return -2;
    }
    cout << "开启GPS的结果:" << this->dataBuff << endl;
    if(strstr((char*)this->dataBuff,"OK"))
    {
        cout << "开启GPS成功!" << endl;
    }else{
        cout << "开启GPS失败!" << endl;
    }

    char ATGPS[] = "AT$MYGPSPOS=3\r\n";
    err = this->AtUart->writeData((unsigned char*)ATGPS,sizeof(ATGPS));
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "获取GPS位置:写串口数据失败!" << endl;
        out.close();
        #endif
        //cout << "判断SIM卡是否存在:写串口数据失败,错误码: " << err << endl;
        return -1;
    }
    usleep(300000);

#endif
#endif

    return 0;
}
/*
    函数名称:~4Gmodule
    函数功能:析构函数
    传入参数:无
    传出数据:无
    编写人员:王清杰
    编写时间:2018-3-1
*/
FGmodule::~FGmodule()
{
     if(this->AtUart){ delete this->AtUart; this->AtUart = (stc_cont *)0x00; }
     if(!this->dataBuff) return;
     delete this->dataBuff; 
     this->dataBuff = (unsigned char *)0x00;
}

/*获取4G模块的ICCID*/
int FGmodule::get_iccid()
{
    char ATCCID[] = "AT+ICCID\r\n";
    int err = this->AtUart->writeData((unsigned char*)ATCCID,sizeof(ATCCID));
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "获取CCID:写串口数据失败!" << endl;
        out.close();
        #endif
        //cout << "判断SIM卡是否存在:写串口数据失败,错误码: " << err << endl;
        return -1;
    }
    usleep(300000);
    memset(this->dataBuff,0,1024);
    err = this->AtUart->getData(this->dataBuff,1024);
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "获取CCID:读取串口数据失败!" << endl;
        out.close();
        #endif
        return -2;
    }

    char* pstr = strstr((char*)this->dataBuff,":");
    string s = string_replace(string(pstr+1)," ","");
    this->ccid = s.substr(0,20);

    Json::Value jsonTemp;
    jsonTemp["ccid"] = this->ccid.c_str();

    JwriteConf(MODULE_CCID,jsonTemp);

    return 0;
}

/*
    函数名称:4GmoduleExitOrNot
    函数功能:判断4G模块是不是插入
    传入参数:无
    传出数据:0存在,非0不存在
    编写人员:王清杰
    编写时间:2018-3-1
*/
int FGmodule::FGmoduleExitOrNot()
{
    if(this->AtUart)
    {
        int err = this->AtUart->stc_init();
        if(err != 0){ return -1;}
        char ATE0[] = "ATE0\r\n";
        err = this->AtUart->writeData((unsigned char*)ATE0,sizeof(ATE0));
        if(err < 0){return -2;}
        memset(this->dataBuff,0,1024);
        usleep(100000);
        err = this->AtUart->getData(this->dataBuff,1024);
        if(err <= 0){return -3;}
        //else{   return 0;}
        ofstream out;
        out.open("/root/jjj.txt");
        out << "FGmoduleExitOrNot: " << string((const char*)this->dataBuff) << endl;
        out.close();
        if(strstr((char*)this->dataBuff,"OK")){   return 0;}
        else{   return -5;}
    }
    return -4;
}

/*获取GPS位置*/
int FGmodule::get_gps_pos()
{
    memset(this->dataBuff,0,1024);
    int err = this->AtUart->getData(this->dataBuff,1024);
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "获取GPS位置:读取串口数据失败!" << endl;
        out.close();
        #endif
        return -2;
    }

    cout << "gps数据: " << this->dataBuff << endl;

    //GPRMC gprmc;
    //memset(&gprmc,0,sizeof(GPRMC));
    ///gps_analyse((char*)this->dataBuff,&gprmc);
    //print_gps(&gprmc);
    return 0;
}

/*
    函数名称:SIMExitOrNot
    函数功能:判断SIM卡是否存在
    传入参数:无
    传出数据:0存在,1不存在,其他值未能判断SIM卡是否插入
    编写人员:王清杰
    编写时间:2018-3-1
*/
int FGmodule::SIMExitOrNot()
{
    char ATCPIN[] = "AT+CPIN?\r\n";
    int err = this->AtUart->writeData((unsigned char*)ATCPIN,sizeof(ATCPIN));
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "判断SIM卡是否存在:写串口数据失败!" << endl;
        out.close();
        #endif
        //cout << "判断SIM卡是否存在:写串口数据失败,错误码: " << err << endl;
        return -1;
    }
    memset(this->dataBuff,0,1024);
    err = this->AtUart->getData(this->dataBuff,1024);
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "判断SIM卡是否存在:读取串口数据失败!" << endl;
        out.close();
        #endif
        return -2;
    }
    #if DEBUG
    //printf("判断SIM卡是否存在时读取的串口数据:%s\n",this->dataBuff);
    //string bakdata = string((const char*)this->dataBuff);
    //ofstream out;
    //out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
    //cout << "判断SIM卡是否存在时读取的串口数据: " << bakdata << endl;
    //out.close();
    #endif
    if(strstr((char*)this->dataBuff,"READY"))
    {
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "SIM卡已插入!" << endl;
        out.close();
        #endif
        return 0;
    }
    #if DEBUG
    ofstream out1;
    out1.open("/root/4GMod.log",ios::in|ios::out|ios::app);
    out1 << "SIM卡未插入!" << endl;
    out1.close(); 
    #endif
    return 1;
}


/*
    函数名称:getSIMType
    函数功能:获取SIM卡的类型
    传入参数:无
    传出数据:0移动,1联通,2电信
    编写人员:王清杰
    编写时间:2018-3-1
*/
int FGmodule::getSIMType()
{
    char ATCOPS[] = "AT+COPS?\r\n";
    int err = this->AtUart->writeData((unsigned char*)ATCOPS,sizeof(ATCOPS));
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "判断SIM卡类型:写串口数据失败!" << endl;
        out.close();
        #endif
        return -1;
    }
    memset(this->dataBuff,0,1024);
    err = this->AtUart->getData(this->dataBuff,1024);
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "判断SIM卡类型:读取串口数据失败!" << endl;
        out.close();
        #endif
        return -2;
    }
    //cout << "SIM Card Type:" << this->dataBuff << endl;
    #if DEBUG
    //string bakdata = string((const char*)this->dataBuff);
    //ofstream out;
    //out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
    //cout << "判断SIM卡类型时读取的串口数据: " << bakdata << endl;
    //out.close();
    #endif
    if(strstr((char*)this->dataBuff,"CHINA MOBILE") || strstr((char*)this->dataBuff,"46000"))
    {
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "移动卡!" << endl;
        out.close();
        #endif
        return 0;
    }

    if(strstr((char*)this->dataBuff,"CHINA UNICOM")||strstr((char*)this->dataBuff,"46001"))
    {
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "联通卡!" << endl;
        out.close();
        #endif
        return 1;
    }

    if(strstr((char*)this->dataBuff,"CHINA TELECOM")||strstr((char*)this->dataBuff,"46011"))
    {
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "电信卡!" << endl;
        out.close();
        #endif
        return 2;
    }

    return -1;
}

/*
    函数名称:getSignalVolume
    函数功能:获取网络信号强度
    传入参数:无
    传出数据:信号强度值
    编写人员:王清杰
    编写时间:2018-3-1
*/
int FGmodule::getSignalVolume()
{
    char ATCSQ[] = "AT+CSQ\r\n";
    int err = this->AtUart->writeData((unsigned char*)ATCSQ,sizeof(ATCSQ));
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "获取网络信号强度:写串口数据失败!" << endl;
        out.close();
        #endif
        return -1;
    }
    memset(this->dataBuff,0,1024);
    err = this->AtUart->getData(this->dataBuff,1024);
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "获取网络信号强度:读取串口数据失败!" << endl;
        out.close();
        #endif
        return -2;
    }
    
    string bakdata = string((const char*)this->dataBuff);
    int pos = bakdata.find(',');
    string value = bakdata.substr(pos-2,2);
    int iValue = atoi(value.c_str());
    #if DEBUG
    //ofstream out;
    //out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
    //cout << "获取网络信号强度返回值: " << bakdata << endl;
    //cout << "网络信号强度: " << iValue << endl;
    //out.close();
    #endif
    return iValue;
}


/*
    函数名称:getNetType
    函数功能:获取网络类型
    传入参数:无
    传出数据:0-4G, 1-3G, 2-2G
    编写人员:王清杰
    编写时间:2018-3-1
*/
int FGmodule::getNetType()
{
    //char ATPSRAT[] = "AT+PSRAT\r\n";
    char ATPSRAT[] = "AT$MYSYSINFO\r\n";
    int err = this->AtUart->writeData((unsigned char*)ATPSRAT,sizeof(ATPSRAT));
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "获取网络类型:写串口数据失败!" << endl;
        out.close();
        #endif
        return -1;
    }
    memset(this->dataBuff,0,1024);
    err = this->AtUart->getData(this->dataBuff,1024);
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "获取网络类型:读取串口数据失败!" << endl;
        out.close();
        #endif
        return -2;
    }

    //cout << "网络制式: " << this->dataBuff << endl;

    #if DEBUG
    //ofstream out;
    //out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
    //cout << "获取网络类型返回数据: " << string((const char*)this->dataBuff) << endl;
    //out.close(); 
    #endif

    if(strstr((char*)this->dataBuff,"4")){ return 0;}
    if(strstr((char*)this->dataBuff,"3")){return 1;}
    if(strstr((char*)this->dataBuff,"2")){return 2;}
    if(strstr((char*)this->dataBuff,"NONE")){return 3;}

    return -3;
}



FGmoduleL610::FGmoduleL610()
{
    this->AtUart = NULL;
    this->dataBuff = NULL;
}

int FGmoduleL610::init()
{
    this->AtUart = new stc_cont("/dev/ttyUSB5",115200,'N',8,1);
    /*分配缓冲区内存*/
    if(!this->dataBuff){this->dataBuff = new unsigned char[1024];}
    return 0;
}
/*
    函数名称:~4Gmodule
    函数功能:析构函数
    传入参数:无
    传出数据:无
    编写人员:王清杰
    编写时间:2018-3-1
*/
FGmoduleL610::~FGmoduleL610()
{
     if(this->AtUart){ delete this->AtUart; this->AtUart = (stc_cont *)0x00; }
     if(!this->dataBuff) return;
     delete this->dataBuff; 
     this->dataBuff = (unsigned char *)0x00;
}


/*获取4G模块的ICCID*/
int FGmoduleL610::get_iccid()
{
    char ATCCID[] = "AT+CCID\r\n";
    int err = this->AtUart->writeData((unsigned char*)ATCCID,sizeof(ATCCID));
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "获取CCID:写串口数据失败!" << endl;
        out.close();
        #endif
        //cout << "判断SIM卡是否存在:写串口数据失败,错误码: " << err << endl;
        return -1;
    }
    usleep(300000);
    memset(this->dataBuff,0,1024);
    err = this->AtUart->getData(this->dataBuff,1024);
    if(err < 0){
        #if DEBUG
        ofstream out;
        out.open("/root/4GMod.log",ios::in|ios::out|ios::app);
        out << "获取CCID:读取串口数据失败!" << endl;
        out.close();
        #endif
        return -2;
    }

    char* pstr = strstr((char*)this->dataBuff,":");
    string s = string_replace(string(pstr+1)," ","");
    this->ccid = s.substr(0,20);

    Json::Value jsonTemp;
    jsonTemp["ccid"] = this->ccid.c_str();

    JwriteConf(MODULE_CCID,jsonTemp);

    return 0;
}