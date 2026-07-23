#coding:utf-8

#########################################################################
#   线程管理器，线程状态采集
#########################################################################

import threading
import time
from conf.config import conf
from task.tsk_baiduToken import task_baiduToken

#########################################################################
#   线程管理器
#########################################################################
class task_manager(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        
        self.thread_baiduToken = None                           #百度token获取线程

    def run(self):
        print "系统开始运行"

        try:
            
            while True:
                #百度语音获取线程
                if task_status.get_status_baiduToken() == 0:
                    try:
                        self.thread_baiduToken = task_baiduToken(conf.CONF_PATH)
                        self.thread_baiduToken.start()
                    except: pass

                time.sleep(conf.SYS_CHECK_TIMEOUT)
        
        except Exception, e:
            print e
            print "系统异常"
        finally:
            print "系统退出！"


#########################################################################
#   线程状态采集及设置
#########################################################################
class task_status:
    __mylock = threading.RLock()

    """
    百度语音ACCESS_TOKEN设置及获取
    """
    __status_baiduToken = 0x00
    @staticmethod
    def set_status_baiduToken():
        task_status.__mylock.acquire()
        task_status.__status_baiduToken = 0x01
        task_status.__mylock.release()

    @staticmethod
    def get_status_baiduToken():
        task_status.__mylock.acquire()
        temp = task_status.__status_baiduToken
        task_status.__mylock.release()
        return temp

    @staticmethod
    def clear_status_baiduToken():
        task_status.__mylock.acquire()
        task_status.__status_baiduToken = 0x00
        task_status.__mylock.release()
