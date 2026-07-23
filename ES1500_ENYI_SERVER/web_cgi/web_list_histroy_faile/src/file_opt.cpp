#include "file_opt.h"


/*
函数名称：readFileList
函数功能：遍历指定目录下所有的db文件，并加入到数组中
传入参数：
            string basepath         要遍历的目录
            vector<string>& _file   遍历完成后，得到的文件名数组
传出数据：
        0   遍历成功
        -1  打开遍历目录失败 
注意事项：
编写人员：柳恩军
编写时间：2017-02-15
*/
int readFileList(string basepath, vector<string>& _file) {
    DIR *dir;
    struct dirent *ptr;

    if ((dir = opendir(basepath.c_str())) == NULL) return -1;

    char buf[128] = {0};
 
    while ((ptr = readdir(dir)) != NULL) {
        if(ptr->d_type == 8) { //file
            int t;     
            
            if(sscanf(ptr->d_name, "%d.db", &t) == 1) {
                if(strlen(ptr->d_name) != 9) continue;

                strcpy(buf, ptr->d_name);
                buf[strlen(ptr->d_name) - 3] = '\0';
                _file.push_back(string(buf));
            } 
            continue;
        }

        if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }

        if(ptr->d_type == 4) { //dir
            string base = basepath + "/" + string(ptr->d_name);    
            readFileList(base, _file);  
        }   
    }

    closedir(dir);

    return 0;
}



/*
函数名称：find_file_path
函数功能：搜索指定文件名所在的目录
传入参数：
            const string basepath   要遍历的目录
            const string filename   搜索的文件名
            string& find_path       文件名所在的目录
传出数据：
            0   成功找到文件所在目录
            -1  打开遍历目录失败
注意事项：无
编写人员：柳恩军
编写时间：2017-02-15
*/
int find_file_path(const string basepath, const string filename, string& find_path) {
    DIR *dir;
    struct dirent *ptr;

    string find_file = string(filename) + ".db";  

    if ((dir = opendir(basepath.c_str())) == NULL) return -1;

    while ((ptr = readdir(dir)) != NULL) {
        if(ptr->d_type == 8) { //file
            if(string(ptr->d_name) == find_file) find_path = basepath;      
        }
        else if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }
        else if (ptr->d_type == 4) { //dir
            string str = basepath + "/" + ptr->d_name;
            find_file_path(str, filename, find_path);
        }
    }

    closedir(dir);

    return 0;
}

static bool cvs_col_name_ok;//标识cvs文件的第一行显示列名



/*
函数名称：get_csv_str_data
函数功能：生成csv文件格式的一行数据
传入参数：
            int argc            列的个数
            char **argv         每一列的参数
传出数据：
            0  生成CSV格式的一行数据成功
            -1  列的个数错误
            -2  列的个数错误
注意事项：无
编写人员：柳恩军
编写时间：2017-02-15
*/
int get_csv_str_data(int argc, char **argv, string& str) {
    /* 参数检查 */
    if(argc != 23) return -1;

    /* 转化为cvs记录的格式 */
    for(int i = 0; i < argc; i++) {
        string s = string(argv[i]);

        switch(i) {
        case 0:
        case 1:
        case 2:  str += s + ",";break;
        case 3:  str += (s == "1") ? "地上 " : "地下 "; break;
        case 4:  str += s + ",";break;
        case 5:  str += (s == "1") ? "卡层 " : "";break;
        case 6:  str += (s == "1") ? "困人 " : "";break;
        case 7:  str += (s == "1") ? "冲顶 " : "";break;
        case 8:  str += (s == "1") ? "遁底 " : "";break;
        case 9:  str += (s == "1") ? "开门走梯 " : "";break;
        case 10: str += (s == "1") ? "超速 " : "";break;
        case 11: str += (s == "1") ? "断电 " : "";break;
        case 12: str += (s == "1") ? "停梯不开门 " : "";break;
        case 13: str += (s == "1") ? "停梯不关门 " : "";break;
        case 14: str += (s == "1") ? "重复开关门 " : "";break;
        case 15: str += (s == "1") ? "电梯震动 " : "";break;
        case 16: str += (s == "1") ? "电池电压低 " : "";break;
        case 17: str += (s == "1") ? "外部掉电 " : "";break;
        case 18: str += (s == "1") ? "电动车入梯 " : "";break;
        case 19: str += (s == "1") ? "手动报警 " : "";break;
        case 20: str += (s == "1") ? "电梯前后倾斜 " : "";break;
        case 21: str += (s == "1") ? "电梯左右倾斜 " : "";break;
        case 22: str += (s == "1") ? "长期未维保 " : "";break;

        default:return -2;
        }
    }

    str += "\r\n";
    return 0;
}

/*
函数名称：callback
函数功能：sql执行时的回调函数
传入参数：
            void *data          生成CSV文件的路径   
            int argc            列的个数
            char **argv         每一列的参数
            char **azColName    每一列的名称
传出数据：
            0  回调成功
注意事项：无
编写人员：柳恩军
编写时间：2017-02-15
*/
static int callback(void *data, int argc, char **argv, char **azColName){
    ofstream outfile((char*)data, ios::app );  
    char buf[256] = {0};
    string str;

    /* 是否显示列名 */
    if(cvs_col_name_ok == false) {
        cvs_col_name_ok = true;
        str = "电梯ID,电梯名称,报警时间,楼层信息,报警内容";
        memset(buf, 0x00, sizeof(buf));
        u2g((char *)str.c_str(), str.length(), buf, sizeof(buf));
        outfile << buf;
        outfile << "\r\n";
    }

    /* 生成CSV文件 每一行的显示的数据 */
    str = "";
    int err = get_csv_str_data(argc, argv, str);
    if(err != 0) return -1;

    /* 转化编码 */
    u2g((char *)str.c_str(), str.length(), buf, sizeof(buf));

    /* 写入到CSV文件中 */
    outfile << buf;
    outfile.close();  

    return 0;
}

/*
函数名称：db_to_csv
函数功能：读取db文件，并将文件内容转换为 csv文件
传入参数：
            const string& db_file       数据库文件的路径
            const string& csv_file      csv文件的路径
传出数据：
            0   转换成功
            -1  打开数据库失败
            -2  执行sql语句错误
注意事项：无
编写人员：柳恩军
编写时间：2017-02-15
*/
int db_to_csv(const string& db_file, const string& csv_file) {
   sqlite3 *db;

   /* Open database */
   int rc = sqlite3_open(db_file.c_str(), &db);
   if( rc ) {
      //fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return -1;
   }

   /* Create SQL statement */ 
   char *sql = (char *)"SELECT dev_id,dev_name,alarm_time,ds_dx,lou_ceng,kaceng,kunren,up_alarm,down_alarm,run_door,speeding,power,\
            stp_doorunopen,stp_doorunclose,stp_reapet,amplitude,voltage,power_down,electromobile,manaual_alarm,slant_fb,slant_lr,maintain from alarm_info";

   /* Execute SQL statement */
   char *zErrMsg = 0;
   cvs_col_name_ok = false;
   rc = sqlite3_exec(db, sql, callback, (void*)csv_file.c_str(), &zErrMsg);
   if(rc != SQLITE_OK) {
      //fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return -2;
   }
   sqlite3_close(db);

   return 0;
}
