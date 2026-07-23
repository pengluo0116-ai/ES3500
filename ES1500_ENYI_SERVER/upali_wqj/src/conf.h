#ifndef __CONF_H__
#define __CONF_H__

/*STC串口*/
#define STC_UART_PATH                   "/dev/ttyS1"
#define STC_UART_SPEED                  57600
#define STC_UART_ENEVT                  'N'
#define STC_UART_BITS                    8
#define STC_UART_STOP                    1
#define STC_UART_GETDATABUFF_MAXSIZE    1024                     //从STC串口获取到的数据缓冲区最大长度
#define STC_UART_PKGDATABUFF_MAXSIZE    767                     //从STC获取到数据有效数据的最大长度

/*共享内存--叠加数据*/
#define SHM_NAME_PM                     "spm_shm"                   //共享内存名称-PM2.5 PM10
#define SHM_NAME_TEMPHUM                "stemphum_shm"              //共享内存名称-温湿度
#define SHM_NAME_WSDIR                  "swsdir_shm"                //共享内存名称-风速风向
#define SHM_NAME_NVH                    "snvh_shm"                  //共享内存名称-噪音
#define SHM_NAME_PKG                    "pkg_shm"                   //共享内存名称-实时数据
#define SHM_BUFF_SIZE                   512                     //共享内存缓冲区大小

#define STC_VERSION_FILE                 "/root/collecter/stcv.log"
#define SYS_VERSION_PATH                 "/root/db/sysinfo"

#define SYS_UPDATE_CONT                  "/root/update/supdate &"

#define SENSOR_CONF_PATH                "/root/collecter/sensor.conf"           //传感器配置路径
#define SENSOR_POST_CONF                "/root/collecter/post.ini"             //数据上传配置文件
#define ALIPOST_CONF                    "/root/upali/alipost.conf"

#define ALL_SENSOR_CONF_PATH            "/root/collecter/all_sensor.conf"       //传感器配置路径
#define ALL_SENSOR_PARAMS               "/root/collecter/sensor_param.conf"
#define EN_SENSOR_PATH                  "/root/collecter/en_sensor.conf"
#define EN_EXE_PATH                     "/root/collecter/en_exe.conf"
#define ALL_CONF                        "/root/collecter/full.conf"
#define NETWORK_CONFPATH    "/etc/network/interfaces.d/eth0"                //设备网络配置路径
#define DNS_CONF_PATH       "/etc/resolv.conf"                              //DNS配置文件路径

/*数据库路径*/
#define DBPATH              "/root/db/mydb.db" 
#define OSD_COLOR_CONF      "/root/color/osdcolor.conf"
#define DevNameFile         "/root/devname.log"                     //LED屏显示的设备名称存储文件
#define UPALI_POST_CONF     "/root/upali/alipost.conf"
#define LED_CONF            "/root/ledshow/led.conf"

#define EN_EXE_STATE_PATH   "/root/upali/exe_state.conf"

//设备自动重启
#define ALARM_TIME_POST      "/root/alarm_time/alarm.conf"


/*系统升级初始化标志*/
#define SYS_UPDATE_BITPATH   "/root/updatebitf"
#define SYS_UPDATE_BITTOUCH  "touch /root/updatebitf"

#define EN_FIELD_PATH      "/root/collecter/en_field.conf"

/*获取CCID*/
#define MODULE_CCID         "/root/ccid.conf"

#define CMD_SYSREBOOT       "/root/cmd/rbt &"
#endif