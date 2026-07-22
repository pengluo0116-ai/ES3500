#include "server.h"
using namespace std;

#define DEBUG 1

static unsigned char host_send_buff[SEND_BUFF_MAX_SIZE];
static unsigned char host_recv_buff[RECV_BUFF_MAX_SIZE];

static void host_searchnode_cont(Json::Value &jsonCmd);         //搜索指令的回复
static void host_setdevinfo_cont(Json::Value &jsonCmd);         //设置设备网络信息
static void host_reboot_cont();                                 //重启设备

/*
    函数名称：mac_check
    函数功能：将获取到的报文中的MAC与本设备的MAC进行比较
    传入参数：
                unsigned char *macBuff  报文中获取到的MAC
                int buffLen             存储MAC的buff的大小
    传出数据：
                 0  比对一致
                -1  输入参数有误
                -2  获取MAC失败，socket创建失败
                 1  获取本地MAC失败
                 2  对比不一致
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-09-14
*/
static int mac_check(unsigned char *macBuff, int buffLen){
    /*校验输入参数*/
    if(macBuff == (unsigned char *)0x00 || buffLen < 0x06){
        #if DEBUG
        cout<< "udpServer-对比MAC-输入参数有误" <<endl;
        #endif
        return -0x01;
    }

    /*获取本地MAC地址*/
    int err = 0x00;
    unsigned char _macBuff[6] = {0x00};
    if(err = dev_getMac(_macBuff, 0x06)){
        #if DEBUG
        cout<< "udpServer-对比MAC-获取MAC地址失败[错误码：" << err << "]" <<endl;
        #endif
        return err;
    }

    /*比对MAC*/
    for(int i=0x00; i<0x06; i++){ if(macBuff[i] != _macBuff[i]) return 2;}

    return 0x00;
}

/*
    函数名称：SERVER_RUN
    函数名称：搜寻主机工作入口
    传入参数：udp_sock_server *_server 
    传出数据：无
    注意事项：udp服务端必须开启，才能调用本函数
    编写人员：王凤龙
    编写时间：2016-05-19
*/
static udp_radio *_send_client_ = (udp_radio *)0x00;

void SERVER_RUN(udp_sock_server *_server){
    Json::Value jsonData;
    Json::Reader reader;

    /*创建广播*/
    _send_client_ = new udp_radio("255.255.255.255", REMENT_SERVER_PORT);
    if(!_send_client_) return;
    if(_send_client_->create_radio()){ delete _send_client_; return; }

    for(;;){
        
        /*清空接收器*/
        memset((void *)host_recv_buff, 0x00 , RECV_BUFF_MAX_SIZE);
        
        /*接收数据*/
        if(_server->getData(host_recv_buff, RECV_BUFF_MAX_SIZE) <= 0x00) break; 
        
        /*报文头检测*/
        unsigned short *_tmp_head_ =  (unsigned short *)host_recv_buff;
        if(*_tmp_head_ != 0xAA55){ usleep(100000); continue;}
        
        /*解析成json*/
        jsonData.clear();
        if(
            !reader.parse(string(((const char *)host_recv_buff) + 10), jsonData) ||
             jsonData["CMD"].isNull()
          ) { usleep(100000); continue;}

        cout<< jsonData.toStyledString() <<endl;
          
        /*将上位机的ip写入json*/
        string _ip_ = "";
        _server->getRemoteIP(_ip_);  jsonData["ip"] = _ip_;
        #if DEBUG
        cout<<"接收IP："<< _ip_ <<endl;        
        #endif
        
        /*处理工作-搜索处理*/
        if(jsonData["CMD"].asString() == string(" **Searchnode")){ host_searchnode_cont(jsonData); continue; }; //搜寻机器信息

        /*比对MAC*/
        unsigned char _getMac[6] = {0x00};
        memcpy((void *)_getMac, (void *)(host_recv_buff + 2), 6);
        if(mac_check(_getMac, 6)) continue;

        /*处理工作-单台处理*/
        if(jsonData["CMD"].asString() == string("SetDevInfo")){ host_setdevinfo_cont(jsonData); continue; }
        if(jsonData["CMD"].asString() == string(" ReStart")){ host_reboot_cont(); continue; }
    }
}

/*
    函数名称：_sendDevInfo_toHost
    函数名称：发送数据到指定主机
    传入参数：
    传出数据：0 运行成功 非0 运行失败
    注意事项：当广播设备信息失败时，发送到指定用户
    编写人员：王凤龙
    编写时间：2016-12-07
*/
static int _sendDevInfo_toHost(const char *_ip, int _port, const unsigned char *_data, unsigned int _len){
    udp_sock_client _client(_ip, _port);
    if(_client.create_client()) return -0x01;
    if(_client.sendData(_data, _len) <= 0x00) return -0x02;
    _client.close_client();
    return 0x00;
}

/*
    函数名称：host_searchnode_cont
    函数功能：对搜寻指令的回应处理
    传入参数：Json::Value &dataData 外部发送的指令包
    传出数据：无
    注意事项：此函数实现的功能是是将本主机的网络信息返回给发送方
    编写人员：王凤龙
    编写时间：2016-05-20
*/
static void host_searchnode_cont(Json::Value &jsonCmd){
    #if DEBUG
    cout<<"发送网络信息"<<endl;    
    #endif

    int err = 0x00;
    
    /*获取网络信息*/
    getNet_wan _wan_; 
    if(int err = _wan_.getNet_wan_data()){
        #if DEBUG
        cout<<"获取网络信息失败:"<< err <<endl;
        #endif

        exit(-1);
        return;
    }
    
    /*将网络信息生成JSON数据*/
    Json::Value _jsonData_;
    _jsonData_["CMD"] = string("ReSearch");
    
    Json::Value DevInfo;
    DevInfo["DevName"] = "";
    DevInfo["DevIP"] = _wan_.ip;
    DevInfo["DevMGW"] = _wan_.netmask;
    DevInfo["GATE"] = _wan_.gateway;
    DevInfo["DHCP"] = _wan_.proto ? string("0") : string("1");
    DevInfo["DevType"] = string(DEVICE_NAME);
    _jsonData_["DevInfo"] = DevInfo;

    cout<< "发送信息:" <<endl;
    cout<< _jsonData_.toStyledString() <<endl;
    
    /*组织发送信息*/
    memset((void *)host_send_buff, 0x00, SEND_BUFF_MAX_SIZE);
    *((unsigned short *)host_send_buff) = 0xAA55;                                                                               //报文头
    if(getMac("eth0", (unsigned char *)&host_send_buff[2])){ memset((void *)&host_send_buff[2], 0xEE, 6); }                     //MAC地址
    *((unsigned short *)&host_send_buff[8]) = htons(_jsonData_.toStyledString().size());                                        //报文长度
    memcpy((void *)&host_send_buff[10], (void *)_jsonData_.toStyledString().c_str(), _jsonData_.toStyledString().size());       //发送的数据
    
    /*发送信息*/
    err = _send_client_->sendData((const char *)host_send_buff, _jsonData_.toStyledString().size()+10);

    #if DEBUG
    cout<<"发送完成"<<endl;
    #endif

    /*发送失败,退出进程*/
    if(err){ delete _send_client_; exit(1); }
}

/*
    函数名称：host_setdevinfo_cont
    函数功能：设置设备的网络配置信息
    传入参数：Json::Value &jsonCmd
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-09-13
*/
static void host_setdevinfo_cont(Json::Value &jsonCmd){
    Json::Value jsonData;
    ostringstream _cmd;

    /*校验网络信息*/
    net_cont _netcont( jsonCmd["DevInfo"]["DevIP"].asString().c_str(), jsonCmd["DevInfo"]["GATE"].asString().c_str(), jsonCmd["DevInfo"]["DevMGW"].asString().c_str() );
    if(_netcont.init()) return;
    if(_netcont.check()) return;

    /*校验输入参数*/
    if(
        jsonCmd["DevInfo"].isNull() ||
        jsonCmd["DevInfo"]["DevIP"].isNull() ||                 //IP
        jsonCmd["DevInfo"]["DevMGW"].isNull() ||                //子网掩码
        jsonCmd["DevInfo"]["GATE"].isNull()                     //网关
    ){ return; }

    /*设置网络参数*/
    unsigned char _mac[6] = {0x00};
    if(dev_getMac(_mac, 0x06)) return;                          //获取MAC地址

    ofstream out(NETWORK_CONFPATH);                             //打开网络配置文件
    if(!out.is_open()) return;

    out << "auto lo\r\n"                                        //写入配置信息
        << "iface lo inet loopback\r\n\r\n"
        << "auto eth0\r\n"
        << "iface eth0 inet static\r\n"
        << "address " << jsonCmd["DevInfo"]["DevIP"].asString().c_str() << "\r\n"
        << "netmask " << jsonCmd["DevInfo"]["DevMGW"].asString().c_str() << "\r\n"
        << "gateway " << jsonCmd["DevInfo"]["GATE"].asString().c_str() << "\r\n"
        << "hwaddress "
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[0] << ":"
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[1] << ":"
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[2] << ":"
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[3] << ":"
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[4] << ":"
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[5] << "\r\n";
        
    out.close();                                                //关闭配置文件

    /*重启网络*/
    lsystem("ip addr flush dev eth0; ifdown eth0; ifup eth0");

    /*发送返回信息*/
    jsonData["Error"] = 0;
    jsonData["CMD"] = "SetDevInfo";
    jsonData["Data"] = "设备网络配置成功";

    DEV_INFO_SET_END:
    /*组织发送信息*/
    memset((void *)host_send_buff, 0x00, SEND_BUFF_MAX_SIZE);
    *((unsigned short *)host_send_buff) = 0xAA55;                                                                               //报文头
    if(getMac("eth0", (unsigned char *)&host_send_buff[2])){ memset((void *)&host_send_buff[2], 0xEE, 6); }                     //MAC地址
    *((unsigned short *)&host_send_buff[8]) = htons(jsonData.toStyledString().size());                                        //报文长度
    memcpy((void *)&host_send_buff[10], (void *)jsonData.toStyledString().c_str(), jsonData.toStyledString().size());       //发送的数据
    
    
    /*发送信息*/
    udp_sock_client _send_client_(jsonCmd["ip"].asString().c_str(), REMENT_SERVER_PORT);
    if(_send_client_.create_client()){
        #if DEBUG
        cout<<"创建客户端失败"<<endl;
        #endif
        return;
    }
    _send_client_.sendData((const unsigned char *)host_send_buff, jsonData.toStyledString().size()+10);
    _send_client_.close_client();
    
    #if DEBUG
    cout<<"发送完成"<<endl;
    #endif
}

/*
    函数名称：host_reboot_cont
    函数功能：重启设备
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-09-13
*/
#if 0
static void host_reboot_cont(){
    /*组长发送报文*/
    tcp_package_modle _pakage;
    dataToPakage("{\"cmd\":4}", _pakage);

    /*建立TCP连接*/
    tcp_sock_client _client("127.0.0.1", 9191);
    if(_client.connectToServer()) { return;}

    /*发送数据*/
    if(_client.sendData((unsigned char *)&_pakage, sizeof(tcp_package_modle)) <= 0x00) { return;}

    /*接收数据*/
    int reg = 0x00;
    if((reg =_client.getData((unsigned char *)&_pakage, sizeof(tcp_package_modle))) <= 0x00){ return;}

    /*关闭连接*/
    _client.tcpClose();

    /*解析数据*/
    Json::Value jsonData;
    if(pakageToJson((unsigned char *)&_pakage, reg, jsonData)){  return;}
    
    if(jsonData["error"].asInt()){  return; }

}
#else
static void host_reboot_cont(){
    lsystem(CMD_SYSREBOOT); return;
}
#endif