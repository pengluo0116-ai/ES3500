#ifndef _FILE_OPT_H_
#define _FILE_OPT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <vector>

#include <fstream>  
#include "../include/sqlite/sqlite3.h"
#include "../include/json/json.h"

using namespace std;

int find_file(const string basepath, const string filename);
int get_db_data_num(const string& db_file, int *num);
int get_db_data_info(const string &db_file, const int page_num, const int page_id, Json::Value& data);

#endif
