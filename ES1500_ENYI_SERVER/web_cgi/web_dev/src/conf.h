#ifndef __CONF_H_
#define __CONF_H_

#define DBPATH              "/root/model/DeviceInfo.db"                      //数据库路径
#define MAP_CONF_PATH       "/root/conf/map.conf"                            //地图配置文件

/*网络信息*/
#define GETWAN_FUN_PATH     "python /root/cmd/getLan"                       //获取网络配置信息
#define WANDATA_PATH        "/root/catch/rlandata"                          //获取到的网络信息存储到该文件路径下

/*REDIS配置信息*/
#define REDIS_SERVER_ADDR   "127.0.0.1"                                      //IP地址
#define REDIS_SERVER_PORT   6379                                             //端口
#define REDIS_SERVER_PASD   NULL                                             //登录密码

/*MQTT配置信息*/
#define MQTT_PORT           9001                                            //MQTT 端口
#define MQTT_TOPIC          "#"                                             //MQTT topic

#endif