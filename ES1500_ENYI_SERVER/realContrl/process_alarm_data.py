#!/usr/bin/python
#coding=utf-8
# 接收数据采集模块处理之后得出的报警信息，并将报警信息写入数据库、发送到微信

import threading
import json
import sqlite3
import paho.mqtt.client as mqtt
import time
import shutil   # High-level file operations
import os
from Queue import Queue
import redis
import urllib2
import sys
reload(sys)
sys.setdefaultencoding('utf8')

delta_time = 10       # 写入数据库的时间间隔，单位为分
alarm_q = Queue(60000)
alarm_cloud = Queue(60000)

r = redis.StrictRedis(host='127.0.0.1', port=6379, db=0)

# 读取devID
f_devID = open("/root/catch/devID", 'r')
dev_SN = f_devID.readline().strip('\n')     # 设备序列号
f_devID.close()

# Get filename'YearMonth.db'
def get_db_name():
    nowtime = time.localtime()      # 返回本地时间的日期元组
    if nowtime[1] < 10:     # 如果月份小于10月，则在月份前加0，比如201609
        filename = str(nowtime[0]) + '0' + str(nowtime[1])
    else:
        filename = str(nowtime[0]) + str(nowtime[1])

    return filename + '.db'

# Copy database from 'model' and rename it
def cp_rn_db(db_name):
    src_path = '/root/model/Alarm.db'   # 数据库文件标准模板
    dst_path = '/root/database'         # 待操作数据库存放目录
    shutil.copy(src_path, dst_path)     # 需要数据库时就从标准模板中复制一份过来
    os.rename(os.path.join(dst_path, 'Alarm.db'), os.path.join(dst_path, db_name))

# Whether there is the required database
def db_exist(db_name):
    path = '/root/database/' + db_name
    return os.path.exists(path)

# Remove database
def db_rm(db_name):
    path = '/root/database/' + db_name
    os.remove(path)

# Delete all files in the directory
def db_all_rm(path='/root/database'):
    for db in os.listdir(path):     # 索引目录下所有数据库
        db_path = os.path.join(path, db)
        if os.path.isfile(db_path):     # 判断待删除的是否为文件
            os.remove(db_path)
        else:
            try:
                os.rmdir(db_path)       # 用于删除指定路径的目录,仅当这文件夹是空的才可以
            except:
                pass

# Remain the latest three databases, others are deleted
# After creating a new database, please call this function
def remain_3_db(path='/root/database'):
    db_list = os.listdir(path)  # 索引目录下所有数据库
    if len(db_list) > 3:
        db_list.sort(reverse=True)  # 对列表中的数据库名称进行排序，日期最新的在前面
        for db in db_list[3:]:
            db_path = os.path.join(path, db)
            if os.path.isfile(db_path):  # 判断待删除的是否为文件
                os.remove(db_path)

def on_connect_inside(client, userdata, flags, rc):
    # print 'Connected with result code' + str(rc)
    client.subscribe('alarm')

def on_message_inside(client, userdata, msg):
    print msg.topic + ' ' + str(msg.payload)
    tmp = msg.payload
    alarm_q.put(tmp)
    alarm_cloud.put(tmp)


# 将报警信息写入数据库
class Alarm_to_db(threading.Thread):
    def run(self):
        last_db_name = ""  # 上次写入数据库的名称
        alarm_list = []
        while 1:
            try:
                alarm_json = alarm_q.get(timeout=10)
                alarm_list.append(alarm_json)

            except:
                print "The queue is timeout!"

            finally:
                print '*' * 80

                local_time = time.localtime()
                if local_time[4] % delta_time == 0 or len(alarm_list) > 100:  # 时间为设置的delta_time时间的整数倍时就写入数据库
                    alarm_list_len = len(alarm_list)
                    if alarm_list_len == 0:
                        pass
                    else:
                        print "writing to the database.."
                        db_name = get_db_name()
                        if last_db_name != db_name:
                            if not db_exist(db_name):  # 如果不存在数据库，则新建一个
                                cp_rn_db(db_name)
                            remain_3_db()
                        db_path = '/root/database/' + db_name
                        conn_alarm_db = sqlite3.connect(db_path)
                        print "alarm_list is:::"
                        print alarm_list
                        for x in xrange(alarm_list_len):
                            alarm           = json.loads(alarm_list.pop(0))
                            Parameters      = alarm.get('Parameters')
                            Alarm           = alarm.get('Alarm')
                            DT_State        = alarm.get('State')

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
                            Manual_Alarm    = Alarm.get('Manual_Alarm')
                            Electromobile   = Alarm.get('Electromobile')
                            Voltage         = Alarm.get('Voltage')
                            Power_Down      = Alarm.get('Power_Down')
                            Maintain        = Alarm.get('Maintain')
                            
                            DS_DX           = DT_State.get('DS_DX')
                            Lou_Ceng        = DT_State.get('Lou_Ceng')
                            status          = DT_State.get('status')
                            Door            = DT_State.get('Door')
                            People          = DT_State.get('People')
                            G_X             = DT_State.get('G_X')
                            G_Y             = DT_State.get('G_Y')
                            G_Z             = DT_State.get('G_Z')
                            OSD             = DT_State.get('OSD')
                            
                            Dev_Name = r.get('d' + '_' + DAQ_ID)
                            if Dev_Name is None:
                                Dev_Name = "未知名称"
                            else:
                                Dev_Name = json.loads(Dev_Name)
                                Dev_Name = Dev_Name.get("dev_name")
                            conn_alarm_db.execute("INSERT INTO alarm_info (dev_id, dev_name, alarm_time, ds_dx, lou_ceng, status, door, people, g_x, g_y, g_z,\
                                    osd, y_n, kaceng, kunren, up_alarm, down_alarm, run_door, speeding, power, stp_doorunopen, stp_doorunclose, stp_reapet, amplitude, voltage, power_down, electromobile, manaual_alarm, slant_fb, slant_lr, maintain) VALUES \
                                    ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')" % \
                                    (DAQ_ID, Dev_Name, Recv_time, DS_DX, Lou_Ceng, status, Door, People, G_X, G_Y, G_Z, OSD, Alarm_YN, Kaceng, Kunren, UP_Alarm, Down_Alarm, Run_door, Speeding, \
                                    Power, Stp_DoorUnopen, Stp_DoorUnclose, Stp_Reapet, Amplitude, Voltage, Power_Down, Electromobile, Manual_Alarm, Slant_fb, Slant_lr, Maintain))
                            conn_alarm_db.commit()
                        conn_alarm_db.close()
                        last_db_name = db_name
                time.sleep(0.2)

#将用于微信报警的相关数据发送到云端
class Alarm_to_cloud(threading.Thread):
    def run(self):
        while 1:
            try:
                alarm_raw = alarm_cloud.get()       # 没有报警信息，队列将阻塞
                alarm_raw = json.loads(alarm_raw)
                Parameters = alarm_raw.get('Parameters')
                DAQ_ID = Parameters.get('ID')
                # 从redis中获取设备名称
                dev_name = r.get('d' + '_' +DAQ_ID)
                if dev_name is None:
                    dev_name = "未知名称"
                else:
                    dev_name = json.loads(dev_name)
                    dev_name = dev_name.get("dev_name")     # DAQ_ID对应的设备名

                Parameters["dev_name"] = dev_name
                alarm_new = json.dumps(alarm_raw)           # 生成包含设备名称的报警信息
                
                try:
                    wx_info_list = []           # 用来存放用户信息和appid的列表
                    conn_dev = sqlite3.connect("/root/model/DeviceInfo.db")
                    cursor = conn_dev.execute("SELECT wname, wchart from wperson")
                    for row in cursor:
                        wx_info = dict(name=row[0], appid=row[1])
                        wx_info_list.append(wx_info)
                    print wx_info_list
                    conn_dev.close()
                except:
                    print "DeviceInfo DB was error!"
                    break

                send_dict = dict(SN=dev_SN, wx_info=wx_info_list, alarm_info=alarm_new)
                send_json = json.dumps(send_dict)
                try:
                    urllib2.urlopen('http://118.190.20.24:9090', send_json,  timeout=10)
                    print "urlopen send!"
                except:
                    print "urlopen error!"

            except:
                print "Send alarm_info to cloud unsuccessfully"


if __name__ == "__main__":
    t1 = Alarm_to_db()
    t1.setDaemon(True)
    t1.start()

    t2 = Alarm_to_cloud()
    t2.setDaemon(True)
    t2.start()
    
    client_inside = mqtt.Client()
    client_inside.on_connect = on_connect_inside
    client_inside.on_message = on_message_inside

    while 1:
        try:
            client_inside.connect('127.0.0.1', port=1883)
            client_inside.loop_forever()
        except:
            client_inside.disconnect()
            print "The MQTT inside is something wrong!!!"
            time.sleep(0.5)
