#ifndef __CKUSER_COOKIE_H__
#define __CKUSER_COOKIE_H__
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include "../include/sqlite/sqlite3.h"
#include "../include/json/json.h"
#include "cookie.h"
#include "dbcontrl.h"
#include "base64.h"
#include "conf.h"

class ckuser_cookie{
    public:
        ckuser_cookie();
        ~ckuser_cookie();
        
    public:
        int ckUserBySql(const char *_name, const char *_pasd);
        int ckUser();   //校验cookie
        int writeUserToCookie(const char *_name, const char *_pasd);    //将用户名,密码写入cookie
};

#endif