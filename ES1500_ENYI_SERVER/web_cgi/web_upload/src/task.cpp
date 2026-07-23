#include "task.h"
using namespace std;

#define GET_DATA_BUFF_LEN 1024              //获取终端传输数据缓冲区的大小
static char msg[GET_DATA_BUFF_LEN] = {0x00};

/*
    函数名称：getPostData
    函数功能：获取终端传递过来的数据，并且进行解析，处理
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-02-06
*/
void getPostData(){
    #if 0
    /*用户登录信息cookie的校验*/
    ckuser_cookie _user_ck;
    if(_user_ck.ckUser()){ err_msg("399", ""); return;}
    #endif

    int len = 0x00;
    char *lenStr = (char *)0x00;
    memset(msg, 0, GET_DATA_BUFF_LEN);
    
    /*获取post上传数据长度*/
    lenStr =  getenv("CONTENT_LENGTH");
    if(lenStr == (char *)0x00){ err_msg("上传数据为空"); return;}
    len = atoi(lenStr);
    if(len > (GET_DATA_BUFF_LEN-1)) { err_msg("上传数据错误"); return;} //上传数据量超限，返回错误信息
    
    /*获取上传数据*/
    fgets(msg, len+1, stdin);
    
    Json::Reader reader;
    Json::Value value;
    int display = 0x00;
    try{
        if(!reader.parse(string(msg), value)) throw("");
        display = value["display"].asInt();
        
        switch(display){
            /*分支操作，添加此处*/
            /*==========================================*/
            case 1: cmd_getAlarmConf(); break;
            case 2: cmd_setAlarmConf(value); break;
            /*==========================================*/
            default: err_msg("上传数据格式错误"); break;
        }
    }catch(...){
        err_msg("上传数据格式错误");
    }
}

/*
    函数名称：dev_point_clear
    函数功能：设备显示坐标清零
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-04
*/
static void dev_point_clear(){
    dbcontrl db((const char *)DBPATH);
    db.exec(
        "UPDATE dev_info SET x=0, y=0;",
        NULL, NULL
    );
}

/*
    函数名称：update_config
    函数功能：将上传地图信息写入配置文件
    传入参数：const char *img_path   图片文件路径
    传出数据：
                 0  运行成功
                -1  参数校验有误
                -2  写入配置文件失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-25
*/
static int update_config(const char *img_path){
    /*参数校验*/
    if(!img_path || strlen(img_path)<=0x00) return -0x01;

    Json::Value jsonData;
    jsonData["path"] = img_path;

    /*写入配置文件*/
    if(JwriteConf(MAP_CONF_PATH, jsonData)) return -0x02;

    return 0x00;
}

/*
    函数名称：get_uploadData
    函数功能：上传文件处理
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-04
*/
void get_uploadData(){
    const char *fileName_client = (char *)0x00;   //上次文件名称--客户端名称
    const char *file_size = (char *)0x00;         //长传文件大小
    const char *fileName_server = (char *)0x00;   //上传文件名称--服务器段

    /*获取文件信息*/
    fileName_client = getenv("FILE_CLIENT_FILENAME_USERFILE"); if(!fileName_client) fileName_client = "null";
    file_size = getenv("FILE_SIZE_USERFILE"); if(!file_size) file_size = "null";
    fileName_server = getenv("FILE_FILENAME_USERFILE"); if(!fileName_server) fileName_server = "null";

    /*将上传的文件复制到图片文件夹*/
    ostringstream ostr; ostr.str("");
    ostr<< "cp -rf " << fileName_server << " " << MAP_IMAGE_DIR << "/" << "map.jpeg";
    lsystem(ostr.str().c_str());

    /*清空地图配置存放文件夹*/
    ostr.str("");
    ostr << "rm -rf " << MAP_IMGEG_CONF_DIR << "/*";
    lsystem(ostr.str().c_str());

    /*将上传文件复制到地图配置文件夹*/
    unsigned long img_name = (unsigned long)time(NULL);
    ostr.str("");
    ostr << "cp -rf " << fileName_server << " " << MAP_IMGEG_CONF_DIR << "/" << img_name << ".jpeg";
    lsystem(ostr.str().c_str());

    /*写入配置文件*/
    ostr.str("");
    ostr << img_name << ".jpeg";
    update_config(ostr.str().c_str());

    /*显示坐标清零*/
    dev_point_clear();

    /*刷新QT页面*/
    lsystem("if [ \"`pidof -x /root/qt_show/qt_real_show`\" != \"\" ]; then kill -9 `pidof -x /root/qt_show/qt_real_show`; fi");    

    /*刷新当前页面*/
    cout<<"Content-type:text/html\r\n\r\n";
    // cout<< "<center>上传成功，请关闭页面<center>";
    cout<< "<center>上传成功，请关闭页面</center><script>window.parent.location.href=window.parent.location.href;</script>"<<endl;
}