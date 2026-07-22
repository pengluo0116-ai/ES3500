#include "radio_update.h"
using namespace std;
#define DEBUG 1

/*
    函数名称：radio_update
    函数功能：构造函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-21
*/
radio_update::radio_update(): http_cont(NULL), http_recv_body(""){}

/*
    函数名称：~radio_update
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-21
*/
radio_update::~radio_update(){
    if(this->http_cont){ delete this->http_cont; this->http_cont = NULL; }
}

/*
    函数名称：init
    函数功能：初始化函数
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-21
*/
int radio_update::init(){
    this->http_cont = new CHttpClient();
    if(!this->http_cont) return -0x01;

    return 0x00;
}

/*
    函数名称：_get_newLiftList_sqlCallback
    函数功能：获取电梯实时列表--数据库操作回调函数
    传入参数：
                data 调用该函数的父函数传给该函数的值
                argc 一条查询记录中，列数
                argv 查询到结果以字符串列表的形式存到改变量中
                azColName 一条查询结果中，以字符串列表的形式，存储所有的列的名称
    传出数据：默认 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-21
*/
int radio_update::_get_newLiftList_sqlCallback(void *data, int argc, char **argv, char **azColName){
    Json::Value *jsonData = (Json::Value *)data;
    Json::Value jsonTmp; jsonTmp.clear();

    try{
        jsonTmp["name"] = argv[0] ? argv[0] : "";
    }catch(...){ return 0x00; }

    jsonData->append(jsonTmp);

    return 0x00;
}

/*
    函数名称：_get_newLiftList
    函数功能：读取数据库，获取最新的电梯列表
    传入参数：无
    传出数据：
                 0  运行成功
                 1  数据库操作失败
                -2  数据库打开失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-21
*/
int radio_update::_get_newLiftList(){
    this->jsonLift.clear();
    
    dbcontrl db(DBPATH);
    return db.exec(
        "SELECT dev_name FROM dev_info order by did desc",
        radio_update::_get_newLiftList_sqlCallback,
        (void *)&this->jsonLift
    );
    return 0x00;
}

/*
    函数名称：_get_newGroupList
    函数功能：读取数据库，获取最新的小组列表
    传入参数：无
    传出数据：
                 0  运行成功
                 1  数据库操作失败
                -2  数据库打开失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-02
*/
int radio_update::_get_newGroupList(){
    this->jsonGroup.clear();

    dbcontrl db(DBPATH);
    return db.exec(
        "SELECT name FROM group_info order by id desc",
        radio_update::_get_newLiftList_sqlCallback,
        (void *)&this->jsonGroup
    );
}

/*
    函数名称：_update
    函数功能：更新百度语音
    传入参数：无
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-21
*/
int radio_update::_update(){
    if(this->jsonLift.size() <= 0x00) return -1;

    /*更新百度语音*/
    string lift_name_md5 = "";
    for(int i=0x00; i<this->jsonLift.size(); i++){
        /*生成电梯名称语音文件存储路径*/
        lift_name_md5 = "";
        if(cmd5_str((const unsigned char *)this->jsonLift[i]["name"].asString().c_str(), this->jsonLift[i]["name"].asString().size(), lift_name_md5)) continue;

        this->tmp_ostr.str("");
        this->tmp_ostr<< HTTP_DOWNLOAD_PATH << "/" << lift_name_md5 << ".mp3";

        /*判断文件是否存在*/
        if(access(this->tmp_ostr.str().c_str(), F_OK) == 0) continue;

        /*获取下载链接*/
        this->http_recv_body = "";
        if(this->http_cont->Post(HTTP_BAIDU_TTS_URL, this->jsonLift[i].toStyledString(), this->http_recv_body)) {
            cout << "错误" << endl;
            continue;
        }
        cout << "发送信息内容"  << this->jsonLift[i].toStyledString().c_str() << endl;
        cout << "返回消息内容" << this->http_recv_body << endl;
    
        this->jsonRecv.clear();
        if(!this->reader_json.parse(this->http_recv_body, this->jsonRecv)) continue;
        if(this->jsonRecv["download"].isNull()) continue;

        /*下载语音文件*/
        #if DEBUG
        cout<< "下载文件：" << this->jsonLift[i]["name"].asString()<<endl;
        #endif
        radio_download::http_cont(this->jsonRecv["download"].asString().c_str(), this->tmp_ostr.str().c_str());
    }

    return 0x00;
}

/*
    函数名称：_clear
    函数功能：去除多余语音文件
    传入参数：无
    传出数据：
                 0  运行成功
                -1  打开目录失败
    注意事项：执行该函数时，需要确保获取的电梯列表为最新
    编写人员：王凤龙
    编写时间：2017-04-21
*/
int radio_update::_clear(){
    if(this->jsonLift.size() <= 0x00) return 0x00;

    /*获取电梯列表名称*/
    string lift_name_md5 = "";
    Json::Value jsonLiftList; jsonLiftList.clear();
    for(int i=0x00; i<this->jsonLift.size(); i++){
        lift_name_md5 = "";
        if(cmd5_str((unsigned char *)this->jsonLift[i]["name"].asString().c_str(), this->jsonLift[i]["name"].asString().size(), lift_name_md5)) continue;

        this->tmp_ostr.str("");
        this->tmp_ostr<< lift_name_md5 << ".mp3";
        jsonLiftList.append(this->tmp_ostr.str());
    }

    /*获取文件列表*/
    Json::Value jsonFileList;   jsonFileList.clear();
    DIR *dir = NULL;
    struct dirent *ptr = NULL;

    dir = opendir(HTTP_DOWNLOAD_PATH); if(!dir) return -0x01;
    while((ptr=readdir(dir)) != NULL){ jsonFileList.append(string(ptr->d_name)); }
    closedir(dir);

    /*检测多余的文件，并且删除*/
    for(int i=0x00; i<jsonFileList.size(); i++){
        int point = 0x00;
        
        /*判断文件是否多余*/
        for(point=0x00; point<jsonLiftList.size(); point++){ if(jsonLiftList[point].asString() == jsonFileList[i].asString()) break; }

        /*删除多余文件*/
        if(point == jsonLiftList.size()){
            /*判断是否为语音文件*/
            if(!strstr((const char *)jsonFileList[i].asString().c_str(), ".mp3")) continue;

            #if DEBUG
            cout<< "删除文件：" << jsonFileList[i].asString() << endl;
            #endif

            this->tmp_ostr.str("");
            this->tmp_ostr<< "rm -rf " << HTTP_DOWNLOAD_PATH << "/" << jsonFileList[i].asString().c_str();
            lsystem(this->tmp_ostr.str().c_str());
        }
    }

    return 0x00;
}

/*
    函数名称：_group_update
    函数功能：更新百度语音--组名称
    传入参数：无
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-02
*/
int radio_update::_group_update(){
    if(this->jsonGroup.size() <= 0x00) return 0x00;

    string group_name_md5 = "";
    for(int i=0x00; i<this->jsonGroup.size(); i++){
        /*生成语音文件存储路径*/
        group_name_md5 = "";
        if(cmd5_str((const unsigned char *)this->jsonGroup[i]["name"].asString().c_str(), this->jsonGroup[i]["name"].asString().size(), group_name_md5)) continue;

        this->tmp_ostr.str("");
        this->tmp_ostr<< HTTP_DOWNLOAD_GROUP << "/" << group_name_md5 << ".mp3";

        /*判断文件是否存在*/
        cout << tmp_ostr.str().c_str() << endl;
        if(access(this->tmp_ostr.str().c_str(), F_OK) == 0) continue;

        /*获取现在连接*/
        this->http_recv_body = "";
        if(this->http_cont->Post(HTTP_BAIDU_TTS_URL, this->jsonGroup[i].toStyledString(), this->http_recv_body)) continue;

        #if DEBUG
        cout<< "发送信息内容：" << this->jsonGroup[i].toStyledString() <<endl;
        cout<< "服务器返回信息：" << this->http_recv_body <<endl;
        #endif

        this->jsonRecv.clear();
        if(!this->reader_json.parse(this->http_recv_body, this->jsonRecv)) continue;
        if(this->jsonRecv["download"].isNull()) continue;

        /*下载语音文件*/
        #if DEBUG
        cout<< "下载文件：" << this->jsonGroup[i]["name"].asString() <<endl;
        cout << "ddddddddd" << this->jsonRecv["download"].asString().c_str() << endl;
        #endif

        radio_download::http_cont(this->jsonRecv["download"].asString().c_str(), this->tmp_ostr.str().c_str());
    }

    return 0x00;
}

/*
    函数名称：_group_clear
    函数功能：删除多余语音文件--组
    传入参数：无
    传出数据：
    注意事项：执行该函数时，需要确保获取到的组列表为最新
    编写人员：王凤龙
    编写时间：2017-06-02
*/
int radio_update::_group_clear(){
    if(this->jsonGroup.size() <= 0x00) return 0x00;

    /*获取组列表名称*/
    string group_name_md5 = "";
    Json::Value jsonGroupList; jsonGroupList.clear();
    for(int i=0x00; i<this->jsonGroup.size(); i++){
        group_name_md5 = "";
        if(cmd5_str((unsigned char *)this->jsonGroup[i]["name"].asString().c_str(), this->jsonGroup[i]["name"].asString().size(), group_name_md5)) continue;

        this->tmp_ostr.str("");
        this->tmp_ostr<< group_name_md5 << ".mp3";
        jsonGroupList.append(this->tmp_ostr.str());
    }

    /*获取文件列表*/
    Json::Value jsonFileList; jsonFileList.clear();
    DIR *dir = NULL;
    struct dirent *ptr = NULL;
    
    dir = opendir(HTTP_DOWNLOAD_GROUP); if(!dir) return -0x01;
    while((ptr=readdir(dir)) != NULL){ jsonFileList.append(string(ptr->d_name)); }
    closedir(dir);

    /*检测多余文件--删除*/
    for(int i=0x00; i<jsonFileList.size(); i++){
        int point = 0x00;

        /*判断文件是否多余*/
        for(point=0x00; point<jsonGroupList.size(); point++){ if(jsonGroupList[point].asString() == jsonFileList[i].asString()) break; }

        /*删除多余文件*/
        if(point == jsonGroupList.size()){
            if(!strstr((const char *)jsonFileList[i].asString().c_str(), ".mp3")) continue;

            #if DEBUG
            cout<< "删除文件[组]：" << jsonFileList[i].asString() <<endl;
            #endif

            this->tmp_ostr.str("");
            this->tmp_ostr<< "rm -rf " << HTTP_DOWNLOAD_GROUP << "/" << jsonFileList[i].asString().c_str();
            lsystem(this->tmp_ostr.str().c_str());
        }
    }

    return 0x00;
}

/*
    函数名称：run
    函数功能：任务执行函数
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-24
*/
int radio_update::run(){
    int err = 0x00;
    for(;;){
        /*设备音频*/
        if(err = this->_get_newLiftList()){ cout<< "获取数据库信息:" << err <<endl; sleep(10); continue; } 
        cout<< "获取数据库信息成功!!!" <<endl;
        if(err = this->_update()){ cout<< "更新语音：" << err <<endl; sleep(10); continue; } 
        cout<< "更新语音信息成功!!!" <<endl;
        if(err = this->_clear()){ cout<< "删除语音：" << err <<endl; sleep(10); continue; }
        cout << "设备音频更新完毕" << endl;

        /*组音频*/
        if(err = this->_get_newGroupList()){ cout<< "获取组数据库信息有误：" << err <<endl; sleep(10); continue; }
        if(err = this->_group_update()){ cout<< "更新组语音有误：" << err <<endl; sleep(10); continue; }
        if(err = this->_group_clear()){ cout<< "删除组语音有误：" << err <<endl; sleep(10); continue; }

        sleep(BAIDU_TTSUPDATE_TIME);
    }

    return 0x01;
}