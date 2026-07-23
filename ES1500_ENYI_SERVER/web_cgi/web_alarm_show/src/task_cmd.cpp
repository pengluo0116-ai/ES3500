#include "task_cmd.h"
using namespace std;

/*
函数名称：get_alarm_num
函数功能：获取报警历史数据中的总个数
传入参数：
           const string& basepath   db文件的目录
           const string& filename   db文件的文件名
传出数据：
          生成的JSON字符串 
注意事项：无
编写人员：柳恩军
编写时间：2017-02-16
*/
static string get_alarm_num(const string& basepath, const string& filename) {
    Json::Value root;
    Json::FastWriter fast_writer;

    /* 查找文件是否存在 */
    int err = find_file(basepath, filename);
    if(err != 0) {
        root["error"] = 404;
        root["data"] = "获取有误，请稍后再试";
        return fast_writer.write(root); 
    }

    /* 获取数据个数*/
    int num;
    string db_file = basepath + filename + ".db";
    err = get_db_data_num(db_file, &num);
    if(err != 0) {
        root["error"] = 404;
        root["data"] = "获取有误，请稍后再试";
        return fast_writer.write(root); 
    }

    /* 生成JSON字符串并返回 */
    root["error"] = 0;
    root["data"] = num;
    return fast_writer.write(root); 
}


/*
函数名称：get_alarm_page_data
函数功能：获取报警指定页的报警信息
传入参数：
           const string& basepath   db文件的路径
           const string& filename   db文件的文件名
           const int page_num       一页的个数
           const int page_id        指定显示的页号
传出数据：
          生成的JSON字符串 
注意事项：无
编写人员：柳恩军
编写时间：2017-02-16
*/
static string get_alarm_page_data(const string& basepath, const string& filename, const int page_num, const int page_id) {
    Json::Value root;
    Json::Value data;

    if((page_num == 0) && (page_id == 0)) {

    }

    /* 获取数据的信息的JSON字符串信息 */
    string db_file = basepath + filename + ".db";

    int err = get_db_data_info(db_file, page_num, page_id, data);
    if(err != 0) {
        root["error"] = 404;
        root["data"] = "获取有误，请稍后再试";
    }
    else {
        root["error"] = 0;
        root["data"] = data;
    }

    /* 生成JSON字符串并返回 */
    Json::FastWriter fast_writer;
    return fast_writer.write(root);
}



/*
    函数名称：cmd_get_alarm_num
    函数功能：
    传入参数：Json::Value &jsonData
    传出数据：无
    注意事项：无
    编写人员：柳恩军
    编写时间：2017-01-16
*/
void cmd_get_alarm_num(Json::Value &jsonData) {
    string db_file = jsonData["data"].asString();

    cout<<"Content-type:text/html\r\n\r\n";
    cout<< get_alarm_num(DB_BASE_PATH, db_file) << endl;
}

/*
    函数名称：cmd_get_alarm_page_data
    函数功能：
    传入参数：Json::Value &jsonData
    传出数据：无
    注意事项：无
    编写人员：
    编写时间：
*/
void cmd_get_alarm_page_data(Json::Value &jsonData){
    string db_file = jsonData["data"].asString();
    int page_num = jsonData["pnum"].asInt();
    int page_id = jsonData["page"].asInt();

    cout<<"Content-type:text/html\r\n\r\n";
    cout<< get_alarm_page_data(DB_BASE_PATH, db_file, page_num, page_id) << endl;
}