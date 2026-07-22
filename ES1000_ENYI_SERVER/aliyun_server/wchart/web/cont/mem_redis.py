#coding=utf-8

#########################################################################
#   文档简介
#   redis缓存操作，用户获取微信token
#########################################################################

import redis
import threading
from conf.config import conf

class mem_redis:
    __mylock        = threading.RLock()             #全局锁
    __rclient       = None                          #redis客户端
    __ip            = conf.REDIS_SERVER_IP          #redis服务器IP
    __port          = conf.REDIS_SERVER_PORT        #redis服务器端口
    __wkey_token    = conf.REDIS_WCHART_TOKEN_KEY   #redis中Token的key

    '''
    函数名称：__keepConneted
    函数功能：连接保持
    传入参数：无
    传出数据：True 运行成功 False 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-08-08
    '''
    @staticmethod
    def __keepConneted():
        err = True
        #判断连接是否保持
        mem_redis.__mylock.acquire()
        try: 
            if mem_redis.__rclient.ping(): err = True
        except: err = False
        mem_redis.__mylock.release()
        if err: return True

        #创建连接
        print "创建缓存..."
        mem_redis.__mylock.acquire()
        try: mem_redis.__rclient.shutdown()
        except: pass

        try: 
            mem_redis.__rclient = redis.StrictRedis(host= mem_redis.__ip, port=mem_redis.__port, db=0)
            if mem_redis.__rclient.ping(): err = True
        except: err = False

        mem_redis.__mylock.release()
        return err

    '''
    函数名称：get_wchartToken
    函数功能：获取微信Token
    传入参数：无
    传出数据：字符串类型的token串 None 未获取到
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-08-08
    '''
    @staticmethod
    def get_wchartToken():
        if not mem_redis.__keepConneted(): return None
        token = None
        try:
            token = mem_redis.__rclient.get(mem_redis.__wkey_token)
        except:
            token = None

        return token
