#coding=utf-8

#########################################################################
#   文档简介
#   微信信息推送类
#########################################################################

import urllib2
import json
import types


class wchart:
    '''
    函数名称：__init__
    函数功能：构造函数
    传入参数：查看函数内容
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-08-07
    '''
    def __init__(self, _wurl, _wappid, _wappsecret, _wdeviceurl):
        self.__URL          = _wurl                                 #微信服务器URL
        self.__APPID        = _wappid                               #公众号APPID
        self.__APPSECRET_ID = _wappsecret                           #公众号SECRETID
        self.__DEVICE_URL   = _wdeviceurl                           #公众号设备推送接口

    '''
    函数名称：init
    函数功能：初始化
    传入参数：无
    传出数据：
                 0  运行成功
                -1  参数非法-微信服务器URL有误
                -2  参数非法-微信APPID有误
                -3  参数非法-微信APPSECRET有误
                -4  参数非法-微信设备提交URL有误
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-08-07
    '''
    def init(self):
        #参数校验
        if self.__URL==None or type(self.__URL)!=types.StringType or len(self.__URL)<=0x00: return -0x01
        if self.__APPID==None or type(self.__APPID)!=types.StringType or len(self.__APPID)<=0x00: return -0x02
        if self.__APPSECRET_ID==None or type(self.__APPSECRET_ID)!=types.StringType or len(self.__APPSECRET_ID)<=0x00: return -0x03
        if self.__DEVICE_URL==None or type(self.__DEVICE_URL)!=types.StringType or len(self.__DEVICE_URL)<=0x00: return -0x04        
        return 0x00

    '''
    函数名称：__GET
    函数功能：GET请求WEB服务器
    传入参数：_url 请求url
    传出数据：
                字典类型 {"error":0, "data":""}
                如果error的键值不为0时，函数运行有误，data显示错误内容
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-08-07
    '''
    def __GET(self, _url):
        error_msg = {}
        error_msg["error"] = 0
        error_msg["data"] = None

        try:
            req = urllib2.Request(_url)
            res = urllib2.urlopen(req)
            error_msg["data"] = res.read()
        
        except urllib2.HTTPError, e:
            #网页请求错误 400 404
            error_msg["error"]  = 404
            error_msg["data"]   = "服务器返回信息失败"

        except urllib2.URLError, e:
            #网页地址不存在
            error_msg["error"]  = 404
            error_msg["data"]   = "服务器地址错误"

        except Exception, e:
            #请求超时
            error_msg["error"]  = 999
            error_msg["data"]   = "服务器连接超时-未知错误"

        return error_msg

    '''
    函数名称：__POST
    函数功能：POST请求WEB服务器
    传入参数：
                _url    请求URL
                _data   提交数据
    传出数据：
                字典类型 {"error":0, "data":""}
                如果error的键值不为0时，函数运行有误，data显示错误内容
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-08-07
    '''
    def __POST(self, _url, _data):
        error_msg = {}
        error_msg["error"] = 0
        error_msg["data"] = None

        try:
            req = urllib2.Request(url=_url, data=_data)
            res = urllib2.urlopen(req)
            error_msg["data"] = res.read()

        except urllib2.HTTPError, e:
            #网页请求错误 400 404
            error_msg["error"]  = 404
            error_msg["data"]   = "服务器返回信息失败"

        except urllib2.URLError, e:
            #网页地址不存在
            error_msg["error"]  = 404
            error_msg["data"]   = "服务器地址错误"

        except Exception, e:
            #请求超时
            error_msg["error"]  = 999
            error_msg["data"]   = "服务器连接超时-未知错误"

        return error_msg

    '''
    函数名称：sendModeMsg
    函数功能：推送模板消息
    传入参数：
                _toUserID       接收消息的用户APPID，用户绑定到该公众号下的APPID
                _wtoken         公众号token
                _msgModeID      消息模板ID
                _rdurl          用户点击模板消息中"跳转"，转到的页面，如果不加跳转，可以直接为空字符串
                _data           模板内容，需要根据模板提供的格式编写
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-08-07
    '''
    def sendModeMsg(self, _toUserID, _wtoken, _msgModeID, _rdurl, _data):
        #配置URL
        url = self.__URL + "message/template/send?access_token=" + _wtoken

        #配置推送数据
        postBody = {}
        postBody["touser"]      = _toUserID
        postBody["template_id"] = _msgModeID
        postBody["url"]         = _rdurl
        postBody["data"]        = _data

        error_msg   = {}
        postMsg     = None
        try:
            postMsg = json.dumps(postBody, encoding='utf-8', ensure_ascii=False)
        except:
            error_msg["error"] = 404
            error_msg["data"] = "数据格式错误"
            return  error_msg

        #推送数据
        error_msg = self.__POST(url, str(postMsg))

        #解析服务器返回数据
        if error_msg["error"] == 0:
            temp = json.loads(error_msg["data"], encoding="utf-8")
            try:
                error_msg["error"]  = temp["errcode"]
                error_msg["data"]   = str(temp["errmsg"])
            except:
                error_msg["error"] = 0
                error_msg["data"] = ""
            
        return error_msg

