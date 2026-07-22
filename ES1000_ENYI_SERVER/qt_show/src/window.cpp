#include "window.h"

window* Global_window::mwindow = (window *)NULL;

/*
    函数名称：window
    函数功能：构造函数
    传入参数：QWidget *_parent   父窗口指针
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-12
*/
window::window(QWidget  *_parent):QWidget(_parent){
    
}

/*
    函数名称：~windowradioBtn
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-13
*/
window::~window(){}

/*
    函数名称：init
    函数功能：初始化
    传入参数：无
    传出数据：
    注意事项：
    编写人员：王凤龙
    编写时间：2017-06-13
*/
int window::init(){
    /*创建窗体*/
    this->layout_body = new QVBoxLayout(this);  
    
    //this->title = new obj_title(this);  
    //if(this->title->init()) return -0x01;

    this->web = new obj_web(WEB_DEFAULT_URL, this); 
    if(this->web->init()) return -0x02;

    /*组装窗体*/
    //this->layout_body->addWidget((QWidget *)this->title);
    this->layout_body->addWidget((QWidget *)this->web);
    this->setLayout(this->layout_body);

    /*设置窗体*/
    this->layout_body->setMargin(0x00);
    this->layout_body->setSpacing(0x00);
    this->showFullScreen();

    return 0x00;
}

/*
    函数名称：checkout_radioBtn
    函数功能：检出音频按钮指针
    传入参数：obj_title_button *_btn 音频按钮指针
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-13
*/
#if 0
int window::checkout_radioBtn(obj_title_button *_btn){
    if(!_btn) return -0x01;

    this->radioBtn = _btn;

    return 0x00;
}
#endif
/*
    函数名称：radioBtn_ON
    函数功能：设置按钮状态--报警
    传入参数：无
    传出数据：
                 0  运行成功
                -1  按钮不存在
                -2  设置按钮状态失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-13
*/
#if 0
int window::radioBtn_setAlarm(){
    /*校验按钮是否存在*/
    if(!this->radioBtn) return -0x01;

    /*设置报警*/
    if(this->radioBtn->set_alarm()) return -0x02;

    return 0x00;
}
#endif
/*
    函数名称：radioBtn_OFF
    函数功能：设置按钮状态--取消报警
    传入参数：无
    传出数据：
                 0  运行成功
                -1  按钮不存在
                -2  设置按钮状态失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-13
*/
#if 0
int window::radioBtn_clearAlarm(){
    /*校验按钮是否存在*/
    if(!this->radioBtn) return -0x01;

    /*设置报警*/
    if(this->radioBtn->clear_alarm()) return -0x02;

    return 0x00;
}
#endif