#!/usr/bin/python
#coding=utf-8

import time
import json
import redis
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import sys
reload(sys)
sys.setdefaultencoding('utf8')

key_alive_time = 1800   # r_alarm中键的生存时间，单位为秒
r = redis.StrictRedis(host='127.0.0.1', port=6379, db=0)

def on_connect(client, userdata, flags, rc):
    print 'Connected with result code' + str(rc)
    client.subscribe('TerminalData/#')

def on_message(client, userdata, msg):
    print msg.topic + ' ' + str(msg.payload)
    rt_process(msg.payload)

def rt_process(recv):
    try:
        hjson           = json.loads(recv)

        Parameters      = hjson.get('Parameters')
        Alarm           = hjson.get('Alarm')
        DT_State        = hjson.get('State')

        DAQ_ID          = Parameters.get('ID')
        Recv_time       = Parameters.get('Recv_time')

        Alarm_YN        = Alarm.get('Y_N')
        Kaceng          = Alarm.get('Kaceng')
        Kunren          = Alarm.get('Kunren')
        UP_Alarm        = Alarm.get('UP_Alarm')
        Down_Alarm      = Alarm.get('Down_Alarm')
        Run_door        = Alarm.get('Run_door')
        Speeding        = Alarm.get('Speeding')
        Power           = Alarm.get('Power')
        Stp_DoorUnopen  = Alarm.get('Stp_DoorUnopen')
        Stp_DoorUnclose = Alarm.get('Stp_DoorUnclose')
        Stp_Reapet      = Alarm.get('Stp_Reapet')
        Slant_lr        = Alarm.get('Slant_lr')
        Slant_fb        = Alarm.get('Slant_fb')
        Amplitude       = Alarm.get('Amplitude')
        Manaual_Alarm   = Alarm.get('Manual_Alarm')
        Electromobile   = Alarm.get('Electromobile')
        Voltage         = Alarm.get('Voltage')
        Power_Down      = Alarm.get('Power_Down')
        MainTain        = Alarm.get('Maintain')

        DS_DX           = DT_State.get('DS_DX')
        Lou_Ceng        = DT_State.get('Lou_Ceng')
        status          = DT_State.get('status')
        Door            = DT_State.get('Door')
        People          = DT_State.get('People')
        Speed           = DT_State.get('Speed')
        Degree_lr       = DT_State.get('Degree_lr')
        Degree_fb       = DT_State.get('Degree_fb')
        Hum             = DT_State.get('Hum')
        Temp            = DT_State.get('Temp')
        Maintain        = DT_State.get('Maintain')
        G_X             = DT_State.get('G_X')
        G_Y             = DT_State.get('G_Y')
        G_Z             = DT_State.get('G_Z')
        Name            = DT_State.get('OSD')

        # if DAQ_ID in mac_list:  # 使用in判断ID是否为合法ID
        if r.get('d' + '_' + DAQ_ID) is not None:
            if Alarm_YN == '1':  # 判断是否存在报警信息
                last_data = r.get('a' + '_' + DAQ_ID)

                if last_data is None:   # Redis里暂无该ID对应的key值，直接写入报警信息即可
                    r.set('r' + '_' + DAQ_ID, recv)
                    r.set('a' + '_' + DAQ_ID, recv, ex=key_alive_time)
                    try:
                        publish.single("alarm", payload=recv, hostname="127.0.0.1")
                    except:
                        print "MQTT publish error"
                    print "The last data is None!"

                else:
                    # 解析Redis目前存储的json字符串
                    last_json = json.loads(last_data)
                    last_alarm = last_json.get('Alarm')

                    # 如果上述条件不满足，则这一次和上一次信息均包含报警信息，需判断两次报警信息是否一致
                    last_alarm_str = '''Kaceng=%s,Kunren=%s,UP_Alarm=%s,Down_Alarm=%s,Run_door=%s,Speeding=%s,Power=%s,Stp_DoorUnopen=%s,Stp_DoorUnclose=%s,Stp_Reapet=%s,Slant_lr=%s,
                                        Slant_fb=%s,Amplitude=%s,Electromobile=%s,Voltage=%s,Power_Down=%s,Manual_Alarm=%s,Maintain=%s''' %\
                                            (last_alarm.get('Kaceng'), last_alarm.get('Kunren'), last_alarm.get('UP_Alarm'), last_alarm.get('Down_Alarm'), last_alarm.get('Run_door'), last_alarm.get('Speeding'),\
                                            last_alarm.get('Power'), last_alarm.get('Stp_DoorUnopen'), last_alarm.get('Stp_DoorUnclose'), last_alarm.get('Stp_Reapet'), last_alarm.get("Slant_lr"),\
                                            last_alarm.get('Slant_fb'),last_alarm.get('Amplitude'), last_alarm.get('Electromobile'), last_alarm.get('Voltage'),last_alarm.get('Power_Down'),last_alarm.get('Manual_Alarm'),last_alarm.get('Maintain'))

                    now_alarm_str =  '''Kaceng=%s,Kunren=%s,UP_Alarm=%s,Down_Alarm=%s,Run_door=%s,Speeding=%s,Power=%s,Stp_DoorUnopen=%s,Stp_DoorUnclose=%s,Stp_Reapet=%s,Slant_lr=%s,
                                        Slant_fb=%s,Amplitude=%s,Electromobile=%s,Voltage=%s,Power_Down=%s,Manual_Alarm=%s,Maintain=%s''' %\
                                    (Kaceng, Kunren, UP_Alarm, Down_Alarm, Run_door, Speeding, Power, Stp_DoorUnopen, Stp_DoorUnclose, Stp_Reapet, Slant_lr, Slant_fb, \
                                        Amplitude, Electromobile, Voltage, Power_Down, Manaual_Alarm, MainTain)

                    if last_alarm_str != now_alarm_str:
                        r.set('r' + '_' + DAQ_ID, recv)
                        r.set('a' + '_' + DAQ_ID, recv, ex=key_alive_time)
                        try:
                            publish.single("alarm", payload=recv, hostname="127.0.0.1")
                        except:
                            print "MQTT publish error"
                        print "The alarm isn't the same as the last one"

                    else:
                        # 避免在一段时间内重复报警
                        r.set('r' + '_' + DAQ_ID, recv)
                        print "The time interval is too small"

            else:              # 不存在报警信息
                r.set('r' + '_' + DAQ_ID, recv)    # 将数据写入Redis
                print "All elevators are running"

        else:                  # ID不合法，继续读取新的数据
            print "The ID is illegal"

    except:
        print 'The format of json is wrong!'

# 接收来自采集器的数据
if __name__ == "__main__":
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    '''每一个小时更新一次数据库'''

    while 1:
        try:
            client.connect('127.0.0.1', port=1883)
            client.loop_forever()
        except:
            client.disconnect()
            print "The MQTT of collector is something wrong!!!"
            time.sleep(0.5)
