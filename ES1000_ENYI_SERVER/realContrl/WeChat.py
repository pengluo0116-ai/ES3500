#coding=utf-8

'''
微信服务器交互类
获取token功能
'''

import urllib2
import json
import base64
import ssl
import sys
reload(sys)
sys.setdefaultencoding('utf8')
ssl._create_default_https_context = ssl._create_unverified_context  # 全局取消证书验证

class server:

    #构造函数
    def __init__(self):
        #从配置共享区读取该类需要的数据
        self.__WCHART_SERVER_URL = "https://api.weixin.qq.com/cgi-bin/" 	#微信服务器地址
        self.__WCHART_APPID      = "wxcf9c2a59f25a0d4b"					    #公众号的ID
        self.__WCHART_APPSECRET  = "WCHART_APPSECRET"						#密钥
        self.__WCHART_DEVICE_URL = "https://api.weixin.qq.com/device/"		#设备授权接口地址

######################################################
    #内部函数：GET方式访问web服务器
    #返回数据：{"error":0, "data":"数据"}
    #备注：
    # 返回数据中的error=0时，函数运行正确； data中包含运行函数得出的数据，如何得出数据为空的话 data为None
    # 返回数据中的error!=0时，函数运行异常; data中包含运行函数异常的说明
    def __GetRequst(self, url):

        error_msg = {}
        error_msg["error"] = 0
        error_msg["data"] = None

        try:
            req = urllib2.Request(url)
            res = urllib2.urlopen(req)
            error_msg["data"] = res.read()

        except urllib2.HTTPError, e:
            #网页串错误代码 400 404
            error_msg["error"] = int(e.code)
            error_msg["data"] = "从服务器获取信息失败"

        except urllib2.URLError, e:
            #网页地址错误
            error_msg["error"] = 404
            error_msg["data"] = "服务器地址错误"

        except Exception, e:
            #连接超时错误
            error_msg["error"] = 999
            error_msg["data"] = "目标服务器未知错误"

        return error_msg

##########################################################
    #内部函数：POST方式访问web服务器
    #返回数据：{"error":0, "data":"数据"}
    #备注：
    # 返回数据中的error=0时，函数运行正确； data中包含运行函数得出的数据，如何得出数据为空的话 data为None
    # 返回数据中的error!=0时，函数运行异常; data中包含运行函数异常的说明
    def __PostRequst(self, url, data):
        error_msg = {}
        error_msg["error"] = 0
        error_msg["data"] = None

        try:
            req = urllib2.Request(url=url, data=data)
            res = urllib2.urlopen(req)
            error_msg["data"] = res.read()

        except urllib2.HTTPError, e:
            #网页串错误代码 400 404
            error_msg["error"] = int(e.code)
            error_msg["data"] = "从服务器获取信息失败"

        except urllib2.URLError, e:
            #网页地址错误
            error_msg["error"] = 404
            error_msg["data"] = "服务器地址错误"

        except Exception, e:
            #连接超时错误
            print e
            error_msg["error"] = 999
            error_msg["data"] = "目标服务器未知错误"

        return error_msg

###################################################
    #外部对象函数：获取微信access tocken数据
    #返回数据：{"error":0, "data":"数据"}
    #备注：
    # 返回数据中的error=0时，函数运行正确； data中包含运行函数得出的数据，如何得出数据为空的话 data为None
    # 返回数据中的error!=0时，函数运行异常; data中包含运行函数异常的说明
    def getAccessTocken(self):
        url = self.__WCHART_SERVER_URL + "token?"
        url += "grant_type=client_credential&appid=" + self.__WCHART_APPID
        url += "&secret=" + self.__WCHART_APPSECRET

        getTupleMsg = self.__GetRequst(url)
        if getTupleMsg["error"] == 0:
            temp = json.loads(getTupleMsg["data"], encoding="utf-8")
            getTupleMsg["data"] = str(temp["access_token"])

        return getTupleMsg


###################################################
    #外部对象函数：获取微信ticket
    # def getTicket(self):
    #     url = self.__WCHART_SERVER_URL + "ticket/getticket?access_token="
    #     url += dataBox.dataBox.getAccessTocken()
    #     url += "&type=jsapi"

    #     getMsg = self.__GetRequst(url)
    #     if getMsg["error"] == 0:
    #         temp = json.loads(getMsg["data"], encoding="utf-8")

    #         #成功获取ticket
    #         if temp["errcode"] == 0:
    #             getMsg["data"] = str(temp["ticket"])
    #         #获取ticket失败
    #         else:
    #             getMsg["error"] = temp["errcode"]
    #             getMsg["data"] = temp["errmsg"]

    #     return  getMsg



###################################################
    #外部对象函数：向指定的appID发送信息
    #返回数据：{"error":0, "data":"数据"}
    #备注：
    # 返回数据中的error=0时，函数运行正确； data中包含运行函数得出的数据，如何得出数据为空的话 data为None
    # 返回数据中的error!=0时，函数运行异常; data中包含运行函数异常的说明
    def sendMsg(self, appID, rtoken, msg):
        url = self.__WCHART_SERVER_URL + "message/custom/send?access_token="
        url += rtoken
        sendMsg = {}
        sendMsg["content"] = msg
        sendBody = {}
        sendBody["touser"] = appID
        sendBody["msgtype"] = "text"
        sendBody["text"] = sendMsg

        Msg = json.dumps(sendBody, encoding="utf-8", ensure_ascii=False)
        GetMsg = self.__PostRequst(url, str(Msg))

        #系统错误
        if GetMsg["error"] != 0:
            return GetMsg

        json_temp = json.loads(GetMsg["data"])
        GetMsg["error"] = json_temp["errcode"]
        GetMsg["data"] = json_temp["errmsg"]
        return GetMsg

##################################################################################
        #外部部函数：发送模板消息
        #传入参数：
        # openID:接收消息的微信客户端(绑定到该公众号下的APPID)
        # templateID:模板ID(公众号申请到的模板)
        # url:模板消息点击“详情”跳转的页面
        # Data:模板内容，需要根据模板提供的格式进行填写
    def sendModelMsg(self, openID, rtoken,templateID, rdurl, Data):
        url = self.__WCHART_SERVER_URL + "message/template/send?access_token="
        url += rtoken

        postBody = {}
        postBody["touser"] = openID
        postBody["template_id"] = templateID
        postBody["url"] = rdurl
        postBody["data"] = Data

        error_msg = {}
        postMsg = None
        try:
            postMsg = json.dumps(postBody, encoding="utf-8", ensure_ascii=False)
        except:
            error_msg["error"] = 404
            error_msg["data"] = "The data format is wrong"
            return  error_msg

        error_msg = self.__PostRequst(url, str(postMsg))
        if error_msg["error"] == 0:
            temp = json.loads(error_msg["data"], encoding="utf-8")
            try:
                error_msg["error"] = temp["errcode"]
                error_msg["data"] = str(temp["errmsg"])
            except:
                error_msg["error"] = 0
                error_msg["data"] = ""

        return error_msg


#####################################################################################################
# postBody = {}
# postBody["first"] = {"value":"电梯报警", "color":"#173177"} #标题
# postBody["keyword1"] = {"value":"系统报警", "color":"#173177"} #故障用户
# postBody["keyword2"] = {"value":"2015-11-19 10:00:00", "color":"#173177"} #故障时间
# postBody["keyword3"] = {"value":"卡层，困人", "color":"#173177"} #故障现象
# postBody["remark"] =  {"value":"地址：山东省济南市高新开发区\n上次维保日期：2015-10-15\n测试发送", "color":"#173177"} #故障现象
#
# wechat_tmp = server()
# err = wechat_tmp.sendModelMsg(
#     "o25k0sz19hw_78ulBBw7Oou5FgvU",
#     "-EIS9C8KmrskJrA4qYOP60IdDILErmw7tKf8Mj8mMfjXNoI9tESv1Uf92FlGb0YEoTd5jXCIKytkhobnY5tknm52KJKITxe_MP110YnTfC4JTVcS8CflnPo-pDk5DKsONDZgAJAWMI",
#     "3gFJjNbu73C23pqQ0Aa2JPtLbQ2cwaiccv_EhihXCOs",
#     "http://www.baidu.com",
#     postBody
# )
# print str(err)
# cfg.SysConf.readConfFile("../conf/conf.ini")
# demo = server()
# postBody = {}
# postBody["first"] = {"value":"电梯报警", "color":"#173177"} #标题
# postBody["keyword1"] = {"value":"系统报警", "color":"#173177"} #故障用户
# postBody["keyword2"] = {"value":"2015-11-19 10:00:00", "color":"#173177"} #故障时间
# postBody["keyword3"] = {"value":"卡层，困人", "color":"#173177"} #故障现象
# postBody["remark"] =  {"value":"地址：山东省济南市高新开发区\n上次维保日期：2015-10-15\n测试发送", "color":"#173177"} #故障现象
#
# a = 0
# while True:
#     a+=1
#     cs = demo.sendModelMsg(
#     #"o25k0s7eIFHT8OzFRyTxFbsEgzQQ", #高坤
#     "o25k0s6wIhMz-Ec2OXQzzc1QFD9U",
#     #"o25k0s3dlhOSAz5faH6gCMbPzdCM",#老李
#     #"o25k0s7-5ctDC2BFoWQhuk8jOrD0",#罗鹏
#     "3gFJjNbu73C23pqQ0Aa2JPtLbQ2cwaiccv_EhihXCOs",
#     "http://www.emblink.com",
#     postBody)
#     print cs
#     if a==10:break
