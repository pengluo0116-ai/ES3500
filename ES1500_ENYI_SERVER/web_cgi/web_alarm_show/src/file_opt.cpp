#include "file_opt.h"


/*
函数名称：find_file_path
函数功能：搜索指定文件名所在的目录
传入参数：
            const string basepath   要遍历的目录
            const string filename   搜索的文件名
传出数据：
            0   成功找到文件
            -1  打开遍历目录失败
            -2  没有找到文件
注意事项：无
编写人员：柳恩军
编写时间：2017-02-16
*/
int find_file(const string basepath, const string filename) {
    DIR *dir;
    struct dirent *ptr;
    string find_file = string(filename) + ".db";  

    /* 打开遍历目录 */
    if ((dir = opendir(basepath.c_str())) == NULL) return -1;

     /* 遍历目录查找指定文件 */
    while ((ptr = readdir(dir)) != NULL) {
        if(ptr->d_type == 8) { //file
            if(string(ptr->d_name) == find_file) return 0;      
        }
    }
    closedir(dir);

    return -2;
}

/*
函数名称：callback_get_db_num
函数功能：sql执行时的回调函数
传入参数：
            void *data          查询到的记录个数   
            int argc            列的个数
            char **argv         每一列的参数
            char **azColName    每一列的名称
传出数据：
            0  回调成功
            -1 参数个数错误
            -2 参数指针错误
            -3 参数内容错误
注意事项：无
编写人员：柳恩军
编写时间：2017-02-16
*/
static int callback_get_db_num(void *data, int argc, char **argv, char **azColName){

    if(argc != 1) return -1;
    if(argv[0] == NULL) return -2;
    if(sscanf(argv[0], "%d", (int*)data) != 1) return -3;

    return 0;
}

/*
函数名称：get_db_data_num
函数功能：读取db文件，获取db中数据记录的个数
传入参数：
            const string& db_file   要打开的db文件
            int *num                db文件中记录的个数
传出数据：
            0   转换成功
            -1  打开数据库失败
            -2  执行sql语句错误
注意事项：无
编写人员：柳恩军
编写时间：2017-02-15
*/
int get_db_data_num(const string& db_file, int *num) {
    sqlite3 *db;

    /* Open database */
    int rc = sqlite3_open(db_file.c_str(), &db);
    if(rc != 0) {
        //fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    /* Create SQL statement */
    char *sql = (char *)"SELECT  count(*) FROM alarm_info";

    /* Execute SQL statement */
    char *zErrMsg = 0;
    rc = sqlite3_exec(db, sql, callback_get_db_num, (void*)num, &zErrMsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -2;
    }
    sqlite3_close(db);

    return 0;
}


/*
函数名称：callback_get_db_info
函数功能：sql执行时的回调函数
传入参数：
            void *data          转换为的JSON格式变量   
            int argc            列的个数
            char **argv         每一列的参数
            char **azColName    每一列的名称
传出数据：
            0  回调成功       
注意事项：无
编写人员：柳恩军
编写时间：2017-02-16
*/
static int callback_get_db_info(void *data, int argc, char **argv, char **azColName) {
    Json::Value *p_root = (Json::Value *)data;
    Json::Value dat;

    for(int i=0; i < argc; i++) {
        dat[azColName[i]] = argv[i];       
    }

    p_root->append(dat);
    return 0;
}

/*
函数名称：get_db_data_info
函数功能：获取指定区域的数据，并存为JSON格式
传入参数：
         const string &db_file  db文件的路径
         const int page_num     每一页的数据个数
         const int page_id      指定显示的页号
         Json::Value& data      转换为JSON类型的数据
传出数据：
            0       成功转换为JSON格式
            -1      打开db失败
            -2      执行sql语句错误        
注意事项：无
编写人员：柳恩军
编写时间：2017-02-16
*/
int get_db_data_info(const string &db_file, const int page_num, const int page_id, Json::Value& data) {
    sqlite3 *db;

    /* Open database */
    int rc = sqlite3_open(db_file.c_str(), &db);
    if(rc != 0) {
        //fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    /* Create SQL statement */
    char sql[64];
    sprintf(sql, "SELECT * FROM alarm_info LIMIT %d OFFSET %d", page_num, page_id * page_num);
   
    /* Execute SQL statement */
    char *zErrMsg = 0;
    rc = sqlite3_exec(db, sql, callback_get_db_info, (void*)&data, &zErrMsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -2;
    }
    sqlite3_close(db);

    return 0;
}
