#coding=utf-8

##################################################################
#   页面管理器
##################################################################

import web
from page.Wt_liftalarm import Wt_liftalarm

class myweb:
    app = None
    __url = (
        "/",            "Wt_liftalarm"
    )

    @staticmethod
    def init():
        myweb.app = web.application(myweb.__url, globals())

    @staticmethod
    def work():
        myweb.app.run()