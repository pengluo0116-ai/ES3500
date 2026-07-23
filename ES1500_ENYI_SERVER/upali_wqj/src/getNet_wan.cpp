#include "getNet_wan.h"
using namespace std;
#define DEBUG 0
#define GETWAN_FUN_PATH "/root/cmd/getLan"  //脚本路径
#define WANDATA_PATH "/root/wandata"

/*
    编译
    mipsel-openwrt-linux-g++ -o ../obj/getNet_wan.o -c getNet_wan.cpp
    mipsel-openwrt-linux-g++ -o ../debug/getNet_wan ../obj/getNet_wan.o ../obj/lsystem.o -ljsoncpp
*/

/*
    函数名称：getNet_wan
    函数功能：构造函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-26
*/
getNet_wan::getNet_wan():ip(""), gateway(""), netmask(""), dns(""), proto(0), netIsCont(0){
    
}

/*
    函数名称：~getNet_wan
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-26
*/
getNet_wan::~getNet_wan(){
    
}

/*
    函数名称：cmd_getWanShell
    函数功能：运行本地shell脚本函数，获取wan口信息
    传入参数：无
    传出数据：无
    注意事项：0:运行正确 非0:运行失败
    编写人员：王凤龙
    编写时间：2016-04-26
*/
int getNet_wan::cmd_getWanShell(){
    string cmd = string(GETWAN_FUN_PATH) + " > " + string(WANDATA_PATH);
    if(lsystem(cmd.c_str())){
        #if DEBUG
        cout<<"shell脚本运行错误"<<endl;
        #endif
        return -1;
    }
    return 0x00;
}

/*
    函数名称：numToMask
    函数功能：根据数字返回子网掩码
    传入参数：
            string _num 数值
            string &_netmask 用于装载返回的子网掩码
    传出数据：无
    编写人员：王凤龙
    编写日期：2016-04-26
*/
static void numToMask(string _num, string &_netmask){
    /*校验num网关是否合法*/
    int maskNum = 0x00;
    if(
        _num.size() == 0 || _num=="" || 
        sscanf((const char *)(_num.c_str()),"%d", &maskNum) < 1 ||
        (maskNum/8) >= 4
    ){ _netmask=""; return;}
    
    int i = 0x00;
    int _size = maskNum/8;
    _netmask = "";
    for(i=0x00; i<_size; i++){
        _netmask += "255.";
    }
    switch(maskNum % 8){
        case 0: _netmask += "0"; break;
        case 1: _netmask += "80"; break;
        case 2: _netmask += "192"; break;
        case 3: _netmask += "224"; break;
        case 4: _netmask += "240"; break;
        case 5: _netmask += "248"; break;
        case 6: _netmask += "252"; break;
        case 7: _netmask += "254"; break;
    }
    i++;
    switch(4-i){
        case 1: _netmask += ".0"; break;
        case 2: _netmask += ".0.0"; break;
        case 3: _netmask += ".0.0.0";break;
    }
}

/*
    函数名称：cmd_getWanDataFromFile
    函数功能：从制定文本中读取生成的wan口数据
    传入参数：无
    传出数据：无
    注意事项：0:运行正确 非0:运行失败
    编写人员：王凤龙
    编写时间：2016-04-26
*/
int getNet_wan::cmd_getWanDataFromFile(){
    ifstream in;
    string getStr = "";
    string tmp = "";
    
    /*读取文件*/
    in.open((const char *)WANDATA_PATH);
    if(!in){ 
        #if DEBUG
        cout<<"打开文件失败"<<endl; 
        #endif
        return -1;
    }
    while(getline(in, tmp)){ getStr += tmp;}
    in.close();
    
    /*解析数据*/
    Json::Reader reader;
    Json::Value val;
    if(!reader.parse(getStr, val)) { 
        #if DEBUG
        cout<<"数据格式有误"<<endl; 
        #endif
        return -2;
    }
    
    /*校验网线是否连接*/
    if(val["ip"].asString() == "" || val["gateway"].asString() == "" ||  val["netmask"].asString() == ""){
        #if DEBUG
        cout<<"网线未连接"<<endl;
        #endif
        this->netIsCont = 1;
    }else{ this->netIsCont = 0;}
    
    /*获取ip，网关，子网掩码*/
    this->ip = val["ip"].asString();
    this->gateway = val["gateway"].asString();
    numToMask(val["netmask"].asString(), this->netmask);
    this->dns = val["dns"].asString();
    
    /*解析连接模式dhcp还是static*/
    if(val["proto"].asString() == ""){
        #if DEBUG
        cout<<"未获取到数据"<<endl;
        #endif
        return 1;
    }
    if(val["proto"].asString() == "dhcp") this->proto = 0;
    else this->proto = 1;
    
    return 0x00;
}

/*
    函数名称：getNet_wan_data
    函数功能：获取wan口的数据所有数据
    传入参数：无
    传出数据：0:获取成功，-1:运行错误 1:获取失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-26
*/
int getNet_wan::getNet_wan_data(){
    if(this->cmd_getWanShell()){ return -1;}
    if(this->cmd_getWanDataFromFile()){ return -2;}
    return 0x00;
}

#if 0
int main(){
    getNet_wan tmp;
    if(tmp.getNet_wan_data()) return -1;
    
    cout<<"ip:"<<tmp.ip<<" gateway:" << tmp.gateway <<" netmask:" << tmp.netmask << "proto:" << tmp.proto << " isCon:" << tmp.netIsCont<<endl;
    return 0x00;
}
#endif