#include "obj_title_button.h"
#include "radio.h"
#define DEBUG 1

/*
    函数名称：obj_title_button
    函数功能：构造函数
    传入参数：
                const unsigned char _open_off   音频开启关闭状态
                QWidget *_parent                父窗体控件指针
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-17
*/
obj_title_button::obj_title_button(const unsigned char _open_off, QWidget *_parent):QPushButton(_parent), open_off(_open_off), status(0){}

/*
    函数名称：set_btimg
    函数功能：设置按钮图标文件
    传入参数：无
    传出数据：保留 0
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-17
*/
int obj_title_button::set_btimg(){
    this->mutex.lock();

    /*生成图标文件*/
    this->imgPath = QString(TITLE_BUTTON_IMG_DIR) + "/" + QString::number(this->open_off) + QString::number(this->status) + ".jpg";

    /*设置图标文件*/
    QIcon ico(this->imgPath);
    this->setIcon(ico);

    this->mutex.unlock();
    return 0x00;
}

/*
    函数名称：~obj_title_button
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-17
*/
obj_title_button::~obj_title_button(){}

/*
    函数名称：init
    函数功能：初始化
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-17
*/
int obj_title_button::init(){

    /*设置图标*/
    if(this->set_btimg()) return -0x01;

    /*设置图标大小*/
    this->setIconSize(QSize(65, 65));

    /*设置尺寸*/
    this->setFixedSize(65, 65);

    /*去掉边框*/
    this->setFlat(true);

    /*设置音频开关*/
    if(this->open_off) radio::ON(); else radio::OFF();

    return 0x00;
}

/*
    函数名称：open
    函数功能：开启音频
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-17
*/
int obj_title_button::open(){
    /*检测当前状态*/
    if(this->open_off == 0x01) return 0x00;

    /*开启音频*/
    this->open_off = 0x01;
    this->set_btimg();
    radio::ON();

    /*写入配置文件*/
    Json::Value jsonConf_btn; jsonConf_btn.clear();
    jsonConf_btn["enable"] = 0x01;
    JwriteConf(TITLE_BUTTON_CONF, jsonConf_btn);

    return 0x00;
}

/*
    函数名称：close
    函数功能：关闭音频
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-17
*/
int obj_title_button::close(){
    /*检测当前状态*/
    if(this->open_off == 0x00) return 0x00;

    /*关闭音频*/
    this->open_off = 0x00;
    this->set_btimg();
    radio::OFF();

    /*写入配置文件*/
    Json::Value jsonConf_btn; jsonConf_btn.clear();
    jsonConf_btn["enable"] = 0x00;
    JwriteConf(TITLE_BUTTON_CONF, jsonConf_btn);

    return 0x00;
}

/*
    函数名称：set_alarm
    函数功能：设置报警状态
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-17
*/
int obj_title_button::set_alarm(){
    /*检测当前状态*/
    if(this->status == 0x01) return 0x00;

    /*设置报警状态*/
    this->status = 0x01;
    this->set_btimg();

    return 0x00;
}

/*
    函数名称：clear_alarm
    函数功能：解除报警状态
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-17
*/
int obj_title_button::clear_alarm(){
    /*检测当前状态*/
    if(this->status == 0x00) return 0x00;

    /*接触报警状态*/
    this->status = 0x00;
    this->set_btimg();

    return 0x00;
}


/*
    函数名称：isOpen
    函数功能：获取音频开关状态
    传入参数：无
    传出数据：
                0   关闭
                1   开启
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-18
*/
int obj_title_button::isOpen(){
    return this->open_off;
}

/*
    函数名称：isAlarm
    函数功能：获取报警状态
    传入参数：无
    传出数据：
                0   无报警
                1   有报警
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-18
*/
int obj_title_button::isAlarm(){
    return this->status;
}