#!/usr/local//bin/python
#coding:utf-8

########################################################################################
#   文件简介：
#   脚本用于百度语音文件清理工作
########################################################################################


import os
import shutil
import types
import datetime

conf_timeout = 3
cont_dir = "/var/web/download"

class yunyin_clear:
    def __init__(self, _parentDir, _timeOut):
        self.__dirList = None
        self.__parentDir = _parentDir
        self.__nowDate_Num = None
        self.__timeOut = _timeOut

    def init(self):
        try:
            self.__nowDate_Num = int(datetime.datetime.now().strftime("%Y%m%d"))
            if self.__nowDate_Num == None or self.__nowDate_Num<=0x00: return -0x01
        except:
            return -0x02

        return 0x00

    #获取父目录下的所有子目录
    def get_dirList(self):
        #参数校验
        if self.__parentDir==None or type(self.__parentDir)!=types.StringType or len(self.__parentDir)<=0x00:
            self.__dirList = None
            return

        try: self.__dirList = os.listdir(self.__parentDir)
        except: self.__dirList = None

    #判断指定目录是否超时，并删除超时目录
    def __del_dir(self, _dirName):
        tmpList = _dirName.split("_")
        if len(tmpList)<=0x00: return

        try:
            if (self.__nowDate_Num - int(str(tmpList[-1]).split(".tar")[0])) >= self.__timeOut: 
                if os.path.isfile(self.__parentDir + "/" + _dirName): os.remove(self.__parentDir + "/" + _dirName)
		else: shutil.rmtree(self.__parentDir + "/" + _dirName)
        except:
            pass

    #工作函数
    def work(self):
        self.get_dirList()
        if self.__dirList==None or len(self.__dirList)<=0x00: return

        for item in self.__dirList: self.__del_dir(str(item))

    
if __name__ == '__main__':
    task = yunyin_clear(cont_dir, conf_timeout)
    if task.init(): exit()
    task.work()
