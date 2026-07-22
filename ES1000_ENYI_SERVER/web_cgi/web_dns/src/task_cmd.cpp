#include "task_cmd.h"
using namespace std;


static int get_system_config_dns(string& dns_str) {

    ifstream infile; 
    infile.open(DNS_FILE_PATH, ios::in); 
    infile >> dns_str; 
    infile >> dns_str; 
    infile.close();
    return 0;
}


void cmd_get_alarm_dns() {
    Json::Value jsonData;

    string dns;
    /*读取配置信息*/
    int err = get_system_config_dns(dns);
    if(err == 0) {
        jsonData["error"] = 0;
        jsonData["conf"] = dns;
    }
    else {
        jsonData["error"] = 404;
        jsonData["data"] = "获取数据失败，请稍候重试";
    }
    
    // /*将配置信息返回*/
    cout<<"Content-type:text/html\r\n\r\n";
    cout<< jsonData.toStyledString() << endl;
}