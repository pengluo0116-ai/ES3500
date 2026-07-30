#include "task_cmd.h"
using namespace std;

/*
    函数名称：cmd_getAlarmConf
    函数功能：获取报警配置信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-25
*/
void cmd_getAlarmConf(){
    Json::Value jsonData;

    /*读取配置信息*/
    if(int err = JreadConf(ALARM_CONF_PATH, jsonData)){
        ostringstream ostr; ostr.str("");
        ostr << "获取配置信息失败，错误码[" << err << "]";
        err_msg(ostr.str().c_str()); return;
    }

    /*将配置信息返回*/
    jsonData["error"] = "0";
    cout<<"Content-type:text/html\r\n\r\n";
    cout<< jsonData.toStyledString() << endl;
}

/*
    函数名称：cmd_setAlarmConf
    函数功能：更新配置信息
    传入参数：Json::Value &jsonData
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-25
*/
void cmd_setAlarmConf(Json::Value &jsonData){
    /*参数校验*/

    /*写入文件*/
    if(int err = JwriteConf(ALARM_CONF_PATH, jsonData)){
        ostringstream ostr; ostr.str("");
        ostr << "更新配置信息失败，错误码[" << err << "]";
        err_msg(ostr.str().c_str()); return;
    }

    /*重启发送器*/
    lsystem("if [ \"`pidof -x /root/collecter/collect`\" != \"\" ]; then kill -9 `pidof -x /root/collecter/collect`; fi");

    err_msg("0", "");
}


/*
    函数名称：cmd_getPostConf
    函数功能：获取推送配置信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-25
*/
void cmd_getPostConf(){
    Json::Value jsonData;
    Json::Value jsonTmp;
    if(int err = JreadConf(ALIPOST_CONF_PATH, jsonTmp)){
        ostringstream ostr; ostr.str("");
        ostr << "获取阿里上报配置信息失败，错误码[" << err << "]";
        err_msg(ostr.str().c_str()); return;
    }
    jsonData["product_key"]     = jsonTmp["product_key"].asString();
    jsonData["device_name"]     = jsonTmp["device_name"].asString();
    jsonData["device_secret"]   = jsonTmp["device_secret"].asString();
    jsonData["aliupswitch"]     = jsonTmp["aliupswitch"].asInt();
    jsonData["alitime"]         = jsonTmp["alitime"].asInt();
    /*将配置信息返回*/

    jsonData["error"] = "0";
    cout<<"Content-type:text/html\r\n\r\n";
    cout<< jsonData.toStyledString() << endl;
}

/*
    函数名称:cmd_setAliPostConf
    函数功能:设置阿里数据上报的配置
    传入参数:配置信息
    传出数据:无
    编写人员:王清杰
    编写时间:2018-6-1
*/
void cmd_setAliPostConf(Json::Value &jsonData)
{
     Json::Value tmp;
     if(int err = JreadConf(ALIPOST_CONF_PATH,tmp))
     {
        ostringstream ostr; ostr.str("");
        ostr << "读取阿里上报配置失败，错误码[" << err << "]";
        err_msg(ostr.str().c_str()); return;
    }
    tmp["aliupswitch"]     = jsonData["aliupswitch"].asInt();
    tmp["alitime"]         = jsonData["alitime"].asInt();
    tmp["product_key"]     = jsonData["product_key"].asString();
    tmp["device_name"]     = jsonData["device_name"].asString();
    tmp["device_secret"]   = jsonData["device_secret"].asString();
     
    if(int err = JwriteConf(ALIPOST_CONF_PATH, tmp)){
        ostringstream ostr; ostr.str("");
        ostr << "阿里上报配置失败，错误码[" << err << "]";
        err_msg(ostr.str().c_str()); return;
    }

    /*重启发送器*/
    lsystem("if [ \"`pidof -x /root/upali/upali`\" != \"\" ]; then kill -9 `pidof -x /root/upali/upali`; fi");

    err_msg("0", "");
}

/*
    函数名称:cmd_getDigestConf
    函数功能:获取
    传入参数:配置信息
    传出数据:无
    编写人员：周广阔
    编写时间:2021-04-13
*/
void cmd_getDigestConf()
{
    Json::Value tmp;
    tmp["isDigest"] = 0;
    if(access("/usr/local/nginx/conf/digest_flag", F_OK) == 0){
        tmp["isDigest"] = 1;
    }
    tmp["error"] = "0";
    cout<<"Content-type:text/html\r\n\r\n";
    cout<< tmp.toStyledString() << endl;
}

/*
    函数名称:cmd_setDigestConf
    函数功能:获取
    传入参数:配置信息
    传出数据:无
    编写人员:周广阔
    编写时间:2021-04-13
*/
void cmd_setDigestConf(Json::Value &jsonData)
{
    int flag = jsonData["isDigest"].asInt();
    if(flag){
        if(access("/usr/local/nginx/conf/digest_flag", F_OK) != 0){
            lsystem("touch /usr/local/nginx/conf/digest_flag");
        }
        if(lsystem("rm /usr/local/nginx/conf/nginx.conf; cp /usr/local/nginx/conf/digest_ok.conf /usr/local/nginx/conf/nginx.conf;")){
            err_msg("1","执行失败！！");
            return ;
        }
    } else {
        if(access("/usr/local/nginx/conf/digest_flag", F_OK) == 0){
            lsystem("rm /usr/local/nginx/conf/digest_flag");
        }
        if(lsystem("rm /usr/local/nginx/conf/nginx.conf; cp /usr/local/nginx/conf/digest_no.conf /usr/local/nginx/conf/nginx.conf;")){
            err_msg("1","执行失败！！");
            return ;
        }
    }
    err_msg("0","");
}
