#include "jsonConfFile.h"
using namespace std;

#define DEBUG 0

/*
    函数名称：JreadConf
    函数功能：读取配置文件
    传入参数：
                const char *conf_path   读取的配置文件路径
                Json::Value &val        提取到的json数据，存储到该变量中
    传出数据：
                 0  运行正确
                -1  配置文件路径为空
                -2  打开配置文件失败
                -3  解析成json数据失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-08-03
*/
int JreadConf(const char *conf_path, Json::Value &val){
    ifstream in;
    string getStr = "";
    string tmp = "";

    /*校验输入参数*/
    if(!conf_path){
        #if DEBUG
        cout<< "配置文件路径为空(读取)" <<endl;
        #endif
        return -1;
    }
    
    /*读取配置文件*/
    in.open(conf_path);
    if(!in){ 
        #if DEBUG
        cout<<"打开配置文件失败(读取):" << conf_path <<endl; 
        #endif
        return -2;
    }
    while(getline(in, tmp)){ getStr += tmp;}
    in.close();

    /*配置信息解析成json格式*/
    Json::Reader reader;
    if(!reader.parse(getStr, val)) { 
        #if DEBUG
        cout<<"配置信息格式错误(读取):" << conf_path <<endl; 
        #endif
        return -3;
    }

    return 0x00;
}

/*
    函数名称：JwriteConf
    函数功能：将json格式的配置信息写入指定文件中
    传入参数：
                const char *conf_path   读取的配置文件路径
                Json::Value &val        提取到的json数据，存储到该变量中
    传出数据：
                 0  运行成功
                -1  配置文件吸入为空
                -2  打开配置文件失败
                 1  写入失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-08-17
*/
int JwriteConf(const char *conf_path, Json::Value &val){
    /*校验输入参数*/
    if(!conf_path){
        #if DEBUG
        cout<< "配置文件路径为空(写入)" <<endl;
        #endif
        return -1;
    }

    /*打开配置文件*/
    ofstream out(conf_path);
    if(!out.is_open()){
        #if DEBUG
        cout<<"打开配置文件失败(写入):" << conf_path <<endl; 
        #endif
        return -2;
    }

    /*写入配置文件*/
    int err = 0x00;
    Json::FastWriter fast_writer;
    try{ out << fast_writer.write(val); }catch(...){ err = 1;}
    out.close();

    return err;
}