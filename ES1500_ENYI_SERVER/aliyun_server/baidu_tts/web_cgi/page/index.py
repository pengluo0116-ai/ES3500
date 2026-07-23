#coding:utf-8

##################################################################
#   index页面
##################################################################

import web

class index:
    def GET(self):
        return "hello"
    
    def POST(self):
        pdata = web.data()
        print str(pdata)
        return "ok\r\n"