#coding:utf-8

##################################################################
#   页面管理器
##################################################################
import web
from page.index import index
from page.upload_tts import upload_tts
from page.radio import radio

class myweb:
    __url = (
        "/cgi-bin/index",           "index",
        "/cgi-bin/upload",          "upload_tts",
        "/cgi-bin/radio",           "radio"
    )

    __app = None
    app = None

    @staticmethod
    def init():
        myweb.__app = web.application(myweb.__url, globals())
        myweb.app = myweb.__app

    @staticmethod
    def work():
        myweb.__app.run()
