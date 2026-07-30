#include "task_cmd.h"
using namespace std;

/*
    函数名称：trim
    函数功能：去除首尾空格
    传入参数：string &s
    传出数据：string&
    注意事项：无
*/
static string trim(string s){  
    if (s.empty()) return s; 
    s.erase(0,s.find_first_not_of(" "));  
    s.erase(s.find_last_not_of(" ") + 1);  
    return s;  
}

/*
    函数名称：getGateway
    函数功能：获取网关配置信息
    传入参数：无
    传出参数：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-04-14
*/
void getGateway(){
    Json::Value jsonData;

    /*获取MAC地址*/
    unsigned char _mac[6] = {0x00};
    if(dev_getMac(_mac, 0x06)){ err_msg("获取设备mac地址失败"); return; };
    
    ostringstream mac; mac.str("");
    mac << setfill('0') << setw(2) << hex << (unsigned int)_mac[0] << ":"
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[1] << ":"
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[2] << ":"
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[3] << ":"
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[4] << ":"
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[5];
    
    /*获取实时ip*/
    getNet_wan getWan;
    if(getWan.getNet_wan_data()){ err_msg("获取数据失败"); return;}
    jsonData["wip"] =getWan.ip;
    jsonData["wgateway"] = getWan.gateway;
    jsonData["wmask"] = getWan.netmask;
    jsonData["proto"] = getWan.proto;
    jsonData["netIsCont"] = getWan.netIsCont;
    jsonData["wmac"] = mac.str();
    
    jsonData["error"] = 0;
    cout<<"Content-type:text/html\r\n\r\n";
    cout<<jsonData.toStyledString()<<endl;
}

/*
    函数名称：getHexVersion
    函数功能：获取系统固件版本号
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-09-02
*/
void getHexVersion(){
    /*获取版本信息*/
    Json::Value jsonData;
    if(JreadConf(SYSINFO_PATH, jsonData)){ err_msg("获取版本信息失败"); return; }

    jsonData["error"] = 0;
    cout<<"Content-type:text/html\r\n\r\n";
    cout<<jsonData.toStyledString()<<endl;
}

/*
    函数名称：_wirteNetToDev
    函数功能：将网络信息写入设备
    传入参数：
                const char *_ip         设备IP
                const char *_gateway    网关
                const char *_netmask    子网掩码
                const char *_mac        MAC地址
    传出数据：
                 0  运行成功
                -1  获取设备MAC地址失败
                -2  打开网络配置文件失败
    注意事项：调用该函数时，应该确保数据格式正确
    编写人员：王凤龙
    编写时间：2016-12-09
*/
static int _wirteNetToDev(const char *_ip, const char *_gateway, const char *_netmask, const char *_mac){
    /*参数校验*/

    /*打开网络配置文件*/
    ofstream out(NETWORK_CONFPATH);
    if(!out.is_open()) return -0x02;

    /*写入配置信息*/
    out << "auto lo\r\n"
        << "iface lo inet loopback\r\n\r\n"
        << "auto eth0\r\n"
        << "iface eth0 inet static\r\n"
        << "address " << _ip << "\r\n"
        << "netmask " << _netmask << "\r\n"
        << "gateway " << _gateway << "\r\n"
        << "hwaddress " << _mac << "\r\n";
        
    /*关闭网络配置文档*/
    out.close();

    return 0x00;
}

/*
    函数名称：cmd_setDevNet
    函数功能：设置网络信息
    传入参数：Json::Value &jsonData
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-12-09
*/
void cmd_setDevNet(Json::Value &jsonData){
    string wip, wgateway, wmask, wmac;
    string isdhcp;

    /*获取提交的网络信息*/
    try{
        wip = jsonData["wip"].asString();
        wgateway = jsonData["wgateway"].asString();
        wmask = jsonData["wmask"].asString();
        isdhcp = jsonData["dhcp"].asString();
        wmac = jsonData["wmac"].asString();
    }catch(...){ err_msg("发送数据有误"); }

    /*校验提交的网络信息*/
    net_cont _netcont(wip.c_str(), wgateway.c_str(), wmask.c_str(), wmac.c_str());
    if(_netcont.init()){ err_msg("上传网络数据有误"); return; };
    switch(_netcont.check()){
        case 0: break;
        case 1: err_msg("主机IP和网关不在一个网段"); return;
        case 2: err_msg("网关地址不合法"); return;
        case 3: err_msg("本注意IP不合法"); return;
        
        case -1: 
        case -2: err_msg("子网掩码有误"); return;
        case -11: 
        case -12: err_msg("MAC地址格式有误"); return;

        default: err_msg("上传数据有误"); return; 
    }

    /*更新网络信息*/
    if(_wirteNetToDev(wip.c_str(), wgateway.c_str(), wmask.c_str(), wmac.c_str())){ err_msg("设置网络信息失败"); return; }

    err_msg("0", "");
}

/*
    函数名称：get_DNS_ip
    函数功能：根据DNS中的每行数据，获取DNS的IP地址
    传入参数：
                const char *_lineStr    行数据
                string &_ip             提取到的IP地址
    传出数据：
                 0  运行成功
                -1  参数校验有误
                -2  定位的到指定字符串失败
                -3  提取IP地址失败，格式有误
    注意事项：内部函数
    编写人员：王凤龙
    编写时间：2017-03-10
*/
static int get_DNS_ip(const char *_lineStr, string &_ip){
    /*参数校验*/
    if(!_lineStr || strlen(_lineStr)<=0x00) return -0x01;

    const char *point = (const char *)0x00;
    unsigned int _IP_[4] = { 0x00 };

    /*定位到nameserver*/
    if(!(point = strstr(_lineStr, "nameserver "))) return -0x02;

    /*获取IP地址*/
    if(sscanf(point, "nameserver %d.%d.%d.%d", &_IP_[0], &_IP_[1], &_IP_[2], &_IP_[3]) != 0x04) return -0x03;

    /*生成IP字符串*/
    ostringstream ostr_ip; ostr_ip.str("");
    ostr_ip<< _IP_[0] << "." << _IP_[1] << "." << _IP_[2] << "." << _IP_[3];

    _ip = ostr_ip.str();

    return 0x00;
}

/*
    函数名称：getDNS
    函数功能：获取DNS数据
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-10
*/
void getDNS(){
    /*判断文件是否存在*/
    if((access(DNS_CONF_PATH, F_OK))){ err_msg("读取DNS配置信息失败-配置文件不存在"); return; }

    Json::Value jsonDNS_list;
    string tmpStr = "";
    string _ip_ = "";
    char *point = (char *)0x00;
    ifstream pIn;

    /*读取配置文件*/
    pIn.open(DNS_CONF_PATH);
    if(!pIn){ err_msg("读取DNS配置文件失败"); return; }

    while(getline(pIn, tmpStr)){
        _ip_ = "";
        if(get_DNS_ip(tmpStr.c_str(), _ip_)) continue;

        jsonDNS_list.append(_ip_);
    }

    pIn.close();

    /*数据校验并且返回*/
    if(jsonDNS_list.size()<=0x00){
        cout<<"Content-type:text/html\r\n\r\n";
        cout<<"{\"error\":0, \"data\":[]}"<<endl;
        return;
    }

    Json::Value jsonPost;
    jsonPost["error"] = 0;
    jsonPost["data"] = jsonDNS_list;
    
    cout<< "Content-type:text/html\r\n\r\n";
    cout<< jsonPost.toStyledString() <<endl;
}

/*
    函数名称：setDNS
    函数功能：设置设备的DNS
    传入参数：Json::Value &jsonData {"display":7, "data":["xxx", "xxx"]}
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-10
*/
void setDNS(Json::Value &jsonData){
    /*参数校验*/
    if(jsonData["data"].isNull()){ err_msg("上传数据有误"); return; }
    if(jsonData["data"].size() <= 0x0){ err_msg("上传数据为空"); return; }

    /*提取数据*/
    ostringstream dnsStr; dnsStr.str("");
    for(int i=0x00; i<jsonData["data"].size(); i++){
        dnsStr << "nameserver " << jsonData["data"][i].asString().c_str() << "\r\n";
    }

    /*写入文件*/
    ofstream fOut(DNS_CONF_PATH);
    if(!fOut.is_open()){ err_msg("DNS设置，开启配置文件失败"); return; }
    try{ fOut<< dnsStr.str().c_str(); }catch(...){ err_msg("DNS设置，写入配置文件失败"); }
    fOut.close();

    err_msg("0", "ok");
}

/*
    函数名称：sysReboot
    函数功能：重启设备
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-12-09
*/
void sysReboot(){
    if(lsystem(CMD_SYSREBOOT)){ err_msg("设备重启失败，请稍后再试"); return; }

    err_msg("0", "");
}

/*
    函数名称：cmd_setPostPswd()
    编写人：周广阔      编写时间: 2020-12-24
    函数功能：修改服务器的推送密码
    传入参数：Json::Value $_jsonData
    返回值：0 成功  非零 失败    
*/
void cmd_setPostPswd(Json::Value &_jsonData)
{
    ostringstream ostr;
    ostr << "python /usr/local/nginx-http-auth-digest-master/pasd.py /usr/local/nginx/conf/passwd.digest admin admin " \
        << _jsonData["newpasd2"].asString().c_str() << " " << _jsonData["oldpasd2"].asString().c_str();
    string cmd = ostr.str();

    int err = system(cmd.c_str());
    switch(err){
        case 0: break;
        case 256: err_msg("设置新密码失败，参数错误"); return;
        case 512: err_msg("设置新密码错误，旧密码不正确"); return;
        default: err_msg("设置密码错误"); return ;
    }
    
    _jsonData.clear();
    _jsonData["error"] = 0;
    Json::FastWriter fast_writer;
    cout<<"Content-type:text/html\r\n\r\n";
    cout<< fast_writer.write(_jsonData) <<endl;
}