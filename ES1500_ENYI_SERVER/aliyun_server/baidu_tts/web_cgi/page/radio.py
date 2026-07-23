#coding:utf-8

##################################################################
#   radio页面，获取单个百度语音
##################################################################

import web
import os
import uuid
import json
import types
import hashlib
import datetime
from cont.cont_baiduTts import cont_baiduTts
from conf.config import conf

class radio:
    def GET(self):
        return """{"error":404, "data":"Do not GET"}"""

    def POST(self):
        #获取提交数据
        pdata = web.data()
        if pdata==None or type(pdata)!=types.StringType or len(pdata)<=0x00:
            return """{"error":404, "data":"submit is null"}"""
        
        print  "获取数据:" + pdata
        #将提交的数据转换成json
        jsonData = None
        try: jsonData = json.loads(pdata, encoding="utf-8")
        except: jsonData = None

        if jsonData == None:
            return """{"error":404, "data":"submit data error"}"""
        
        #转换成音频
        mp3_path = str(uuid.uuid1()) + "_" + datetime.datetime.now().strftime("%Y%m%d")
        download_path = conf.MP3_DOWNLOAD_PAHT + "/" + mp3_path
        try: os.mkdir(download_path)
        except: """{"error":404, "data":"Server err: CD"}"""

        mp3_name = str(jsonData["name"]) if "name" in jsonData else None
        if mp3_name == None: return """{"error":404, "data":"submit data error: A"}"""

        err = 0
        try: err = cont_baiduTts.create_baiduTTS(mp3_name, download_path)
        except: return """{"error":404, "data":"server get radio error: B"}"""

        if err:
            print "音频获取错误" + str(err) 
            return """{"error":404, "data":"server get radio error: C"}"""

        #返回信息
        msha1 = hashlib.sha1()
        msha1.update( mp3_name )

        jsonPost = {}
        jsonPost["error"] = 0
        jsonPost["name"] = msha1.hexdigest() + ".mp3"
        jsonPost["download"] = conf.WEB_SERVER_NAME + "/" + conf.WEB_DOWNLOAD_PATH + "/" + mp3_path + "/" + str(jsonPost["name"])

        return json.dumps(jsonPost, encoding='utf-8', ensure_ascii=False)



    