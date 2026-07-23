#coding:utf-8

import os
import json
import mmap
import types
import time
import datetime
import threading
from baidu.baidu_tts import baidu_tts
from sys_server.db_redis import db_redis
import task.tsk_manager 

class task_baiduToken(threading.Thread):
    """
    函数名称：__init__
    函数功能：构造函数
    传入参数：_conf_path 配置文件路径
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-18
    """
    def __init__(self, _conf_path):
        threading.Thread.__init__(self)

        self.__conf_path = _conf_path                       #配置文件路径
        self.__tokenList = None                             #百度token控制列表

        self.__redis_conn = None

    """
    函数名称：checkInit
    函数功能：校验，初始化
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-18
    """
    def checkInit(self):
        #参数校验
        if self.__conf_path==None or type(self.__conf_path)!=types.StringType or len(self.__conf_path)<=0x00: return -0x01

        #读取配置文件，并且转换成json
        if (not os.path.exists(self.__conf_path)) or (not os.path.isfile(self.__conf_path)): return -0x02 
        
        confFile = open(self.__conf_path)
        self.__tokenList = json.load(confFile)
        confFile.close()

        #创建REDIS客户端
        self.__redis_conn = db_redis()
        if self.__redis_conn.connect(): 
            print "百度语音获取TOKEN [初始化，校验]：连接REDIS服务器失败"
            return -0x03

        for item in self.__tokenList:
            
            #创建语音对象
            item["cont"]  = baidu_tts(str(item["api_key"]), str(item["secret_key"]))
            if item["cont"].init_check(): print item["name"] + "：创建失败"; return -0x04

            #创建百度语音accessToken
            if item["cont"].get_accecssToken():
                self.__redis_conn.get_conn().set(
                    "baidu_tts_token_" + item["name"],
                    "null"
                )
            else:
                self.__redis_conn.get_conn().set(
                    "baidu_tts_token_" + item["name"],
                    item["cont"].getStr_accecssToken()
                )

            #加入时间戳
            item["date"] = datetime.datetime.now().strftime("%Y%m%d")

            

        return 0x00

    """
    函数名称：createToken
    函数功能：创建accessToken
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-18
    """
    def createToken(self):

        for item in self.__tokenList:
            #token更新条件判断
            if str(item["update_time"]) != datetime.datetime.now().strftime("%H"): continue     #未到更新时间
            if str(item["date"]) == datetime.datetime.now().strftime("%Y%m%d"): continue        #当日已经更新

            #判断redis是否连接
            if self.__redis_conn.isConnected() == False:
                while True: 
                    if self.__redis_conn.connect(): break
                    time.sleep(5)
            
            #创建语音对象
            item["cont"]  = baidu_tts(str(item["api_key"]), str(item["secret_key"]))
            if item["cont"].init_check(): print item["name"] + "：创建失败"; return -0x04

            #创建百度语音accessToken
            if item["cont"].get_accecssToken():
                self.__redis_conn.get_conn().set(
                    "baidu_tts_token_" + item["name"],
                    "null"
                )
            else:
                self.__redis_conn.get_conn().set(
                    "baidu_tts_token_" + item["name"],
                    item["cont"].getStr_accecssToken()
                )

            #加入时间戳
            item["date"] = datetime.datetime.now().strftime("%Y%m%d")


    """
    函数名称：run
    函数功能：线程函数
    传入参数：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-18
    """
    def run(self):
        try:
            task.tsk_manager.task_status.set_status_baiduToken()
            
            if self.checkInit(): self.__redis_conn.close(); raise

            while True:
                self.createToken()
                time.sleep(120)

        except Exception, e: pass
        finally:
            task.tsk_manager.task_status.clear_status_baiduToken()
