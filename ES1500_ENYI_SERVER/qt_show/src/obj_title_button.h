#ifndef __OBJ_TITLE_BUTTON_H__
#define __OBJ_TITLE_BUTTON_H__

/*
    文档简介：
    QT实时展示界面标题栏中的音频控制按钮
    
    按钮图标名称规则：
    [是否开启][有无报警].jpg
    00.jpg  音频关闭-无报警
    01.jpg  音频关闭-有报警
    10.jpg  音频开启-无报警
    11.jpg  音频开启-有报警
*/

#include <QPushButton>
#include <QWidget>
#include <QString>
#include <QIcon>
#include <QMutex>

#include "conf.h"
#include "jsonConfFile.h"
#include "../include/json/json.h"

class obj_title_button:public QPushButton{
    
    private:
        unsigned char open_off;             //开启关闭状态
        unsigned char status;               //有无报警状态
        QString imgPath;                    //图标文件地址
        QMutex mutex;

    public:
        obj_title_button(const unsigned char _open_off=0, QWidget *_parent=0);
        ~obj_title_button();
    
    private:
        int set_btimg();                                                         //生成按钮图标文件

    public:
        int init();                                                                     //初始化
        int open();                                                                     //开启音频
        int close();                                                                    //关闭音频
        int set_alarm();                                                                //设置报警
        int clear_alarm();                                                              //取消报警                                               

        int isOpen();                                                                   //判断音频是否开启
        int isAlarm();                                                                  //是否处于报警状态
};

#endif
