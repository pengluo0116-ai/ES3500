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
#include "gbk_utf8.h"

using namespace std;


int readFileList(string basepath, vector<string>& _file);
int find_file_path(const string basepath, const string filename, string& find_path);

int db_to_csv(const string& db_file, const string& csv_file);

#endif
