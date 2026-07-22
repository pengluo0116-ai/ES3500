#coding:utf-8

import redis
from conf.config import conf

class db_redis:
    def __init__(self):
        self.__sip = conf.REDIS_SERVER_IP                   #IP地址
        self.__sport = conf.REDIS_SERVER_PORT               #端口
        self.__client = None                                #客户端

    """
    函数名称：connect
    函数功能：连接服务器
    传入参数：无
    传出数据：0 连接服务器成功 非0 连接服务器失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-19
    """
    def connect(self):
        self.close()

        err = 0x00
        try: self.__client = redis.Redis(host=self.__sip, port=self.__sport)
        except Exception, e: self.__client = None; err = 0x01

        return err

    """
    函数名称：close
    函数功能：关闭服务器连接
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-19
    """
    def close(self):
        if self.__client != None:
            try: self.__client.shutdown()
            except: pass
            finally: self.__client = None
    
    """
    函数名称：isConnected
    函数功能：校验连接是否存在
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-19
    """
    def isConnected(self):
        if self.__client == None: return False

        try:
            if self.__client.ping(): return True
            else: raise
        except:
            self.close()
            return False

        
    """
    函数名称：get_conn
    函数功能：获取客户端连接
    传入参数：无
    传出数据：None 返回失败 非None 返回成功
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-19
    """
    def get_conn(self):
        return self.__client