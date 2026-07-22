#coding=utf-8

import web
import json
import types
from conf.config import conf
from cont.mem_redis import mem_redis
from cont.wchart import wchart
from conf.config import WEB_ERR_CODE

class Wt_liftalarm:
    def __init__(self):
        self.__responseStr      = None      #上报数据-字符串
        self.__responseJson     = None      #上报数据-JSON
        self.__alarmStr         = None      #报警内容
        self.__alarmDate        = None      #报警时间
        
        self.__json_AlarmInfo   = None      #报警内容json
        self.__json_wchartInfo  = None      #推送用户信息列表
        self.__deviceID         = None      #设备ID
    
    def GET(self):
        return WEB_ERR_CODE["METHOD_ERR"]
    
    def POST(self):
        #获取上报数据
        self.__responseStr = web.data()
        if self.__responseStr==None or type(self.__responseStr)!=types.StringType or len(self.__responseStr)<=0x00: 
            return WEB_ERR_CODE["REQUEST_DATA_NONE"]

        #上报数据解析成json
        try: self.__responseJson = json.loads(self.__responseStr, encoding="utf-8")
        except: return  WEB_ERR_CODE["REQUEST_DATA_ERR"]

        #提取客户端信息
        try: self.__getInfo()
        except: return WEB_ERR_CODE["REQUEST_DATA_NOFULL"]

        #生成报警内容
        if self.__create_alarmStr() <= 0x00: return  WEB_ERR_CODE["REQUEST_DATA_NOFULL"]

        #发送报警信息
        try: 
            if self.__send_alarmMsg(): return WEB_ERR_CODE["SEND_WCHART_ERROR"]
        except: return WEB_ERR_CODE["SEND_WCHART_EXCEPT"]

        return WEB_ERR_CODE["NONE_ERR"]

    '''
    函数名称：__getInfo
    函数功能：提取客户端上报信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-08-08
    '''
    def __getInfo(self):
        #设备实时信息--报警
        self.__json_AlarmInfo = json.loads(self.__responseJson["alarm_info"], encoding="utf-8")

        #微信发送列表
        self.__json_wchartInfo = self.__responseJson["wx_info"]

        #设备ID
        self.__deviceID = self.__responseJson["SN"]

    '''
    函数名称：__create_alarmStr
    函数功能：生成报警内容字符串
    传入参数：无
    传出数据：
                > 0 报警内容的个数
                 -1 运行有误
                ==0 无报警项
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-08-08
    '''
    def __create_alarmStr(self):
        alarmList = []
        try:
            _jsonAlarm = self.__json_AlarmInfo["Alarm"]

            if _jsonAlarm["Power"]          == "1" : alarmList.append("掉电")
            if _jsonAlarm["UP_Alarm"]       == "1" : alarmList.append("冲顶")
            if _jsonAlarm["Down_Alarm"]     == "1" : alarmList.append("蹲底")
            if _jsonAlarm["Kunren"]         == "1" : alarmList.append("困人")
            if _jsonAlarm["Speeding"]       == "1" : alarmList.append("超速")
            if _jsonAlarm["Run_door"]       == "1" : alarmList.append("开门走梯")
            if _jsonAlarm["Kaceng"]         == "1" : alarmList.append("卡层")
        
            #版本二
            if "Stp_DoorUnopen" in _jsonAlarm : 
                if _jsonAlarm["Stp_DoorUnopen"] == "1" : alarmList.append("停梯不开门")

            if "Stp_DoorUnclose" in _jsonAlarm :
                if _jsonAlarm["Stp_DoorUnclose"] == "1" : alarmList.append("停梯不关门")

            if "Stp_Reapet" in _jsonAlarm :
                if _jsonAlarm["Stp_Reapet"] == "1" : alarmList.append("重复开关门")
        
        except: return -0x01

        self.__alarmStr = ", ".join(alarmList)
        return alarmList.count
    
    '''
    函数名称：__send_alarmMsg
    函数功能：发送报警信息
    传入参数：无
    传出数据：0 运行正确 非0 运行错误
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-08-08
    '''
    def __send_alarmMsg(self):
        if self.__json_wchartInfo.count <= 0x00: return -0x01

        #创建微信推送客户端
        mwchart = wchart(conf.WCHART_SERVER_URL, conf.WCHART_SERVER_APPID, conf.WCHART_SERVER_APPSECRET, conf.WCHART_DEVICE_URL)
        if mwchart.init(): return -0x02

        #组装发送数据包
        postBody = {}
        postBody["first"]       = {"value": "电梯报警", "color": "#FF0000"}                                                      # 标题
        postBody["keyword1"]    = {"value": "系统报警", "color": "#000000"}                                                      # 故障用户
        postBody["keyword2"]    = {"value": self.__json_AlarmInfo["Parameters"]["Recv_time"], "color": "#000000"}               # 故障时间
        postBody["keyword3"]    = {"value": self.__alarmStr, "color": "#000000"}                                                # 故障现象
        postBody["remark"]      = {"value": "电梯地址：" + self.__json_AlarmInfo["Parameters"]["dev_name"], "color": "#000000"}  # 故障电梯名

        #推送报警信息到用户
        token = None
        for item in self.__json_wchartInfo:
            
            #获取token
            token = mem_redis.get_wchartToken()
            if token == None: return -0x03

            #推送信息
            errmsg = mwchart.sendModeMsg(
                item["appid"],
                token,
                conf.WCHART_MSGMODE_ID,
                "",
                postBody
            )
            if errmsg["error"]: return -0x04

        return 0x00