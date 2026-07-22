#coding:utf-8

#################################################################################
#   文件介绍
#   实时检测CPU温度，当CPU温度高于设定值时，开启风扇；当CPU温度低于设定值时，关闭风扇
################################################################################

import types
import gpio
import time

cpu_fan_conf = {
    "work_cycle"                        : 60,                                           #控制周期--秒
    "cpu_temperature_infoFile"          : "/sys/class/thermal/thermal_zone0/temp",      #温度获取文件
    "cpu_temperature_high"              : 70,                                           #阈值--CPU最高温度
    "cpu_temperature_low"               : 50,                                           #阈值--CPU最低温度
    "fan_gpio"                          : 19,                                           #IO口--风机
    "fan_open"                          : 1,                                            #开启--风机 高电平
    "fan_close"                         : 0                                             #关闭--风机 低电平
}

#风机GPIO操作类
class gpio_fan:
    def __init__(self, _gpio, _sta_open, _sta_close):
        self.__gpio = _gpio                         #gpio管脚
        self.__sta_open = _sta_open                 #开启状态电平值
        self.__sta_close = _sta_close               #关闭状态电平值
        self.__status = 0                           #风机运行状态 1:开启 0:关闭
    
    """
    函数名称：init
    函数功能：初始化函数
    传入参数：无
    传出数据：
                 0  运行成功
                -1  设定值有误--GPIO管脚
                -2  设定值有误--风机开启状态
                -3  设定值有误--风机关闭状态
                -4  风机开启，关闭状态设置值一致
                 1  初始化GPIO管脚失败
                 2  关闭风机失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-05
    """
    def init(self):
        #参数校验
        if self.__gpio==None or type(self.__gpio)!=types.IntType or self.__gpio<=0x00: return -0x01
        if self.__sta_open==None or type(self.__sta_open)!=types.IntType or self.__sta_open not in [0, 1]: return -0x02
        if self.__sta_close==None or type(self.__sta_close)!=types.IntType or self.__sta_close not in [0, 1]: return -0x03
        if self.__sta_open == self.__sta_close: return -0x04

        #初始化GPIO引脚
        try: gpio.setup(self.__gpio, "out")
        except: return 0x01

        #关闭风机
        try: gpio.set(self.__gpio, self.__sta_close); self.__status = 0x00
        except: return 0x02

        return 0x00

    """
    函数名称：open
    函数功能：打开风机
    传入参数：无
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写世家：2017-05-05
    """
    def open(self):
        try: gpio.set(self.__gpio, self.__sta_open); self.__status = 0x01
        except: return -0x01

        return 0x00

    """
    函数名称：close
    函数功能：关闭风机
    传入参数：无
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-05
    """
    def close(self):
        try: gpio.set(self.__gpio, self.__sta_close); self.__status = 0x00
        except: return -0x01

        return 0x00

    """
    函数名称：get_status
    函数功能：获取风机当前运行状态
    传入参数：无
    传出数据：1 开启 0 关闭
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-05
    """
    def get_status(self): return self.__status

###########################################################################################

#CPU温度获取
class cpu_tmperature:
    def __init__(self, _infoFile):
        self.__tmperature = 0                   #当前CPU温度
        self.__infoFile = _infoFile             #温度获取配置文件路径

    """
    函数名称：init
    函数功能：初始化函数
    传入参数：无
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-05
    """
    def init(self):
        if self.__infoFile==None or type(self.__infoFile)!=types.StringType or len(self.__infoFile)<=0x00: return -0x01
        return 0x00

    
    """
    函数名称：get_temperrture
    函数功能：获取CPU温度
    传入参数：无
    传出数据：0  运行成功 非0 运行失败
    注意事项：从系统文件中获取，返回的值只标示是否获取成功，不是实际温度
    编写人员：王凤龙
    编写时间：2017-05-05
    """
    def get_temperature(self):
        tfile = None

        #读取配置信息
        try: tfile = open(self.__infoFile)
        except: return -0x01

        cputmp = str(tfile.read())
        tfile.close()

        #转换数据
        self.__tmperature = int(cputmp)
        return 0x00

    """
    函数名称：temper
    函数功能：获取当前温度数值
    传入参数：无
    传出数据：温度值
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-05
    """
    def temper(self): return self.__tmperature

###########################################################################################

#CPU，风机控制器
class manager_cpuFan:
    def __init__(self):
        self.cpu_tmp = None                                 #CPU温度获取器
        self.fan = None                                     #风机控制器
        self.tmp_high = None                                #温度阈值--CPU最高温度
        self.tmp_low = None                                 #温度阈值--CPU最低温度
        self.cyc = None                                     #检测周期--秒

    """
    函数名称：init
    函数功能：初始化函数
    传入参数：无
    传出数据：
                 0  运行成功
                -1  初始化风机失败
                -2  初始化CPU温度获取器失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-05
    """
    def init(self):
        #创建并初始化风机
        self.fan = gpio_fan(
            cpu_fan_conf["fan_gpio"],
            cpu_fan_conf["fan_open"],
            cpu_fan_conf["fan_close"]
        )
        if self.fan.init(): return -0x01

        #创建并初始化cpu温度获取器
        self.cpu_tmp = cpu_tmperature(cpu_fan_conf["cpu_temperature_infoFile"])
        if self.cpu_tmp.init(): return -0x02

        #温度阈值设置
        self.tmp_high = cpu_fan_conf["cpu_temperature_high"]
        self.tmp_low = cpu_fan_conf["cpu_temperature_low"]

        #检测周期设置
        self.cyc = cpu_fan_conf["work_cycle"]

        return 0x00

    """
    函数名称：contrl
    函数功能：控制器，一个工作周期
    传入参数：无
    传出数据：
                0   运行正确
                1   风机开启失败
                2   风机关闭失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-05
    """
    def contrl(self):
        #获取CPU温度
        if self.cpu_tmp.get_temperature():
            print "从系统中获取温度失败"
            reurn -0x01
        
        #数据判断并进行风机控制
        if self.cpu_tmp.temper() >= self.tmp_high:        #温度过高，开启风机
            if self.fan.open(): print "风机开始失败"; return 0x01
            else: return 0x00

        if self.cpu_tmp.temper() <= self.tmp_low:         #温度过低，关闭风机
            if self.fan.close(): print "风机关闭失败"; return 0x02
            else: return 0x00

        return 0x00

    """
    函数名称：task
    函数功能：任务函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-05
    """
    def task(self):
        while True:
            if self.contrl(): exit()
            time.sleep(self.cyc)

###########################################################################################

if __name__ == '__main__':
    cpu_fan_task = manager_cpuFan()
    if cpu_fan_task.init():
        print "CPU温度风速控制器初始化失败..."
        exit()

    print "开始工作"
    cpu_fan_task.task()
