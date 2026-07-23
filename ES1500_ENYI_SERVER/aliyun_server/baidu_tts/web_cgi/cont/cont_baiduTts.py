#coding:utf-8

import os
import threading
import json
import time
from conf.config import conf
from baidu.baidu_tts import baidu_tts
from sys_server.db_redis import db_redis

class cont_baiduTts:
    __mylock = threading.RLock()                                #全局锁
    __redis_client = None                                       #REDIS客户端
    __conf_path = None                                          #配置文件路径
    __tokenList = None                                          #百度token控制列表
    __pid = None

    """
    函数名称：check_init
    函数功能：校验初始化
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-19
    """
    @staticmethod
    def check_init():
        #获取进程ID
        cont_baiduTts.__pid = str(os.getpid())
        
        #连接REDIS服务器
        cont_baiduTts.__redis_client = db_redis()
        if cont_baiduTts.__redis_client.connect():
            print "百度语音生成模块 [初始化，校验]：连接REDIS失败"
            return -0x01

        #读取配置文件，并且转换成json
        cont_baiduTts.__conf_path = conf.CONF_PATH
        if (not os.path.exists(cont_baiduTts.__conf_path)) or (not os.path.isfile(cont_baiduTts.__conf_path)): return -0x02

        confFile = open(cont_baiduTts.__conf_path)
        cont_baiduTts.__tokenList = json.load(confFile)
        confFile.close()

        #创建语音对象
        for item in cont_baiduTts.__tokenList:
            item["cont"] = baidu_tts(str(item["api_key"]), str(item["secret_key"]))
            if item["cont"].init_check(): print item["name"] + "：创建失败"; return -0x04

        return 0x00

    """
    函数名称：create_baiduTTS
    函数功能：创建百度语音
    传入参数：
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-20
    """
    @staticmethod
    def create_baiduTTS(_radio_str, _radio_filePath):
        #判断REDIS是否连接
        cont_baiduTts.__mylock.acquire()
        tmp_isconned = cont_baiduTts.__redis_client.isConnected()
        cont_baiduTts.__mylock.release()

        if tmp_isconned == False:
            for i in range(0, 10):
                cont_baiduTts.__mylock.acquire()
                tmp_conn = cont_baiduTts.__redis_client.connect()
                cont_baiduTts.__mylock.release()

                if tmp_conn == 0: break
                time.sleep(1)
            else: return -0x01
        
        #获取百度语音
        for item in cont_baiduTts.__tokenList:
            #从redis读取百度ACCESS_TOKEN
            rd_key = "baidu_tts_token_" + item["name"]
            rd_val = "null"
            
            cont_baiduTts.__mylock.acquire()
            try: rd_val = cont_baiduTts.__redis_client.get_conn().get(rd_key)
            except: rd_val = "null"
            cont_baiduTts.__mylock.release()
            if rd_val == "null": continue

            #配置百度ACCESS_TOKEN
            if item["cont"].setStr_accessToken(rd_val): continue
            
            #生成百度语音
            if item["cont"].get_baiduRadio(_radio_str, _radio_filePath, cont_baiduTts.__pid): continue
                
            return 0x00
        
        return -0x02