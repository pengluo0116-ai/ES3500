#include "task_cmd.h"
using namespace std;


/*
函数名称：filelist_to_json
函数功能：生成display参数为1时返回的字符串
传入参数：
            const char *p_path        DB文件搜索路径
传出数据：
            生成的JSON字符串
注意事项：无
编写人员：柳恩军
编写时间：2017-02-15
*/
static string filelist_to_json(const char *p_path) {
    Json::Value root;
    vector<string> file_db;

    file_db.clear();

    if(0 != readFileList(p_path, file_db)) {
        root["error"] = 404;
        root["data"] = "获取有误，请稍后再试";
    }
    else {
        Json::Value item;

        if(file_db.empty()) {
            root["error"] = 0;
            root["data"].resize(0);
        }
        else {
            for(int i = 0; i < file_db.size(); i++) { item.append(file_db[i]);}
            root["error"] = 0;
            root["data"] = item;
        }      
    } 

    Json::FastWriter fast_writer;
    return fast_writer.write(root);
}

/*
    函数名称：get_alarmDB_dataNum_sqlCallback
    函数功能：获取指定报警信息数据库中的报警信息数量--回调函数
    传入参数：
                data 调用该函数的父函数传给该函数的值
                argc 一条查询记录中，列数
                argv 查询到结果以字符串列表的形式存到改变量中
                azColName 一条查询结果中，以字符串列表的形式，存储所有的列的名称
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-05
*/
static int get_alarmDB_dataNum_sqlCallback(void *data, int argc, char **argv, char **azColName){
    *((int *)data) = atoi(argv[0]);

    return 0x00;
}

/*
    函数名称：get_alarmDB_dataNum
    函数功能：获取指定报警信息数据库中的报警信息数量
    传入参数：const char *_db_path       数据库文件路径
    传出数据：
                < 0 运行有误
                >=0 报警信息数量
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-05
*/
static int get_alarmDB_dataNum(const char *_db_path){
    /*参数校验*/
    if(!_db_path || strlen(_db_path) <= 0x00) return -0x01;

    /*判断文件是否存在*/
    if(access(_db_path,F_OK)) return -0x02;

    /*操作数据库*/
    int alarmNum = -0x01;
    dbcontrl db(_db_path);
    if(db.exec(
        "SELECT count(*) AS alarmNum FROM alarm_info",
        get_alarmDB_dataNum_sqlCallback,
        (void *)&alarmNum
    )) return -0x03;

    return alarmNum;
}

/*
    函数名称：gen_tar_file
    函数功能：在display参数为2时，生成指定文件的压缩文件
    传入参数：
            const char *p_db_path       db文件的搜索目录
            const string& filename      db文件的文件名
            const string& down_path     生成的压缩文件所在的目录
    传出数据：
            0   生成压缩文件成功  
            -1  查找指定文件所在的路径失败
            -2  db文件转csv文件失败

    注意事项：无
    编写人员：柳恩军
    编写时间：2017-02-15

    修改内容：经测试发现，报警信息数据库数据内容为空的情况下，生成文件操作会出现异常，添加了生成文件前的校验功能（检测报警内容数量）
    传出数据：
                1   检测报警信息数量-数据库路径为空
                2   检测报警信息数量-数据库路径指向的文件不存在
                3   检测报警信息数量-数据库操作失败
    修改人员：王凤龙
    修改时间：2017-05-05
*/
static int gen_tar_file(const char *p_db_path, const string& filename, const string& down_path) {
  
    /* 查找文件所在路径 */
    string find_filepath;
    int err = find_file_path(string(p_db_path), filename, find_filepath);
    if(err != 0) return -1;

    /* db文件转csv文件 */
    string csv_file = "./" + filename + ".csv";
    string db_file = find_filepath + filename + ".db"; 

    /*检测是否有报警信息*/
    switch(get_alarmDB_dataNum(db_file.c_str())){
        case -1: return 0x01;               //路径为空   
        case -2: return 0x02;               //路径指向的文件不存在
        case -3: return 0x03;               //数据库操作失败   
        case  0: return 0x04;               //数据库中的报警信息为空
    }

    err = db_to_csv(db_file, csv_file);
    if(err != 0) return -2;
    
    /* 压缩CSV文件 */
    char buf[256] = {0x00};
    sprintf(buf, "tar -zcvf %s.tar.gz %s", filename.c_str(), csv_file.c_str());
    system(buf);

    /* 删除临时文件.csv */
    sprintf(buf, "rm -rf %s", csv_file.c_str());
    system(buf);

    /* 移动压缩文件到下载目录 */
    sprintf(buf, "mv -f  ./%s.tar.gz %s", filename.c_str(), down_path.c_str());
    system(buf);

    return 0;
}



/*
函数名称：get_tar_file
函数功能：在display参数为2时，调用的函数，执行对应的操作，并生成JSON字符串内容
传入参数：  
        char *p_db_path     DB文件搜索路径
        string& filename    db文件的文件名
        string& down_path   生成的压缩文件所在的目录
传出数据：
        生成的JSON字符串
注意事项：无
编写人员：柳恩军
编写时间：2017-02-15
*/
static string get_tar_file(const char *p_db_path, string& filename, const char *down_path) {
    Json::Value root;

    int err = gen_tar_file(p_db_path, filename, string(down_path));
    switch(err){
        case 0:
            root["error"] = 0;
            root["data"] = filename + ".tar.gz";
            break;
        
        case  1:
        case  2:
        case -1:
            root["error"] = 404;
            root["data"] = "历史故障文件已不存在，请刷新页面";
            break;

        case  3:
        case -2:
            root["error"] = 404;
            root["data"] = "历史故障信息获取失败";
            break;

        case 4:
            root["error"] = 404;
            root["data"] = "历史故障信息为空";
            break;

        default:
            root["error"] = 404;
            root["data"] = "服务器故障，请稍后再试";
            break;
    }

    Json::FastWriter fast_writer;
    return fast_writer.write(root);
}


void cmd_get_filelist() {
    cout<<"Content-type:text/html\r\n\r\n";
    cout<< filelist_to_json(DB_BASE_PATH) << endl;
}

void cmd_get_tar_file(Json::Value &jsonData) {
    string filename = jsonData["data"].asString();
    cout<<"Content-type:text/html\r\n\r\n";
    cout<< get_tar_file(DB_BASE_PATH, filename, DOWNLOAD_TAR_PATH) << endl;
}
