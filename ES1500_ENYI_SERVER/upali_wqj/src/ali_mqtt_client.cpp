#include "ali_mqtt_client.h"
#include <iostream>
using namespace std;

#define DEBUG 1
#define LINKKIT_OTA_BUFFER_SIZE (512)
#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

typedef struct _sample_context {
    const void   *thing;
    int           cloud_connected;
    int           local_connected;
    int           thing_enabled;
} sample_context_t;

//added by hyy
static unsigned long long start_time_yield = 0;

/*
    函数名称:HAL_UptimeMs
    函数功能:获取当前时间
*/
static unsigned long long ENYI_UptimeMs_yield(void)
{
    struct timeval tv = { 0 };
    unsigned long long time_ms;

    gettimeofday(&tv, NULL);

    time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    return time_ms;
}

/*
    函数名称:uptime_sec
    函数功能:用于计算上报时间间隔
*/
static unsigned long long enyi_uptime_sec_yield(void)
{
    if (start_time_yield == 0) {
        start_time_yield = ENYI_UptimeMs_yield();
    }

    return (ENYI_UptimeMs_yield() - start_time_yield) / 1000;
}

int disconnec_flag;//断开连接标识
int reconnected_flag;//重连标识
static unsigned long long start_time = 0;
static int aliupswitch = 1;
static int CYC_TIMEOUT = 20000;

static int setp_to_thing(sample_context_t* sample);
Json::Value jsonSendExeStr;

/*
    函数名称:HAL_UptimeMs
    函数功能:获取当前时间
*/
static unsigned long long ENYI_UptimeMs(void)
{
    struct timeval tv = { 0 };
    unsigned long long time_ms;

    gettimeofday(&tv, NULL);

    time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    return time_ms;
}

/*
    函数名称:uptime_sec
    函数功能:用于计算上报时间间隔
*/
static unsigned long long enyi_uptime_sec(void)
{
    if (start_time == 0) {
        start_time = ENYI_UptimeMs();
    }

    return (ENYI_UptimeMs() - start_time) / 1000;
}

#define LINKKIT_PRINTF(...)  \
    do {                                                     \
        printf("\e[0;32m%s@line%d\t:", __FUNCTION__, __LINE__);  \
        printf(__VA_ARGS__);                                 \
        printf("\e[0m");                                   \
    } while (0)

/* identifier of property/service/event, users should modify this macros according to your own product TSL. */
#define EVENT_PROPERTY_POST_IDENTIFIER         "post"
#define EVENT_ERROR_IDENTIFIER                 "Error"
#define EVENT_ERROR_OUTPUT_INFO_IDENTIFIER     "ErrorCode"

sample_context_t g_sample_context;

/*
 * please modify this string follow as product's TSL.
 */
static const char TSL_STRING[] =
 "{\"profile\":{\"productKey\":\"a1GiK3cLHs4\"},\"services\":[{\"outputData\":[],\"identifier\":\"set\",\"method\":\"thing.service.property.set\",\"required\":true,\"callType\":\"async\"},{\"identifier\":\"get\",\"inputData\":[\"P_network\",\"P_web\",\"P_camera\",\"P_collecter_post\",\"P_color\",\"P_device_name\",\"P_sensor_info\",\"P_alipost_info\",\"P_en_sensor\",\"P_en_exe\",\"P_osd\",\"P_led\",\"temp\",\"hum\",\"pm2p5\",\"pm10\",\"windsp\",\"winddir\",\"so2\",\"no2\",\"o3\",\"co\",\"lux\",\"ata\"],\"method\":\"thing.service.property.get\",\"required\":true,\"callType\":\"async\"},{\"outputData\":[],\"identifier\":\"getSensorConf\",\"inputData\":[],\"method\":\"thing.service.getSensorConf\",\"required\":false,\"callType\":\"async\"},{\"outputData\":[],\"identifier\":\"resetcollect\",\"inputData\":[],\"method\":\"thing.service.resetcollect\",\"required\":false,\"callType\":\"async\"},{\"outputData\":[],\"identifier\":\"reboot\",\"inputData\":[],\"method\":\"thing.service.reboot\",\"required\":false,\"callType\":\"async\"}],\"properties\":[{\"identifier\":\"P_network\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"},\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"P_web\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"},\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"P_camera\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"},\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"P_collecter_post\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"},\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"P_color\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"},\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"P_device_name\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"},\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"P_sensor_info\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"},\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"P_alipost_info\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"},\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"P_en_sensor\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"},\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"P_en_exe\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"},\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"P_osd\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"},\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"P_led\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"},\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"temp\",\"dataType\":{\"specs\":{\"min\":\"-50\",\"max\":\"150\",\"step\":\"0.1\"},\"type\":\"float\"},\"accessMode\":\"r\",\"required\":true},{\"identifier\":\"hum\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"100\",\"step\":\"0.1\"},\"type\":\"float\"},\"accessMode\":\"r\",\"required\":true},{\"identifier\":\"pm2p5\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"2000\",\"step\":\"0.1\"},\"type\":\"float\"},\"accessMode\":\"r\",\"required\":true},{\"identifier\":\"pm10\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"2000\",\"step\":\"0.1\"},\"type\":\"float\"},\"accessMode\":\"r\",\"required\":true},{\"identifier\":\"windsp\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"150\",\"step\":\"0.1\"},\"type\":\"float\"},\"accessMode\":\"r\",\"required\":true},{\"identifier\":\"winddir\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"},\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"so2\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"200000\",\"step\":\"0.1\"},\"type\":\"float\"},\"accessMode\":\"r\",\"required\":true},{\"identifier\":\"no2\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"200000\",\"step\":\"0.1\"},\"type\":\"float\"},\"accessMode\":\"r\",\"required\":false},{\"identifier\":\"o3\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"200000\",\"step\":\"0.1\"},\"type\":\"float\"},\"accessMode\":\"rw\",\"required\":false},{\"identifier\":\"co\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"200000\",\"step\":\"0.1\"},\"type\":\"float\"},\"accessMode\":\"r\",\"required\":true},{\"identifier\":\"lux\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"2000\",\"step\":\"0.1\"},\"type\":\"float\"},\"accessMode\":\"r\",\"required\":false},{\"identifier\":\"ata\",\"dataType\":{\"specs\":{\"min\":\"0\",\"max\":\"2000\",\"step\":\"0.1\"},\"type\":\"float\"},\"accessMode\":\"rw\",\"required\":false}],\"events\":[{\"identifier\":\"post\",\"method\":\"thing.event.property.post\",\"type\":\"info\",\"required\":true},{\"outputData\":[{\"identifier\":\"InfoValue\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"}}],\"identifier\":\"ExeInfo\",\"method\":\"thing.event.ExeInfo.post\",\"type\":\"info\",\"required\":false},{\"outputData\":[{\"identifier\":\"device_disconnect\",\"dataType\":{\"specs\":{\"length\":\"2048\"},\"type\":\"text\"}}],\"identifier\":\"event_fault\",\"method\":\"thing.event.event_fault.post\",\"type\":\"error\",\"required\":false}]}";


void post_property_cb(const void *thing_id, int response_id, int code, const char *response_message, void *ctx)
{
    EXAMPLE_TRACE("thing@%p: response arrived:\nid:%d\tcode:%d\tmessage:%s\n", thing_id, response_id, code,
                  response_message == NULL ? "NULL" : response_message);

    /* do user's post property callback process logical here. */

    /* ............................... */

    /* user's post property callback process logical complete */
}

static int trigger_event_errinfo(sample_context_t *sample,string info)
{
    char event_output_identifier[64];
    snprintf(event_output_identifier, sizeof(event_output_identifier), "%s.%s", "ExeInfo", "InfoValue");

    int errorCode = 0;
    linkkit_set_value(linkkit_method_set_event_output_value,
                      sample->thing,
                      event_output_identifier,
                      (void*)info.c_str(), NULL);

    return linkkit_trigger_event(sample->thing, "ExeInfo", post_property_cb);
}

/*
    函数名称：post_event_device_disconnec_alert
    函数功能：云端设备断开告警信息
    传入参数：无
    传出数据：0代表成功，非0代表失败
    编写人员：侯勇勇
    编写时间:
*/
static int post_event_device_disconnec_alert(sample_context_t* sample)
{
    char event_output_identifier[64];
    snprintf(event_output_identifier, sizeof(event_output_identifier), "%s.%s", "event_fault", "device_disconnect");
     
    ifstream in;
    string getStr = "";
    string tmp = "";

    /*读取配置文件*/
    in.open("/root/time.log");
    if(!in){
    cout<<"打开配置文件失败(读取):"<<endl;
    return -2;
    }
    while(getline(in, tmp)){ getStr += tmp;}
    in.close();

    char temp[256]={0};
    sprintf(temp,"云端设备断开连接的时间是:%s",getStr.c_str());

    linkkit_set_value(linkkit_method_set_event_output_value,
                      sample->thing,
                      event_output_identifier,
                      NULL,temp);
                     
    return linkkit_trigger_event(sample->thing,"event_fault", post_property_cb);
}


/*
    函数名称：device_disconnec
    函数功能：设备连接断开
    传入参数：无
    传出数据：0代表成功，非0代表失败
    编写人员：侯勇勇
    编写时间:
*/
void *device_disconnec(void *arg)
{
    pthread_detach(pthread_self());
   
    sample_context_t*sample = &g_sample_context;
    post_event_device_disconnec_alert(sample);
}

static int on_connect(void* ctx)
{
    sample_context_t* sample = (sample_context_t*)ctx;

    sample->cloud_connected = 1;

    EXAMPLE_TRACE("cloud is connected\n");

    return 0;
}

static int on_disconnect(void* ctx)
{
    sample_context_t* sample = (sample_context_t*)ctx;

    sample->cloud_connected = 0;

    EXAMPLE_TRACE("cloud is disconnect\n");

     /* do user's disconnect process logical here. */
    //将当前系统时间记录到文件中
    time_t now;
    struct tm *tm_now;
    time(&now);
    tm_now = localtime(&now);

    FILE *fp1 = NULL;
    fp1 = fopen("/root/time.log","w+");  /* 可读可写，不存在则创建 */
    char time_buffer[128]={0};
    sprintf(time_buffer,"%d年%d月%d日%d时%d分%d秒",tm_now->tm_year+1900,tm_now->tm_mon+1,tm_now->tm_mday,tm_now->tm_hour,
    tm_now->tm_min,tm_now->tm_sec);
   
    fwrite(time_buffer,1,strlen(time_buffer),fp1);
    fclose(fp1);
    
    cout<<"111on_disconnect222"<<endl;

    return 0;
}

static int raw_data_arrived(const void* thing_id, const void* data, int len, void* ctx)
{
    char raw_data[128] = {0};

    EXAMPLE_TRACE("raw data arrived,len:%d\n", len);

    /* do user's raw data process logical here. */

    /* ............................... */

    /* user's raw data process logical complete */

    snprintf(raw_data, sizeof(raw_data), "test down raw reply data %lld", (long long)HAL_UptimeMs());

    linkkit_invoke_raw_service(thing_id, 0, raw_data, strlen(raw_data));

    return 0;
}

/*
    函数名称：readFile_network
    函数功能：读取网络登录信息
    传入参数：Json::Value &jsonData
    传出数据：0运行成功 非0 运行失败
    注意事项：获取到的信息存储到network_info中，里面的数据以字符串的形式展现
    编写人员：王凤龙
    编写时间：
*/
static int readFile_network(Json::Value &jsonData){
    /*获取网口信息*/
    getNet_wan lanInfo;
    if(lanInfo.getNet_wan_data()) return 1;

    /*写入json*/
    Json::Value jsonLan;
    jsonLan["ip"] = lanInfo.ip;
    jsonLan["gateway"] = lanInfo.gateway;
    jsonLan["netmask"] = lanInfo.netmask;
    jsonLan["dns"] = lanInfo.dns;
    jsonData = jsonLan;

    return 0x00;
}

/**
 * 函数名称：read_process_confFile
 * 函数功能：读取进程配置文件
 * 传入参数：const char * filepath 配置文件路径
 *           operation * 操作名称
 *           property_identifier 属性值
 *           sample_context_t* sample 控制结构体
 * 返回值：0x00
 * 编写日期：2021-03-22 
 * 编写人：周广阔
*/
static int read_process_confFile(const char *filepath,const char *operation,const char *property_identifier,sample_context_t* sample)
{
    Json::Value jsonConf;jsonConf.clear();
    Json::FastWriter Jwriter;
    ostringstream ostr; ostr.str("");
    if(JreadConf(filepath,jsonConf)){
        ostr << "读取" << operation << "配置失败！！";
        trigger_event_errinfo(sample,ostr.str());
    }
    string tmp = Jwriter.write(jsonConf);
    int ret = linkkit_set_value(linkkit_method_set_property_value, sample->thing, property_identifier, NULL, tmp.c_str());
    if(ret){
        ostr.str(""); ostr << "设置属性" << property_identifier << "失败！！";
        trigger_event_errinfo(sample,ostr.str());
    }
    linkkit_post_property(sample->thing, property_identifier,post_property_cb);
}

/*
    函数名称：getGateway
    函数功能：获取网关配置信息
    传入参数：无
    传出参数：无
    注意事项：无
    编写人员: 周广阔
    编写时间：2016-04-14
*/
static int getGateway(const char *property_identifier,sample_context_t* sample)
{
    Json::Value jsonData;
    Json::FastWriter Jwriter;
    /*获取MAC地址*/
    unsigned char _mac[6] = {0x00};
    if(dev_getMac(_mac, 0x06)){ cout << "获取设备mac地址失败" << endl; return -2; };
    
    ostringstream mac; mac.str("");
    mac << setfill('0') << setw(2) << hex << (unsigned int)_mac[0] << ":"
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[1] << ":"
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[2] << ":"
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[3] << ":"
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[4] << ":"
        << setfill('0') << setw(2) << hex << (unsigned int)_mac[5];
    
    /*获取实时ip*/
    getNet_wan getWan;
    if(getWan.getNet_wan_data()){ cout << "获取数据失败" << endl; return -3;}
    jsonData["wip"] =getWan.ip;
    jsonData["wgateway"] = getWan.gateway;
    jsonData["wmask"] = getWan.netmask;
    jsonData["proto"] = getWan.proto;
    jsonData["netIsCont"] = getWan.netIsCont; 
    jsonData["wmac"] = mac.str();   //mac地址

    string tmp = Jwriter.write(jsonData);
    int ret = linkkit_set_value(linkkit_method_set_property_value, sample->thing, property_identifier, tmp.c_str(), NULL);
    if(ret){
        trigger_event_errinfo(sample,"设置属性P_network失败");
    }
    linkkit_post_property(sample->thing, property_identifier, post_property_cb);
    return 0x00;
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
static int getDNS(const char *property_identifier,sample_context_t* sample)
{
    /*判断文件是否存在*/
    if((access(DNS_CONF_PATH, F_OK))){ cout << "读取DNS配置信息失败-配置文件不存在" << endl; return -2; }

    /*读取配置文件*/
    ifstream pIn;
    pIn.open(DNS_CONF_PATH);
    if(!pIn){ cout << "读取DNS配置文件失败！！" << endl; return -1; }

    Json::Value jsonDNS_list;
    Json::FastWriter Jwriter;
    string tmpStr = "";
    string _ip_ = "";
    char *point = (char *)0x00;
    
    while(getline(pIn, tmpStr)){
        _ip_ = "";
        if(get_DNS_ip(tmpStr.c_str(), _ip_)) continue;

        jsonDNS_list.append(_ip_);
    }
    pIn.close();

    /*数据校验并且返回*/
    if(jsonDNS_list.size()<=0x00){
        cout << "获取dis错误" << endl;
        return -1;
    }

    string tmp = Jwriter.write(jsonDNS_list);
    int ret = linkkit_set_value(linkkit_method_set_property_value, sample->thing, property_identifier, tmp.c_str(), NULL);
    if(ret){
        trigger_event_errinfo(sample,"设置属性P_dns失败");
    }
    linkkit_post_property(sample->thing, property_identifier, post_property_cb);
    return 0x00;
}

static int setp_to_thing(sample_context_t* sample)
{
    /*读取采集器配置文件*/
    read_process_confFile(SENSOR_POST_CONF,"采集器","P_collect",sample);

    /*读取阿里配置文件*/
    read_process_confFile(ALIPOST_CONF,"阿里上报","P_upali",sample);

    /*获取网关数据*/
    getGateway("P_network",sample);

    /*获取DNS数据*/
    getDNS("P_dns",sample);

    return 0;
}

static int thing_create(const void* thing_id, void* ctx)
{
    sample_context_t* sample = (sample_context_t*)ctx;

    EXAMPLE_TRACE("new thing@%p created.\n", thing_id);

    sample->thing = thing_id;
    setp_to_thing(sample);
    return 0;
}

static int thing_enable(const void* thing_id, void* ctx)
{
    sample_context_t* sample = (sample_context_t*)ctx;

    sample->thing_enabled = 1;

    return 0;
}

static int thing_disable(const void* thing, void* ctx)
{
    sample_context_t* sample = (sample_context_t*)ctx;

    sample->thing_enabled = 0;

    return 0;
}

/*系统重启函数*/
static int sysReboot()
{
    lsystem(CMD_SYSREBOOT);
}

/*
    函数名称:thing_call_service
    函数功能:响应云端服务请求
    编写人员:王清杰
    编写时间:2018-4-24
*/
#ifdef RRPC_ENABLED
static int thing_call_service(void* thing_id, char* service, int request_id, int rrpc, void* ctx)
#else
static int thing_call_service(const void* thing_id, const char* service, int request_id, void* ctx)
#endif /* RRPC_ENABLED */
{
    sample_context_t* sample = (sample_context_t*)ctx;

    EXAMPLE_TRACE("service(%s) requested, id: thing@%p, request id:%d\n",service, thing_id, request_id);

    /*获取传感器校准值*/
    if (strcmp(service, "getSensorConf") == 0)
    {
        setp_to_thing(sample);
    }
    else if (strcmp(service,"resetcollect") == 0)
    {
        lsystem("if [ \"`pidof -x /root/collecter/collect`\" != \"\" ]; then kill -9 `pidof -x /root/collecter/collect`; fi");
        lsystem("if [ \"`pidof -x /root/upali/upali`\" != \"\" ]; then kill -9 `pidof -x /root/upali/upali`; fi");
    }
    else if (strcmp(service,"reboot") == 0)
    {
        sysReboot();
    }

    return 0;
}

/*
    判断某字段是否使能
    0未使能,1使能,其他值错误
*/
#if 1
static int hasfield(string& field,Json::Value& data)
{
    if((data["enable"].type() == Json::nullValue)&&(!data["enable"].isArray())){cout << "function hasfield params error!" << endl;return -1;}
    int len = data["enable"].size();
    for(int i = 0;i < len;i++)
    {
        if(field == data["enable"][i].asString()){return 1;}
    }
    return 0;
}
#endif

/*更新dns配置*/
static string updateDns(Json::Value &jsonData)
{
    string tmp = "";
    int len = jsonData.size();
    for(int i=0; i<len; i++){
        if(jsonData[i].isNull()){
            tmp = "传入数据有误";
            return tmp;
        }
    }
    /*提取数据*/
    ostringstream dnsStr; dnsStr.str("");
    for(int i=0x00; i<len; i++){
        dnsStr << "nameserver " << jsonData[i].asString().c_str() << "\r\n";
    }

    /*写入文件*/
    ofstream fOut(DNS_CONF_PATH);
    if(!fOut.is_open()){ 
        tmp = "开启配置文件失败！！";
        return tmp;    
    }
    try{ fOut << dnsStr.str().c_str(); } catch (...){ 
        tmp = "写入配置文件失败！！";
        return tmp;
    }
    fOut.close();

    tmp = "更新dns配置成功";
    return tmp; 
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

/*更新网络配置*/
static string updateGate(Json::Value &jsonData)
{
    string wip, wgateway, wmask, wmac;
    string isdhcp;
    string tmp = "";

    /*获取提交的网络信息*/
    try{
        wip = jsonData["wip"].asString();
        wgateway = jsonData["wgateway"].asString();
        wmask = jsonData["wmask"].asString();
        isdhcp = jsonData["dhcp"].asString();
        wmac = jsonData["wmac"].asString();
    }catch(...){ tmp = "发送数据有误";return tmp;}

    /*校验提交的网络信息*/
    net_cont _netcont(wip.c_str(), wgateway.c_str(), wmask.c_str(), wmac.c_str());
    if(_netcont.init()){ tmp = "上传网络数据有误";  return tmp; };
    switch(_netcont.check()){
        case 0: break;
        case 1: tmp = "主机IP和网关不在一个网段";   return tmp;
        case 2: tmp = "网关地址不合法";             return tmp;
        case 3: tmp = "本注意IP不合法";             return tmp;
        
        case -1: 
        case -2: tmp = "子网掩码有误";              return tmp;
        case -11: 
        case -12: tmp = "MAC地址格式有误";          return tmp;

        default: tmp = "上传数据有误";              return tmp; 
    }

    /*更新网络信息*/
    if(_wirteNetToDev(wip.c_str(), wgateway.c_str(), wmask.c_str(), wmac.c_str())){ tmp = "设置网络信息失败"; return tmp; }
    tmp = "配置成功";
    return tmp;
}

/*更新非阿里平台上报配置信息*/
static string update_collect_post(Json::Value &jsonData)
{
    string tmp = "";
    /*参数校验*/
    
    if((jsonData["post_type"].type() == Json::nullValue)||(!jsonData["post_type"].isInt()))
    {
        tmp = "post_type字段为空或者类型不为int!";
        return tmp;
    }
    if(jsonData["ip"].type() == Json::nullValue)
    {
        tmp = "字段tcp_ip为空!";
        return tmp;
    }
    if(jsonData["port"].type() == Json::nullValue)
    {
        tmp = "字段tcp_port为空!";
        return tmp;
    }
    if((jsonData["iswork"].type() == Json::nullValue)||(!jsonData["iswork"].isInt()))
    {
        tmp = "字段iswork为空或者类型不为int!";
        return tmp;
    }
    if((jsonData["display"].type() == Json::nullValue)||(!jsonData["display"].isInt()))
    {
        tmp = "字段iswork为空或者类型不为int!";
        return tmp;
    }
    if(JwriteConf(SENSOR_POST_CONF,jsonData))
    {
        cout << "写入失败"<< endl;
        tmp = "更新恩易平台上报配置文件失败[-1]!";
        return tmp;
    }
    /*从起上报进程*/
    lsystem("if [ \"`pidof -x /root/collecter/collect`\" != \"\" ]; then kill -9 `pidof -x /root/collecter/collect`; fi");
    cout << "成功" << endl;

    tmp = "更新恩易平台上报配置文件成功!";
    return tmp;
}

/*更新阿里平台上报的配置信息*/
static string update_alipost_info(Json::Value &jsonData)
{
    string tmp = "";
    /*配置信息校验*/
    if((jsonData["alitime"].type() == Json::nullValue)||(!jsonData["alitime"].isInt()))
    {
        tmp = "字段alitime为空或者类型不是int!";
        return tmp;
    }
    if((jsonData["aliupswitch"].type() == Json::nullValue)||(!jsonData["aliupswitch"].isInt()))
    {
        tmp = "字段aliupswitch为空或者类型不是int!";
        return tmp;
    }
    if((jsonData["device_name"].type() == Json::nullValue)||(!jsonData["device_name"].isString()))
    {
        tmp = "字段device_name为空或者类型不是string!";
        return tmp;
    }
    if((jsonData["device_secret"].type() == Json::nullValue)||(!jsonData["device_secret"].isString()))
    {
        tmp = "字段device_secret为空或者类型不是string!";
        return tmp;
    }
    if((jsonData["product_key"].type() == Json::nullValue)||(!jsonData["product_key"].isString()))
    {
        tmp = "字段product_key为空或者类型不是string!";
        return tmp;
    }
    if(JwriteConf(UPALI_POST_CONF, jsonData))
    {
        tmp = "更新阿里上报配置信息失败[-1]!";
        return tmp;
    }

    tmp = "更新阿里上报配置信息成功!";
    return tmp;
}

/*
 * the handler of property changed
 * alink method: thing.service.property.set
 */
static int thing_prop_changed(const void *thing_id, const char *property, void *ctx)
{
    char *value_str = NULL;
    char property_buf[64] = {0};
    int response_id = -1;

    sample_context_t* sample = (sample_context_t*)ctx;
    if((strncmp(property,"P_",2) == 0))
    {
        /*获取属性值*/
        int ret = linkkit_get_value(linkkit_method_get_property_value, thing_id, property, NULL, &value_str);
        if(ret !=  0)
        {
            cout << "thing_prop_changed:linkkit_get_value error!" << endl; 
            return -1;
        }
        
        /*将属性值转换成json*/
        EXAMPLE_TRACE("property(%s), str_value:%s\n", property, value_str);
        ostringstream ostr;ostr.str("");
        ostr << "\\" << "\"";
        string tmp = string_replace(string(value_str),ostr.str(),"\"");
        cout << "解析后的属性值为===============>:" << tmp << endl;
        Json::Reader JR;
        Json::Value jsonData;jsonData.clear();
        if(!JR.parse(tmp,jsonData))
        {
            tmp = "thing_prop_changed:parse error!";
            trigger_event_errinfo(sample,tmp);
            return -2;
        }
        ret = linkkit_set_value(linkkit_method_set_property_value, thing_id, property, NULL, tmp.c_str());
        if(ret !=  0){
            cout << "thing_prop_changed:linkkit_set_value error!" << endl; 
            return -1;
        }

        /*释放属性值空间*/
        if(value_str){free(value_str);value_str = NULL;}
        string retmp = "没有执行属性配置!";
        
        if(strcmp(property, "P_dns") == 0)
        {
            retmp = updateDns(jsonData);
            response_id = linkkit_post_property(thing_id, property, post_property_cb);
            EXAMPLE_TRACE("post property(%s) response id: %d\n", property, response_id);
            return 0;
        }
        else if(strcmp(property,"P_network") == 0)
        {
            retmp = updateGate(jsonData);
            response_id = linkkit_post_property(thing_id, property, post_property_cb);
            EXAMPLE_TRACE("post property(%s) response id: %d\n", property, response_id);
            return 0;
        }
        else if(strcmp(property,"P_collect") == 0)
        {
            retmp = update_collect_post(jsonData);
        }
        else if(strcmp(property,"P_upali") == 0)
        {
            retmp = update_alipost_info(jsonData);

            EXAMPLE_TRACE("post property(%s) response id: %d\n", property, response_id);

            lsystem("if [ \"`pidof -x /root/upali/upali`\" != \"\" ]; then kill -9 `pidof -x /root/upali/upali`; fi");

            lsystem("if [ \"`pidof -x /root/collecter/collect`\" != \"\" ]; then kill -9 `pidof -x /root/collecter/collect`; fi");

            /*上报属性配置执行结果*/
            if(trigger_event_errinfo(sample,retmp))
            {
                cout << "thing_prop_changed:trigger_event_errinfo error!" << endl;
            }
            response_id = linkkit_post_property(thing_id, property, post_property_cb);
            return 0;
        }

        /*上报属性配置执行结果*/
        if(trigger_event_errinfo(sample,retmp))
        {
            cout << "thing_prop_changed:trigger_event_errinfo error!" << endl;
        }

        /* post property
         * result is response_id; if response_id = -1, it is fail, else it is success.
         * response_id by be compare in post_property_cb.
         */
        response_id = linkkit_post_property(thing_id, property, post_property_cb);

        EXAMPLE_TRACE("post property(%s) response id: %d\n", property, response_id);
    }

    return 0;
}

/* there is some data transparent transmission by linkkit */
static int linkit_data_arrived(const void *thing_id, const void *params, int len, void *ctx)
{
    EXAMPLE_TRACE("thing@%p: masterdev_linkkit_data(%d byte): %s\n", thing_id, len, (const char *)params);

    /* do user's data arrived process logical here. */

    /* ............................... */

    /* user's data arrived process logical complete */
    return 0;
}

static linkkit_ops_t alinkops = {
    .on_connect           = on_connect,            /* connect handler */
    .on_disconnect        = on_disconnect,         /* disconnect handler */
    .raw_data_arrived     = raw_data_arrived,      /* receive raw data handler */
    .thing_create         = thing_create,          /* thing created handler */
    .thing_enable         = thing_enable,          /* thing enabled handler */
    .thing_disable        = thing_disable,         /* thing disabled handler */
    .thing_call_service   = thing_call_service,    /* self-defined service handler */
    .thing_prop_changed   = thing_prop_changed,    /* property set handler */
    .linkit_data_arrived  = linkit_data_arrived,   /* transparent transmission data handler */
};

static unsigned long long uptime_sec(void)
{
    static unsigned long long start_time = 0;

    if (start_time == 0) {
        start_time = HAL_UptimeMs();
    }

    return (HAL_UptimeMs() - start_time) / 1000;
}

int post_all_prop(sample_context_t *sample)
{
    /* demo for post all property */
    return linkkit_post_property(sample->thing, NULL, post_property_cb);
}


static int is_active(sample_context_t* sample)
{
    #if DEBUG
    cout<<"sample->cloud_connected======"<<sample->cloud_connected<<endl;
    #endif
    
    return sample->cloud_connected/* && sample->thing_enabled*/;
}

/*
    函数名称：Ali_Mqtt_Post
    函数功能：构造函数
    传入参数：_post_path推送的配置文件
    传出数据：无
    编写人员：王清杰
    编写时间：2017-11-11
*/
Ali_Mqtt_Post::Ali_Mqtt_Post(const char *_post_path)
{
    this->post_path = string(_post_path);
    this->product_key = "";
    this->device_name = "";
    this->device_secret = "";
    this->product_secret = "";
    this->enconf.clear();
    this->en_execonf.clear();
    this->up_time = 10;
    this->linkPt = -1;
    this->linkEv = -1;
    this->flag_r = 0;
    this->jsonSendExeData = "";
    this->ExeStateStr = "";
}

/*
    函数名称：~Ali_Mqtt_Post
    函数功能：析构函数
    传入参数：无
    传出参数：无
    编写人员：王清杰
    编写时间：2017-11-11
*/
Ali_Mqtt_Post::~Ali_Mqtt_Post()
{
    
}

/*
    函数名称：_loadConf
    函数功能：加载配置信息
    传入参数：无
    传出数据：
                 0  运行成功
                -1  配置文件路径为空
                -2  读取配置文件失败
    注意事项：无
    编写人员：王清杰
    编写时间：2017-11-11
*/
int Ali_Mqtt_Post::_loadConf()
{
    if(this->post_path.size() <=0 ) return -0x01;
    Json::Value JsonConf;
    if(JreadConf(this->post_path.c_str(),JsonConf)) return -0x02;

    if(JsonConf["product_key"].isNull() || !JsonConf["product_key"].isString() || JsonConf["product_key"].asString().size() <=0)
        return -0x03;
    if(JsonConf["device_name"].isNull() || !JsonConf["device_name"].isString() || JsonConf["device_name"].asString().size() <=0)
        return -0x04;
    if(JsonConf["device_secret"].isNull() || !JsonConf["device_secret"].isString() || JsonConf["device_secret"].asString().size() <=0)
        return -0x05;

    this->product_key = JsonConf["product_key"].asString();
    this->device_name = JsonConf["device_name"].asString();
    this->device_secret = JsonConf["device_secret"].asString();
    this->up_time = JsonConf["alitime"].asInt();
    aliupswitch = JsonConf["aliupswitch"].asInt();

    // if(JreadConf(EN_FIELD_PATH,this->enconf)) return -0x03;

    return 0x00;
}


/*
    函数名称：deal_with_com_config
    函数功能：串口配置文件进行处理
    传入参数：无
    传出数据：0代表成功，非0代表失败
    编写人员：侯勇勇
    编写时间：2019-07-08
*/
int Ali_Mqtt_Post::deal_with_com_config()
{
    Json::Value JsonConfig;
    Json::Value jsonNew;
    ostringstream oid;

    Json::Value jsonInt;
    jsonInt["a"] = "";
    if(JwriteConf(EN_FIELD_PATH,jsonInt)){return -0x03;}

    for(int q = 1;q<5;q++)
    {
        oid.str("");
        oid << "/root/collecter/uart"<<q<<".conf";
        
       JsonConfig.clear();
       jsonNew.clear();

        if(JreadConf(oid.str().c_str(),JsonConfig)) return -0x02;

        int all_len = JsonConfig["alladdr"].size();
    
        //1.循环设备
        for(int i = 0;i<all_len;i++)
        {
            int addr_len = JsonConfig["alladdr"][i]["addr_info"].size();
            //2.循环设备有几个传感器
            for(int k=0;k<addr_len;k++)
            {
                jsonNew.clear();
                if(JreadConf(EN_FIELD_PATH,jsonNew)) return -0x02;
                //3.循环配置文件
                int field_len = jsonNew["enable"].size();
            
                jsonNew["enable"][field_len] =  JsonConfig["alladdr"][i]["addr_info"][k]["field_name"];
                
                if(JwriteConf(EN_FIELD_PATH,jsonNew)){return -0x03;}
            }
        }
    }
    return 0;
}

/*
    函数名称：init
    函数功能：初始化函数
    传入参数：无
    传出数据：0代表成功，非0代表失败
    编写人员：王清杰
    编写时间：2018-04-23
*/
int Ali_Mqtt_Post::init()
{   
    /*加载配置文件中的设备信息*/
    if(int err = this->_loadConf())
    {
        cout << "错误码" << err << endl;
        return -0x01;
    } 

    HAL_SetProductKey((char*)this->product_key.c_str());
    HAL_SetDeviceName((char*)this->device_name.c_str());
    HAL_SetDeviceSecret((char*)this->device_secret.c_str());
    
    /*创建阿里上报用的共性内存*/
    if(!this->shm_ali)
    {
        this->shm_ali = new shm_cont("shm_ali",4096);
        if(this->shm_ali->shm_create())
        {   
            cout << "创建阿里上报用进程失败!" << endl;
            return -1;
        }
    }

    /*开辟线程执行linkkit_start用于接收云端请求和上报数据*/
    if(0 != pthread_create(&this->linkPt,NULL,Ali_Mqtt_Post::link_yun_thread,(void*)this))
    {
        cout << "创建link_yun_thread失败!" << endl;
        return -0x02;
    }
    cout << "开辟线程成功" << endl;

    return  0x00;
}

void linkkit_cota_callback(service_cota_callback_type_t callback_type, const char *configid, uint32_t configsize, const char *gettype, const char *sign, const char *signmethod, const char *cota_url)
{
    char cota_buffer[LINKKIT_OTA_BUFFER_SIZE] = {0};

    EXAMPLE_TRACE("Cota Config ID: %s",configid);
    EXAMPLE_TRACE("Cota Config Size: %d",configsize);
    EXAMPLE_TRACE("Cota Get Type: %s",gettype);
    EXAMPLE_TRACE("Cota Sign: %s",sign);
    EXAMPLE_TRACE("Cota Sign Method: %s",signmethod);
    EXAMPLE_TRACE("Cota URL: %s",cota_url);

    linkkit_invoke_cota_service(cota_buffer,LINKKIT_OTA_BUFFER_SIZE);

}

/*
    函数名称:link_yun_thread
    函数功能:用于接收云端请求
    传入参数:对象指针
    传出参数:void*
    编写人员:王清杰
    编写时间:2018-4-23
*/

void* Ali_Mqtt_Post::link_yun_thread(void* arg)
{
    pthread_detach(pthread_self());

    // IOT_OpenLog("linkkit");
    // IOT_SetLogLevel(IOT_LOG_DEBUG);

    sample_context_t* sample_ctx = &g_sample_context;
    unsigned long long now = 0;
    unsigned long long prev_sec = 0;
    int get_tsl_from_cloud = 1;                        /* the param of whether it is get tsl from cloud */
    EXAMPLE_TRACE("linkkit start\n");

    /*
     * linkkit start
     * max_buffered_msg = 16, set the handle msg max numbers.
     *     if it is enough memory, this number can be set bigger.
     * if get_tsl_from_cloud = 0, it will use the default tsl [TSL_STRING]; if get_tsl_from_cloud =1, it will get tsl from cloud.
     */
    if (-1 == linkkit_start(22, get_tsl_from_cloud, linkkit_loglevel_debug, &alinkops, linkkit_cloud_domain_shanghai,sample_ctx)) {
        EXAMPLE_TRACE("linkkit start fail\n");
        return (void*)-1;
    }

    if (!get_tsl_from_cloud) {
        /*
         * if get_tsl_from_cloud = 0, set default tsl [TSL_STRING]
         * please modify TSL_STRING by the TSL's defined.
         */
        linkkit_set_tsl(TSL_STRING, strlen(TSL_STRING));
    }

    linkkit_cota_init(linkkit_cota_callback);
    // linkkit_fota_init(linkkit_fota_callback);
    EXAMPLE_TRACE("linkkit enter loop\n");

    while (1) {
        /*
         * if linkkit is support Multi-thread, the linkkit_dispatch and linkkit_yield with callback by linkkit,
         * else it need user to call these function to received data.
         */
        linkkit_dispatch();

        now = uptime_sec();
        if (prev_sec == now) {
            linkkit_yield(100);
            continue;
        }
        prev_sec = now;
    }
    
    linkkit_end();
}

/*
    函数名称：_create_send_data
    函数功能：创建发送数据
    传入参数：无
    传出数据：待发送的数据
    编写人员：周广阔
    编写时间: 2021-03-22
*/
void Ali_Mqtt_Post::_create_send_data()
{
    sample_context_t* sample_ctx = &g_sample_context;
    /*电梯状态*/
    string tmp = "";
    tmp = this->JWriter.write(jsonSendData);
    if(tmp == ""){
        linkkit_set_value(linkkit_method_set_property_value, sample_ctx->thing, "R_Data", NULL, "11");
    }else{
        linkkit_set_value(linkkit_method_set_property_value, sample_ctx->thing, "R_Data", NULL, (const char*)tmp.c_str());
    }

    return;
}

/*
    函数名称：sendData
    函数功能：发送数据
    传入参数：无
    传出数据：0代表成功，非0代表失败
    编写人员：王清杰
    编写时间：2017-11-11
*/
int Ali_Mqtt_Post::sendData()
{
    /*判断重连标志netflag是否存在,如果存在,代表网络断开重连的情况*/
    if(access("/root/netflag", F_OK) == 0x00)
    { 
        lsystem("rm -rf /root/netflag");
        sample_context_t* sample_ctr = &g_sample_context;
    
        sample_ctr->cloud_connected = 1;

        pthread_t linkid;
        if(0 != pthread_create(&linkid,NULL,device_disconnec,NULL))
        {
            cout << "创建device_disconnec失败!" << endl;
            return -0x02;
        }
        cout<<"on_connect is execute!!!"<<endl;
    }

    if((aliupswitch) && is_active(&g_sample_context))
    {
        this->_create_send_data();
        post_all_prop(&g_sample_context);
    }
    return 0;
}

int Ali_Mqtt_Post::recvData()
{
    return 0;
}

/*  
    函数名称:closeThread
    函数功能:关闭线程使其释放系统资源
    传入参数:无
    传出数据:无
    编写人员:王清杰
    编写时间:2018-3-12
*/
void Ali_Mqtt_Post::closeThread()
{
    if(linkPt != -1)
    {
        pthread_cancel(linkPt);
    }
    if(linkEv != -1){pthread_cancel(linkEv);}
}

/**
 * 函数名称：getSendData_from_AliVector
 * 函数功能：从共享内存获取上报数据
 * 编写时间：2021-03-22
 * 传入参数：无 返回值：0x00 成功
 * 编写人：周广阔
*/
int Ali_Mqtt_Post::getSendData_from_AliVector()
{
    if(this->shm_ali)
    {
        string str = "";
        this->shm_ali->readStr(str);
        if(str == "")
            return 0;
        Json::Reader jr;
        if(!jr.parse(str,this->jsonSendData))
        {
            cout << "解析数据为json对象失败!" << endl;
            return 0;
        }
        return 1;
    }
    return 0;
}

/*
    函数名称：things_post_two_hours
    函数功能：每隔两个小时上报一次物模型
    传入参数：无
    传出数据：0代表成功，非0代表失败
    编写人员：侯勇勇
    编写时间:
*/
void *things_post_two_hours(void *arg)
{
    pthread_detach(pthread_self());
    unsigned long long realnowyield = 0;
    sample_context_t*sample = &g_sample_context;
    for(;;)
    {
        sleep(200);
        realnowyield = enyi_uptime_sec_yield();
        if((realnowyield < 7200)||(realnowyield == 0)){usleep(20000);continue;}
        setp_to_thing(sample);
        start_time_yield = 0;
        usleep(20000);
    }
}

/*
    函数名称:work
    函数功能:上报数据到阿里平台
    传入参数:无
    传出数据:无
    编写人员:王清杰
    编写时间:2018-5-31
*/
 int Ali_Mqtt_Post::work()
 {
      /*初始化*/
    if(this->init()) {
        cout << "初始化失败" << endl;
        return -0x01;
    }

    unsigned long long realnow = 0;

    //每隔两个小时上报一次物模型
    pthread_t linkidyield;
    if(0 != pthread_create(&linkidyield,NULL,things_post_two_hours,NULL)){
        cout << "创建things_post_two_hours失败!" << endl;
        return -0x02;
    }

    /*工作*/
    for(;;){
        /*获取数据*/
        switch(this->getSendData_from_AliVector()){
            case 0: break;
            case 1: break;
            default: return -0x03;
        }
        realnow = enyi_uptime_sec();
        if((realnow % this->up_time != 0)||(realnow == 0)){usleep(20000);continue;}
        
        if(this->sendData()){ break;}
        if(this->recvData()){ break;}
        
        start_time = 0;
        usleep(20000);
    }
    this->closeThread();
    return 0x03;
 }
