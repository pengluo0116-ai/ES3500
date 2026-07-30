#ifndef __CONF_H_
#define __CONF_H_

#define DBPATH              "/root/model/DeviceInfo.db"                     //数据库路径 管理员登录信息存在该数据库中

#define GETWAN_FUN_PATH     "python /root/cmd/getLan"                       //脚本路径
#define WANDATA_PATH        "/root/catch/landata"                           //获取到的网络信息存储到该文件路径下
#define SYSINFO_PATH        "/root/model/sysinfo"                           //系统信息路径
#define NETWORK_CONFPATH    "/etc/network/interfaces.d/eth0"                //设备网络配置路径
#define CMD_SYSREBOOT       "/root/cmd/rbt &"
#define DNS_CONF_PATH       "/etc/resolv.conf"                              //DNS配置文件路径         
#endif