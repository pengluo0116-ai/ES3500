#include "tcp_sock_pakage.h"
using namespace std;
#define DEBUG 0

/*
    函数名称：dataToPakage
    函数功能：将字符串封装成包
    传入参数：
                const char *str                     要封装的数据
                struct tcp_package_modle &_pakage   封装好的数据存储到该结构体变量中
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-08-30
*/
int dataToPakage(const char *str, struct tcp_package_modle &_pakage){
    /*校验输入参数*/
    if(str == (const char *)0x00 || strlen(str) >= 1000) return -0x01;

    /*封装前准备*/
    memset((void *)&_pakage, 0x00, sizeof(struct tcp_package_modle));
    _pakage.head = 0xAA55;

    /*复制数据*/
    memcpy((void *)_pakage.data, (void *)str, strlen(str));

    /*crc8*/
    _pakage.crc8 = CRC8_Table((unsigned char *)str, strlen(str));

    return 0x00;
}

/*
    函数名称：dataToPakage
    函数功能：将json封装成包
    传入参数：
                Json::Value &jsonData               要封装的数据
                struct tcp_package_modle &_pakage   封装好的数据存储到该结构体变量中
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-08-30
*/
int dataToPakage(Json::Value &jsonData, struct tcp_package_modle &_pakage){
    /*Json准换成字符串*/
    Json::FastWriter fast_writer;
    string jsonStr = fast_writer.write(jsonData); 

    return dataToPakage(jsonStr.c_str(), _pakage);
}

/*
    函数名称：pakageToJson
    函数功能：将接收到的包转换成json
    传入参数：
                unsigned char *getData  接收到的数据
                int len                 数据长度
                Json::Value &jsonData   转换成的json放置到该变量中
    传出数据：0 运行成功 非0 运行失败
    注意事项：该函数进针对于TCP接收数据时使用 TCPbuff为1024
    编写人员：王凤龙
    编写时间：2016-08-30
*/
int pakageToJson(unsigned char *getData, int len, Json::Value &jsonData){
    /*校验输入参数*/
    if( getData == (unsigned char *)0x00 || len <=4 || len > 1004 ) return -1;

    /*转换类型*/
    struct tcp_package_modle *tmp_model = (struct tcp_package_modle *)getData;

    /*校验*/
    if(tmp_model->head != 0XAA55 || CRC8_Table(tmp_model->data, strlen((const char *)tmp_model->data)) != tmp_model->crc8) return 1;   //CRC8

    /*提取数据*/
    getData[len] = 0x00;
    string jsonStr = string((const char *)tmp_model->data);

    /*转换成JSON*/
    Json::Features f = Json::Features::strictMode();
    Json::Reader reader(f);
    if(!reader.parse(jsonStr, jsonData)) return 2;

    return 0x00;
}