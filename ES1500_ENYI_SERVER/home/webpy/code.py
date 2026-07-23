#!/usr/bin/python
#coding=utf-8
import web
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import json
import time
import threading
import sys
import hashlib
import random
import redis
import sqlite3

reload(sys)
sys.setdefaultencoding('utf8')

############### 创建mqtt客户端 #################
client = mqtt.Client()
client.connect('127.0.0.1', port=1883)
t = threading.Thread(target=client.loop_forever)
t.start()

class MqttPublish():
# 接收终端发来的数据，并将其转发到mqtt broker
    @staticmethod
    def send_terminal_data(recv, topic):
        client.publish(topic, payload=recv)

###############创建redies客户端##################
redisClient = redis.StrictRedis(host='127.0.0.1', port=6379, db=0)

########## web服务器，用于接收解析数据 ##########
urls = (
    '/', 'Hello'
)
app = web.application(urls, globals())
application = app.wsgifunc()


'''
函数名称：updateDatabase
功能：更新数据库
编写人：周广阔      时间：2020-12-28    
传入参数：jsonBase 要存的数据库信息
返回值:无   注意事项：无
'''
def updateDatabase(ID,jsonBase):
    times = jsonBase['mainter_time']
    strBase = json.dumps(jsonBase)
    redisClient.set('d_' + ID,strBase)  # 更新redis

    db_dev = sqlite3.connect('/root/model/DeviceInfo.db') #更新数据库
    db_dev.execute("UPDATE dev_info SET mainter_time=%s WHERE id='%s'" %(jsonBase['mainter_time'], ID))
    db_dev.commit()
    db_dev.close()

'''
函数名称：analyticalData 
功能：解析Es1000设备发来的实时数据
编写人：周广阔      时间：2020-12-16    
传入参数：jsondata 客户端发来的数据
返回值:无   注意事项：无
'''
def analyticalData(jsondata):
    try:
        Parameters      = jsondata.get('Parameters')
        Alarm           = jsondata.get('Alarm')
        DT_State        = jsondata.get('State')

        Version         = Parameters.get('Version')
        if Version == "ES1000_V2.0":
            ParametersTmp = {}
            AlarmTmp = {}
            StateTmp = {}

            ParametersTmp['ID'] = Parameters['Id']

            AlarmTmp['Y_N'] = Alarm['HasAlarm']
            AlarmTmp['Kaceng'] = Alarm['Stuck']
            AlarmTmp['Kunren'] = Alarm['PersonTrapped']
            AlarmTmp['UP_Alarm'] = Alarm['UpAlarm']
            AlarmTmp['Down_Alarm'] = Alarm['DownAlarm']
            AlarmTmp['Run_door'] = Alarm['RunDoor']
            AlarmTmp['Speeding'] = Alarm['Speed']
            AlarmTmp['Stp_DoorUnopen'] = Alarm['StpDoorUnOpen']
            AlarmTmp['Stp_DoorUnclose'] = Alarm['StpDoorUnClose']
            AlarmTmp['Stp_Reapet'] = Alarm['StpDoorRepeat']
            AlarmTmp['Slant_lr'] = Alarm['SlantLr']
            AlarmTmp['Slant_fb'] = Alarm['SlantFb']
            AlarmTmp['Amplitude'] = Alarm['Amplitude']
            AlarmTmp['Manual_Alarm'] = Alarm['ManualAlarm']
            AlarmTmp['Electromobile'] = Alarm['Ebike']

            StateTmp['DS_DX'] = "1"
            StateTmp['Lou_Ceng'] = DT_State['Floor']
            StateTmp['status'] = DT_State['Status']
            StateTmp['Door'] = DT_State['Door']
            StateTmp['People'] = DT_State['People']
            StateTmp['Speed'] = DT_State['Speed']
            StateTmp['Degree_lr'] = DT_State['DegreeLr']
            StateTmp['Degree_fb'] = DT_State['DegreeFb']
            StateTmp['Hum'] = DT_State['Hum']
            StateTmp['Temp'] = DT_State['Temp']
            StateTmp['Amplitude'] = DT_State['Amplitude']
            StateTmp['Maintain'] = DT_State['Maintain']

            Alarm = AlarmTmp
            jsondata['Alarm'] = AlarmTmp
            Parameters = ParametersTmp
            jsondata['Parameters'] = ParametersTmp
            DT_State = StateTmp
            jsondata['State'] = StateTmp

        '''paremeters'''
        DAQ_ID          = Parameters.get('ID')

        '''alarm'''
        Alarm_YN        = Alarm.get('Y_N')              #是否报警
        Kaceng          = Alarm.get('Kaceng')           #卡层
        Kunren          = Alarm.get('Kunren')           #困人
        UP_Alarm        = Alarm.get('UP_Alarm')         #冲顶
        Down_Alarm      = Alarm.get('Down_Alarm')       #蹲底
        Run_door        = Alarm.get('Run_door')         #开门走梯
        Speeding        = Alarm.get('Speeding')         #超速
        Power           = Alarm.get('Power')            #断电
        Stp_DoorUnopen  = Alarm.get('Stp_DoorUnopen')   #停梯不开门
        Stp_DoorUnclose = Alarm.get('Stp_DoorUnclose')  #停梯不关门
        Stp_Reapet      = Alarm.get('Stp_Reapet')       #重复开关门
        Slant_lr        = Alarm.get('Slant_lr')         #电梯左右倾斜
        Slant_fb        = Alarm.get('Slant_fb')         #电梯前后倾斜
        Amplitude       = Alarm.get('Amplitude')        #电梯震动
        Manual_Alarm    = Alarm.get('Manual_Alarm')     #手动报警
        Electromobile   = Alarm.get('Electromobile')    #电动车入梯
        Voltage         = Alarm.get('Voltage')          #电池电量低
        Power_Down      = Alarm.get('Power_Down')       #外部掉电Manaual_Alarm

        '''DT_State'''
        DS_DX           = DT_State.get('DS_DX')         #地上地下
        Lou_Ceng        = DT_State.get('Lou_Ceng')      #楼层数
        status          = DT_State.get('status')        #上下行停止
        Door            = DT_State.get('Door')          #开关门
        People          = DT_State.get('People')        #有无人
        G_X             = DT_State.get('G_X')           #待定
        G_Y             = DT_State.get('G_Y')           #待定
        G_Z             = DT_State.get('G_Z')           #待定
        Name            = DT_State.get('OSD')           #待定  
        Speed           = DT_State.get('Speed')         #速度
        Degree_lr       = DT_State.get('Degree_lr')     #左右偏移角度
        Degree_fb       = DT_State.get('Degree_fb')     #前后偏移角度
        Hum             = DT_State.get('Hum')           #温度
        Temp            = DT_State.get('Temp')          #湿度
        Amplitude       = DT_State.get('Amplitude')     #电梯振幅
        Voltage         = DT_State.get('Voltage')       #电池电量
        Maintain        = DT_State.get('Maintain')      #维保状态

        '''计算接收时间'''
        recv_time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
        Parameters['Recv_time'] = recv_time             #数据接收时间

        '''维保判断'''
        jsondata['Alarm']['Maintain'] = '0'      #维保报警的初始值为 '0'
        jsondata['State']['MainTime'] = '0-0-0 0:0:0'
        devDataAsString = redisClient.get('d_'+ DAQ_ID)   #获取设备数据
        
        if devDataAsString is not None:
            devDataAsJson = json.loads(devDataAsString)

            # print int(devDataAsJson['mainter_period'])
            if devDataAsJson['mainter_time'] == "0":
                devDataAsJson['mainter_time'] = str(int(time.time()))
                updateDatabase(DAQ_ID,devDataAsJson)

            if False: #Maintain == '1':  #维保触发,更改redis数据
                print(int(time.time()))
                print(int(eval(devDataAsJson['mainter_time'])))
                if (int(time.time()) - int(eval(devDataAsJson['mainter_time']))) >= 30:
                    devDataAsJson['mainter_time'] = str(int(time.time()))
                    updateDatabase(DAQ_ID,devDataAsJson)
            else:
                if int(devDataAsJson['mainter_period']) != 0:
                    if (int(time.time()) - int(devDataAsJson['mainter_time']))/60/60/24 >= int(devDataAsJson['mainter_period']): #判断是否超过维保周期
                        jsondata['Alarm']['Maintain'] = '1'
                        jsondata['Alarm']['Y_N'] = '1'
            
            jsondata['State']['MainTime'] = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(int(devDataAsJson['mainter_time'])))
        else:
            print 'error'
        
        recv = json.dumps(jsondata)
        # print recv
        if Alarm_YN == "1":
            topic = "TerminalData" + "/" + DAQ_ID + "/" + "real_alarm"
        else:
            topic = "TerminalData" + "/" + DAQ_ID + "/" + "real_ok"
        print topic
        MqttPublish.send_terminal_data(recv, topic)
        return 0

    except:
        f = open("/home/webpy/except.log", "a")
        f.write(str(time.time()))
        f.close()
        print "The format of json is illegal!!!"
        return -1

class Hello:
    def POST(self):
        recv = web.data()
     #   f = open("/home/webpy/log2.txt", "a")
     #   f.write(recv)
     #   f.close()
        jsondata = json.loads(recv)
        if analyticalData(jsondata):
            return 'error'
        return 'ok'

if __name__ == "__main__":
    app.run()
