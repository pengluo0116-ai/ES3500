#coding:utf-8

#########################################################################
#   百度语音接口-RECT协议
#########################################################################

from conf.config import conf
import json
import hashlib
import urllib2
import urllib
import types


class baidu_tts:
    
    """
    函数名称：__init__
    函数功能：构造函数
    传入参数：
                _client_id      百度应用API-KEY 
                _client_secret  百度应用Secret-KEY
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    """
    def __init__(self, _client_id, _client_secret):
        self.url_getToken = conf.BAIDU_TOKEN_URL        #百度获取API token请求地址
        self.grand_type = conf.BAIDU_GRAND_TYPE         #百度API grand_type
        self.client_id = _client_id                     #百度应用API-KEY
        self.client_secret = _client_secret             #百度应用Secret-Key
        self.access_token = None                        #百度API token
        self.url_getRadio = conf.BAIDU_GETRADIO_URL     #百度音频获取URL
        
        return

    """
    函数名称：init_check
    函数功能：初始化校验
    传入参数：无
    传出数据：
                 0  运行成功
                -1  校验百度获取API token请求地址有误
                -2  校验百度API grand_type有误
                -3  校验百度应用API-KEY有误
                -4  百度应用Secret-Key有误
                -5  百度音频获取URL有误
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-17
    """
    def init_check(self):
        if self.url_getToken == None or type(self.url_getToken) != types.StringType or len(self.url_getToken)<=0x00 :
            return -0x01

        if self.grand_type == None or type(self.grand_type) != types.StringType or len(self.grand_type)<=0x00:
            return -0x02

        if self.client_id == None or type(self.client_id) != types.StringType or len(self.client_id)<=0x00:
            return -0x03

        if self.client_secret == None or type(self.client_secret) != types.StringType or len(self.client_secret)<=0x00:
            return -0x04

        if self.url_getRadio == None or type(self.url_getRadio) != types.StringType or len(self.url_getRadio)<=0x00:
            return -0x05

        self.access_token = None

        return 0x00

    """
    函数名称：getStr_accessToken
    函数功能：获取accessToken
    传入参数：无
    传出数据：accessToken字符串或None
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-18
    """
    def getStr_accecssToken(self):
        return self.access_token

    """
    函数名称：setStr_accessToken
    函数功能：设置accessToken字符串
    传入参数：_accessToken
    传出数据：0 运行正确 非0 运行错误
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-18
    """
    def setStr_accessToken(self, _accessToken):
        #参数校验
        if _accessToken==None or type(_accessToken)!=types.StringType or len(_accessToken)<=0x00: return -0x01

        self.access_token = _accessToken; return 0x00

    """
    函数名称：get_accecssToken
    函数功能：获取access token
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-17
    """
    def get_accecssToken(self):
        #组装发送报文
        post_body = "grant_type=client_credentials&" + \
                    "&client_id=" + self.client_id + \
                    "&client_secret=" + self.client_secret
        
        try:
            #HTTP对话
            req = urllib2.Request(url = self.url_getToken,  data=post_body)
            res = urllib2.urlopen(req)
            str_tmp = res.read()
            
            #将返回的报文体解析成json
            jsonData = json.loads(str_tmp, encoding="utf-8")
            print str_tmp
            self.access_token = str(jsonData["access_token"])

        except urllib2.HTTPError, e:                                                        #服务器返回错误，例400 404
            return -0x01
        except urllib2.URLError, e:                                                         #服务器地址有误
            return -0x02
        except Exception, e:                                                                #未知错误
            return -0x03

        return 0x00

    """
    函数名称：get_baiduRadio
    函数功能：下载百度语音
    传入参数：
                str _radio_str              要进行语音话的文字
                str _radio_filePath         转换成的语音存储的文件路径
                str _uuid                   UUID
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-17
    """
    def get_baiduRadio(self, _radio_str, _radio_filePath, _uuid="00:0c:29:49:d0:65"):
        #参数校验
        if _radio_str==None or type(_radio_str)!=types.StringType or len(_radio_str)<=0x00 or \
           _radio_filePath==None or type(_radio_filePath)!=types.StringType or len(_radio_filePath)<=0x00 or \
           _uuid==None or type(_uuid)!=types.StringType or len(_uuid)<=0x00 or \
           self.access_token==None or len(self.access_token)<=0x00:
           return -0x01

        #生成音频文件路径
        msha1 = hashlib.sha1()
        msha1.update(_radio_str)
        _radio_filePath = _radio_filePath + "/" + msha1.hexdigest() + ".mp3" 
        
        #组装发送报文
        post_data = {
            "tex" : _radio_str,
            "lan" : "zh",
            "tok" : self.access_token,
            "ctp" : "1",
            "cuid": _uuid
        }
        post_data = urllib.urlencode(post_data)

        try:
            #HTTP对话
            req = urllib2.Request(url=self.url_getRadio, data=post_data)
            res = urllib2.urlopen(req)
            content_type = res.info().getheader("Content-Type")
            recvData = res.read()

            #获取音频有误
            if content_type == "application/json":
                jsonData = json.loads(recvData, encoding="utf-8")
                
                return int(str(jsonData["err_no"]))
            
            #将获取到的音频存入指定文件
            f = open(_radio_filePath, "wb")
            f.write(recvData)
            f.close()

        except urllib2.HTTPError, e:    #服务器返回错误，例400 404
            return -0x01
        except urllib2.URLError, e:     #服务器地址有误
            return -0x02
        except Exception, e:            #未知错误
            return -0x03

        return 0x00

         



