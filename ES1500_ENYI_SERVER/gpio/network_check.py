#coding:utf-8

#################################################################################
#   文件介绍
#   实时检测外网连接状态，并通过指示灯执行相应状态--连接：灯亮 端口：灯灭
#
#   检测方式
#   连接指定的网页域名，获取网页数据；根据数据的获取情况判断是否连接外网
################################################################################

import urllib2
import gpio
import types
import time

networkCheck_conf = {
    "work_cycle"                        : 60,                                           #检测周期
    "domain_list"                       :[                                              #域名列表
            "http://www.baidu.com",
            "http://www.qq.com",
            "http://www.360.com"
    ],
    "networkLed_gpio"                   : 200,                                          #IO口--网络指示灯
    "networkLed_open"                   : 1,                                            #开启--网络指示灯
    "networkLed_close"                  : 0                                             #关闭--网络指示灯
}

#网络指示灯操作类
class network_led:
    def __init__(self, _gpoi, _sta_open, _sta_close):
        self.__gpio = _gpoi                         #gpio管脚
        self.__sta_open = _sta_open                 #开启状态电平值
        self.__sta_close = _sta_close               #关闭状态电平值
        self.__status = 0                           #风机运行状态 1:开启 0:关闭

    """
    函数名称：init
    函数功能：初始化
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-06
    """
    def init(self):
        #参数校验
        if self.__gpio==None or type(self.__gpio)!=types.IntType or self.__gpio<=0x00: return -0x01
        if self.__sta_open==None or type(self.__sta_open)!=types.IntType or self.__sta_open not in [0, 1]: return -0x02
        if self.__sta_close == None or type(self.__sta_close)!=types.IntType or self.__sta_close not in [0, 1]: return -0x03
        if self.__sta_open == self.__sta_close: return -0x04

        #初始化GPIO口
        try: gpio.setup(self.__gpio, "out")
        except: return 0x01

        #关闭网络指示灯
        try: gpio.set(self.__gpio, self.__sta_close); self.__status = 0
        except: return 0x02

        return 0x00


    """
    函数名称：open
    函数功能：打开网络指示灯
    传入参数：无
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-06
    """
    def open(self):
        try: gpio.set(self.__gpio, self.__sta_open); self.__sta_close = 0x01
        except: return -0x01

        return 0x00

    """
    函数名称：close
    函数功能：关闭网络指示灯
    传入参数：无
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-06
    """
    def close(self):
        try: gpio.set(self.__gpio, self.__sta_close); self.__sta_close = 0x00
        except: return -0x01

        return 0x00

    """
    函数名称：get_status
    函数功能：获取网络指示能状态
    传入参数：无
    传出数据：1 开启 0 关闭
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-06
    """
    def get_status(self): return self.__status

###########################################################################################

#网络检测类
class network_check:
    def __init__(self, _domain_list):
        self.__domain_list = _domain_list                           #域名列表

    """
    函数名称：init
    函数功能：初始化
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-06
    """
    def init(self):
        if self.__domain_list==None or type(self.__domain_list)!=types.ListType or len(self.__domain_list)<=0x00: return -0x01
        for item in self.__domain_list:
            if type(item)!=types.StringType or len(item)<=0x00: return -0x02

        return 0x00

    """
    函数名称：__check_url
    函数功能：访问指定网站域名
    传入参数：_url
    传出数据：
                 0  运行成功
                -1  域名访问有误
                -2  HTTP会话返回数据为空
    注意事项：内部函数
    编写人员：王凤龙
    编写时间：2017-05-06
    """
    def __check_url(self, _url):
        recvData = ""                                           #HTTP会话返回数据

        #访问域名
        try:
            req = urllib2.Request(_url)
            res = urllib2.urlopen(req)
            recvData = res.read()
        except: return -0x01

        #检测返回数据
        if str(recvData) <= 0x00: return -0x02

        return 0x00

    """
    函数名称：connect_check
    函数功能：检测外网连接
    传入参数：无
    传出数据：0 网络连接 非0 网络断开
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-06
    """
    def connect_check(self):
        for item in self.__domain_list:
            if self.__check_url(item) == 0: return 0x00

        return 0x01

###########################################################################################

#网络检测，控制指示灯显示状态
class manager_netLed:
    def __init__(self):
        self.__net_check = None                             #网络检测器
        self.__net_led = None                               #网络指示灯
        self.__cyc = None                                   #检测周期-秒

    """
    函数名称：init
    函数功能：初始化
    传入参数：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-06
    """
    def init(self):
        #创建并初始化网络检测器
        self.__net_check = network_check(networkCheck_conf["domain_list"])
        if self.__net_check.init(): return -0x01

        #创建并初始化网络指示灯
        self.__net_led = network_led(
            networkCheck_conf["networkLed_gpio"],
            networkCheck_conf["networkLed_open"],
            networkCheck_conf["networkLed_close"]
        )
        if self.__net_led.init(): return -0x02

        #设置检测周期
        self.__cyc = networkCheck_conf["work_cycle"]
        if self.__cyc==None or type(self.__cyc)!=types.IntType or self.__cyc<=0x00: return -0x03

        return 0x00

    """
    函数名称：contrl
    函数功能：控制器，一个工作周期
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-06
    """
    def contrl(self):
        #检测网络连接
        if self.__net_check.connect_check() == 0:                           #网络连接
            if self.__net_led.get_status() == 0:                            #开启指示灯
                if self.__net_led.open(): return 0x01

        else:
            if self.__net_led.get_status() == 1:                            #关闭指示灯
                if self.__net_led.close(): return 0x02

        print "网络正常..."
        return 0x00

    """
    函数名称：task
    函数功能：任务函数
    传入参数：无
    传出数据：无
    注意实行：无
    编写人员：王凤龙
    编写时间：2017-05-06
    """
    def task(self):
        while True:
            if self.contrl(): exit()
            time.sleep(self.__cyc)

###########################################################################################

if __name__ == '__main__':
    networkLed_task = manager_netLed()

    if networkLed_task.init():
        print "网络指示灯控制器初始化失败..."
        exit()

    print "开始工作"
    networkLed_task.task()