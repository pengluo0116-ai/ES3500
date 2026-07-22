#coding:utf-8

##################################################################
#   通过解析上传的文件内容，解析并生成语音
##################################################################

import web
import os
import tarfile
import json
import uuid
import datetime
from conf.config import conf
from mode.page_mode import page_mode
from cont.cont_baiduTts import cont_baiduTts

class upload_tts:
    def __create_page(self, _responseStr):
        page = page_mode.upload_tts_recv
        page = page.replace("$RECVSTR$", _responseStr)
        return page


    def GET(self):
        return self.__create_page("""{"error":404, "data":"方位无效"}""")

    def POST(self):
        web.header("Content-type", "text/html; charset=utf-8")
        #获取上传文件
        file_size = web.utils.intget(web.ctx.env.get("CONTENT_LENGTH"), 0)
        print "上传文件大小：" + str(file_size)

        if file_size <= 0x00: return self.__create_page("""{"error":501, "data":"上传文件为空"}""")
        if file_size > conf.WEB_UPLOAD_LEN_MAX: return self.__create_page("""{"error":502, "data":"上传文件过大"}""")

        #将上传内容解析成json格式
        jsonList = None
        try:
            x = web.input(userfile={})
            if "userfile" in x:
                jsonList = json.loads(str(x.userfile.file.read()), encoding="utf-8")
        except Exception, e:
            print "上传文件转换成json错误：" + str(e)
            return self.__create_page("""{"error":503, "data":"上传文件格式有误"}""")

        if len(jsonList) > 100: 
            print "创建音频数:" + str(len(jsonList))
            return self.__create_page("""{"error":503, "data":"要创建的音频数量过多，单次最多创建100个音频"}""")

        #创建下载目录
        mp3_path = "list_" + str(uuid.uuid1()) + "_" + datetime.datetime.now().strftime("%Y%m%d")
        download_path = conf.MP3_DOWNLOAD_PAHT + "/" + mp3_path

        try: os.mkdir(download_path)
        except: self.__create_page("""{"error":504, "data":"服务器创建现在目录失败，请稍候再试"}""")

        #下载音频
        for item in jsonList:
            try:
                #获取音频名称
                mp3_name = str(item["name"]) if "name" in item else None
                if mp3_name == None: continue

                #生成音频
                err = cont_baiduTts.create_baiduTTS(mp3_name, download_path)
                if err: 
                    item["error"] = 1
                    item["log"] = "创建失败-错误码" + str(err); continue

                item["error"] = 0
                item["log"] = "创建成功"
            except Exception, e:
                item["error"] = 1
                item["log"] = "创建失败-服务器异常("+ str(e) +")"
        else:
            if len(jsonList) <= 0x00: return self.__create_page("""{"error":505, "data":"上传文件内容为空，或者格式有误"}""")

        #进行打包
        jsonPost = {}
        jsonPost["c"] = 101
        jsonPost["error"] = 0
        jsonPost["data"] = jsonList

        file_log = None
        try: file_log = open(download_path + "/log", "w")
        except: return self.__create_page("""{"error":505, "data":"音频日志创建失败"}""")

        try:
            file_log.write(json.dumps(jsonPost, encoding='utf-8', ensure_ascii=False))
        except:
            pass
        finally: file_log.close()

        try: 
            with tarfile.open(download_path + ".tar", "w:gz") as tar: tar.add(download_path, arcname=os.path.basename(download_path))
        except: 
            return self.__create_page("""{"error":505, "data":"音频文件打包失败，请稍后重试"}""")

        jsonPost["download"] = conf.WEB_SERVER_NAME + "/" + conf.WEB_DOWNLOAD_PATH + "/" + mp3_path + ".tar"
        return self.__create_page(json.dumps(jsonPost, encoding='utf-8', ensure_ascii=False))