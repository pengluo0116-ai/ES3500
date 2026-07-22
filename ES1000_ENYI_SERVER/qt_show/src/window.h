#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <QWidget>
#include <QVBoxLayout>

#include "conf.h"
#include "obj_web.h"
#include "obj_title_button.h"

class window: public QWidget{
    private:
        QVBoxLayout *layout_body;           //布局管理器
        //obj_title *title;                   //标题
        obj_web *web;                       //WEB展示
        //obj_title_button *radioBtn;         //音频按钮

    public:
        window(QWidget *_parent = 0);
        ~window();

    public:
        int init();
        //int checkout_radioBtn(obj_title_button *_btn);                      //检出音频按钮指针
        //int radioBtn_setAlarm();                                            //设置按钮状态--报警
        //int radioBtn_clearAlarm();                                          //设置按钮状态--取消报警
};

class Global_window{
    public:
        static window *mwindow;
};

#endif