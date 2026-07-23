#ifndef __CONF_H__
#define __CONF_H__

/*数据库路径*/
#define DBPATH                  "/root/model/DeviceInfo.db"   

/*REDIS服务器的网络信息*/
#define MQTT_SERVER_IP          "127.0.0.1"
#define MQTT_SERVER_PORT        6379

/*WEB配置参数-用户WEB*/
#define WEB_PATH                "/root/bin/appweb"
#define WEB_CONF_PATH           "/root/bin/web.conf"
#define WEB_CMD_DELLOG          "rm -rf /root/bin/errorlog/*"
#define WEB_NAME                "WEB"

/*搜索软件参数*/
#define UDPSERVER_PATH          "/root/server_udp/gethost_udpserver"
#define UDPSERVER_NAME          "UDPSERVER"

/*转发采集器参数*/
#define COLECTER_PATH           "/root/collecter/collect"
#define COLECTER_CONF           "/root/collecter/post.ini"
#define COLECTER_NAME           "COLECTER"

/*NGINX服务器*/
#define NGINX_PATH              "/usr/local/nginx/sbin/nginx"
#define NGINX_CONF              ""
#define NGIGX_NAME              "NGINX"

/*阿里上报*/
#define ALIPOST_PATH            "/root/upali/upali"
#define ALIPOST_CONF            "/root/upali/alipost.conf"
#define ALIPOST_NAME            "UPALI"

/*REDIS服务器*/
#define REDIS_PATH              "/usr/local/redis-3.2.1/src/redis-server"
#define REDIS_CONF              ""
#define REDIS_NAME              "REDIS_SERVER"

/*MQTT服务器*/
#define MQTTSERVER_PATH         "/usr/local/sbin/mosquitto"
#define MQTTSERVER_CONF         "/etc/mosquitto/mosquitto.conf"
#define MQTTSERVER_NAME         "MQTT_SERVER"

/*底层采集器-WEB*/
#define DEV_COLECTER_PATH       "/usr/local/bin/uwsgi"
#define DEV_COLECTER_CONF       "/etc/uwsgi.ini"
#define DEV_COLECTER_NAME       "UWSGI_COLLECTER"

/*实时处理模块*/
#define REALCONT_PATH           "/root/recv_collector_data.py"
#define REALCONT_CONF           ""
#define REALCONT_NAME           "REALCONT"

/*报警信息处理模块*/
#define ALARMCONT_PATH          "/root/process_alarm_data.py"
#define ALARMCONT_CONF          ""
#define ALARMCONT_NAME          "ALARMCONT"

/*REDIS缓存监控模块*/
#define CREDIS_CK_PATH          "/root/rcatch/credis"
#define CREDIS_CK_CONF          ""
#define CREDIS_CK_NAME          "CREDIS"

/*QT实时展示*/
#define QT_REAL_SHOW_PATH       "/root/qt_show/qt_real_show"
#define QT_REAL_SHOW_CONF       "-qws"
#define QT_REAL_SHOW_NAME       "QTSHOW"

/*百度语音实时更新*/
#define BAIDUTTS_UPDATE_PATH    "/root/baidu_tts/baidu_tts_client"
#define BAIDUTTS_UPDATE_CONF    ""
#define BAIDUTTS_UPDATE_NAME    "BAIDU_TTS"

/*重启脚本*/
#define SYS_REBOOT_CONT         "/root/cmd/rbt &"

#endif