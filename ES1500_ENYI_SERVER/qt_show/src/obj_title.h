#ifndef __OBJ_TITLE_H__
#define __OBJ_TITLE_H__

/*
    文档简介：
    QT实时展示界面的标题栏，展示系统名称“电梯运行实时监控系统”，展示设备的总数，在线数，报警数
    提供用户一个按钮（音频开关），控制报警音频的开启和关闭
*/

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "obj_title_button.h"
#include "jsonConfFile.h"

class obj_title: public QLabel{
    Q_OBJECT
    private:
        unsigned short dev_all;                 //设备总数
        unsigned short dev_online;              //在线设备数
        unsigned short dev_alarm;               //故障设备数
        
        QLabel *lable_liftNum_all;              //标签-电梯总数
        QLabel *lable_liftNum_online;           //标签-在线电梯数量
        QLabel *lable_liftNum_alarm;            //标签-报警电梯数量
        obj_title_button *button_radio;         //按钮-开关报警音频

    public:
        obj_title(QWidget *_parent = 0);                                            //构造函数
        ~obj_title();                                                               //析构函数
    
    private:
        int create_viewFrame();                                                     //创建视图

    public:
        int init();                                                                 //初始化函数
        unsigned short get_devAll();                                                //获取设备总数记录
        unsigned short get_devOnline();                                             //获取在线设备数记录
        unsigned short get_devAlarm();                                              //获取故障设备树记录
        int status(unsigned short _devAll, unsigned short _devOnline, unsigned short _devAlarm);    //实时状态设置

    private slots:
        void button_click();
        void t_status(unsigned short _devAll, unsigned short _devOnline, unsigned short _devAlarm);

    signals:
        void s_status(unsigned short _devAll, unsigned short _devOnline, unsigned short _devAlarm);
};

#endif