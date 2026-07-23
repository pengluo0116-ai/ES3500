#include "task_cmd.h"
using namespace std;

/*
    函数名称：get_devCid
    函数功能：获取设备的硬件ID
    传入参数：string &_cid   将获取到的硬件ID存入引用指向的存储空间
    传出数据：
                 0  运行成功
                -1  文件不存在或打开文件失败
                -2  读取配置信息失败，或者配置文件中内容为空
    注意事项：内部函数
    编写人员：王凤龙
    编写时间：2017-03-09
*/
static int get_devCid(string &_cid){
    /*判断文件是否存在*/
    if((access(DEVID_FILE_PATH, F_OK)) != 0) return -0x01;

    /*打开文件*/
    ifstream fInput;
    fInput.open(DEVID_FILE_PATH);
    if(!fInput) return -0x01;

    /*读取文件*/
    _cid = "";
    string tmp = "";
    while(getline(fInput, tmp)){ _cid += tmp; }

    /*关闭文件*/
    fInput.close();

    if(_cid.size() <= 0x00) return -0x02;

    return 0x00;
}

/*
    函数名称：cmd_getQrcode_img
    函数功能：获取带参数的二维码图像数据
    传入参数：Json::Value &_jsonData  {"display":1, "name":"xxxx"}
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-09
*/
void cmd_getOrcode_img(Json::Value &_jsonData){
    /*参数校验*/
    if(_jsonData["name"].isNull() || _jsonData["name"].asString().size()<=0x00){ err_msg("上传数据有误"); return; }
    
    /*组装消息*/
    string cid = "";
    if(get_devCid(cid)){ err_msg("读取本地设备信息失败，请稍后再试，或者重新启动报警主机"); return; }
    
    _jsonData["display"] = "1";
    _jsonData["dev_snum"] = cid;

    /*发送信息*/
    Json::FastWriter fast_writer;
    string post_bodyStr = fast_writer.write(_jsonData);

    string recv_bodyStr = "";
    CHttpClient httpClient;
    if(int err = httpClient.Post(string(QRCODE_SERVER_ADDR), (const string)post_bodyStr, recv_bodyStr)){ 
        ostringstream ostr; ostr.str("");
        ostr<< "连接云服务器失败[" << err << "]";
        err_msg(ostr.str().c_str()); return; 
    }
    ofstream out;
    out.open("/root/wqj.txt");
    out << "recvbak: " << recv_bodyStr << endl;
    out.close();
    cout<<"Content-type:text/html\r\n\r\n";
    cout<< recv_bodyStr.c_str() << endl;
}

/*
    函数名称：get_getWchart_userAppId
    函数功能：获取用户的微信appID
    传入参数：Json::Value &_jsonData  {"display":2, "data":"qrcode.xxxx"}
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-03-09
*/
void cmd_getWchart_userAppId(Json::Value &_jsonData){
    /*参数校验*/
    if(_jsonData["data"].isNull() || _jsonData["data"].asString().size()<=0x00){ err_msg("上传数据有误"); return; }

    /*组装消息*/
    string cid = "";
    if(get_devCid(cid)){ err_msg("读取本地设备信息失败，请稍后再试，或者重新启动报警主机"); return; }

    _jsonData["display"] = "2";
    _jsonData["dev_snum"] = cid;

    /*发送信息*/
    Json::FastWriter fast_writer;
    string post_bodyStr = fast_writer.write(_jsonData);

    string recv_bodyStr = "";
    CHttpClient httpClient;
    if(int err = httpClient.Post(string(QRCODE_SERVER_ADDR), (const string)post_bodyStr, recv_bodyStr)){ 
        ostringstream ostr; ostr.str("");
        ostr<< "连接云服务器失败[" << err << "]";
        err_msg(ostr.str().c_str()); return; 
    }
    ofstream out;
    out.open("/root/jjj.txt");
    out << "getWchart_userAppId: " << recv_bodyStr << endl;
    out.close();
    cout<<"Content-type:text/html\r\n\r\n";
    cout<< recv_bodyStr.c_str() << endl;
}