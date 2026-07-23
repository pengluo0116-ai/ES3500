#include "task_cmd.h"
using namespace std;

static int wirteConfig_resetCollecter();
static int send_speed_cmd_to_manager();
static int resetNTPClient();
/*
    函数名称：cmd_getConf
    函数功能：获取STM32外接口波特率等配置信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王清杰
    编写时间：2017-11-07
*/
void cmd_getConf(){
    //Json::Value jsonData;
    /*读取校时相关信息*/
    Json::Value jsonNtp;
    if(int err = JreadConf("/root/t_sync.conf",jsonNtp))
    {
        ostringstream ostr; ostr.str("");
        ostr << "获取校时相关信息失败，错误码[" << err << "]";
        err_msg(ostr.str().c_str()); return;
    }
    
    /*将配置信息返回*/
    jsonNtp["error"] = "0";
    cout<<"Content-type:text/html\r\n\r\n";
    cout<< jsonNtp.toStyledString() << endl;
}

/*
    函数名称:cmd_setTime
    函数功能:设置校时
    传入参数:Json::Value &jsonData
    传出数据:无
    编写人员:王清杰
    编写时间:2018-1-11
*/

// void cmd_setTime(Json::Value &jsonData)
// {
//     int type = jsonData["tm_type"].asInt();
//     if(type == 1)//NTP校时
//     {
//         Json::Value tmpData;
//         if(int err = JreadConf(NTP_CONF_PATH,tmpData))
//         {
//             ostringstream ostr; ostr.str("");
//             ostr << "读取NTP校时配置文件失败，错误码[" << err << "]";
//             err_msg(ostr.str().c_str()); return;
//         }
//         jsonData["sync_sys_flag"] = tmpData["sync_sys_flag"].asInt();
//         /*写入文件*/
//         if(int err = JwriteConf(NTP_CONF_PATH, jsonData)){
//             ostringstream ostr; ostr.str("");
//             ostr << "更新NTP校时配置文件失败，错误码[" << err << "]";
//             err_msg(ostr.str().c_str()); return;
//         }

//         //重启NTP客户端
//         if(int err = resetNTPClient()){  
//             ostringstream ostr; ostr.str("");
//             ostr << "重启NTP客户端失败，请稍后再试[" << err << "]";
//             err_msg(ostr.str().c_str()); return;
//         }
        
//         err_msg("0", "");
//     }else if(type == 0)
//     {
//         /*将校时类型写入配置文件*/
//         Json::Value tmpData;
//         if(int err = JreadConf(NTP_CONF_PATH,tmpData))
//         {
//             ostringstream ostr; ostr.str("");
//             ostr << "读取NTP校时配置文件失败，错误码[" << err << "]";
//             err_msg(ostr.str().c_str()); return;
//         }
//         tmpData["tm_type"] = 0;
//         tmpData["sync_sys_flag"] = jsonData["sync_sys_flag"].asInt();
//         /*写入文件*/
//         if(int err = JwriteConf(NTP_CONF_PATH, tmpData)){
//             ostringstream ostr; ostr.str("");
//             ostr << "更新NTP校时配置文件失败，错误码[" << err << "]";
//             err_msg(ostr.str().c_str()); return;
//         }

//         ostringstream cmd;cmd.str("");
//         cmd << "date -s " << "\"" << jsonData["hand_time"].asString() << "\"";
//         lsystem(cmd.str().c_str());

//         //重启NTP客户端
//         if(int err = resetNTPClient()){  
//             ostringstream ostr; ostr.str("");
//             ostr << "重启NTP客户端失败，请稍后再试[" << err << "]";
//             err_msg(ostr.str().c_str()); return;
//         }
//         err_msg("0", "");
//     }
// }

void cmd_setTime(Json::Value &jsonData)
{
    //int type = jsonData["tm_type"].asInt();
    /*写入文件*/
    if(int err = JwriteConf("/root/t_sync.conf", jsonData)){
        ostringstream ostr; ostr.str("");
        ostr << "更新校时配置文件失败，错误码[" << err << "]";
        err_msg(ostr.str().c_str()); return;
    }
    /*手工校时*/
    //if(type == 0)
    {
         ostringstream cmd;cmd.str("");
         cmd << "date -s " << "\"" << jsonData["hand_time"].asString() << "\"";
         lsystem(cmd.str().c_str());
    }
    err_msg("0", "");
}

/*
    函数名称:resetNTPClient
    函数功能:重启NTP客户端
    传入参数:无
    传出数据:0成功,非0失败
    编写人员:王清杰
    编写时间:2018-1-11
*/
#if 0
static int resetNTPClient()
{
     /*组长发送报文*/
    tcp_package_modle _pakage;
    dataToPakage("{\"cmd\":1, \"data\":\"NTPC\"}", _pakage);

    /*建立TCP连接*/
    tcp_sock_client _client("127.0.0.1", 9191);
    if(_client.connectToServer()) return -1;

    /*发送数据*/
    if(_client.sendData((unsigned char *)&_pakage, sizeof(tcp_package_modle)) <= 0x00) return -2;

    /*接收数据*/
    int reg = 0x00;
    if((reg =_client.getData((unsigned char *)&_pakage, sizeof(tcp_package_modle))) <= 0x00) return -3;

    /*解析数据*/
    Json::Value jsonData;
    if(pakageToJson((unsigned char *)&_pakage, reg, jsonData)) return -4;

    /*查看错误信息*/
    if(jsonData["error"].asInt()) return jsonData["error"].asInt();

    ostringstream cmd; cmd.str("");
    if(jsonData["pid"].asInt() <= 0x00) return 0x00;
    cmd<< "kill -9 " << jsonData["pid"].asInt();
    lsystem(cmd.str().c_str());

    return 0x00;
}

/*
    函数名称：wirteConfig_resetCollecter
    函数功能：重启采集器
    传入参数：无
    传出数据：
                 0  运行成功 
                -1  连接服务器失败
                -2  发送数据到服务器失败
                -3  从服务器接收数据失败
                其它错误关注tcp服务器指令处理函数的相应错误
    注意事项：内部函数
    编写人员：王凤龙
    编写时间：2016-09-01
*/
static int wirteConfig_resetCollecter(){
    /*组长发送报文*/
    tcp_package_modle _pakage;
    dataToPakage("{\"cmd\":1, \"data\":\"COLECTER\"}", _pakage);

    /*建立TCP连接*/
    tcp_sock_client _client("127.0.0.1", 9191);
    if(_client.connectToServer()) return -1;

    /*发送数据*/
    if(_client.sendData((unsigned char *)&_pakage, sizeof(tcp_package_modle)) <= 0x00) return -2;

    /*接收数据*/
    int reg = 0x00;
    if((reg =_client.getData((unsigned char *)&_pakage, sizeof(tcp_package_modle))) <= 0x00) return -3;

    /*解析数据*/
    Json::Value jsonData;
    if(pakageToJson((unsigned char *)&_pakage, reg, jsonData)) return -3;

    /*查看错误信息*/
    if(jsonData["error"].asInt()) return jsonData["error"].asInt();

    ostringstream cmd; cmd.str("");
    if(jsonData["pid"].asInt() <= 0x00) return 0x00;
    cmd<< "kill -9 " << jsonData["pid"].asInt();
    lsystem(cmd.str().c_str());

    return 0x00;
}
#endif
/*
    函数名称:cmd_getTime
    函数功能:获取系统的时间戳
    传入参数:无
    传出数据:无
    编写人员:王清杰
    编写时间:2018-1-11
*/
void cmd_getTime()
{
    Json::Value t_time;
    time_t t;
    time(&t);
    t_time["time"] = (int)t;
    // ofstream out;
    // out.open("/root/tm.txt");
    // out << t_time["time"].asInt() << endl;
    // out.close();
    t_time["error"] = "0";
    cout<<"Content-type:text/html\r\n\r\n";
    cout<< t_time.toStyledString() << endl;
}