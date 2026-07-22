#include "stc_cont.h"
using namespace std;
#define DEBUG 0

/*
    函数名称：stc_cont
    函数功能：构造函数
    传入参数：
                const char *_ttl_path           串口文件路径
                int _nSpeed                     波特率
                char _nEvent                    奇偶校验位
                int _nPartity                   数据位
                int _nStop                      停止位
    传出数据：无
    注意事项：无
    编写人员：王凤龙 
*/
stc_cont::stc_cont(const char *_ttl_path, int _nSpeed, char _nEvent, int _nPartity, int _nStop):
usart_read(_ttl_path, _nSpeed, _nEvent, _nPartity, _nStop), dataBuff((unsigned char *)0x00){}

/*
    函数名称：~stc_cont
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-25
*/
stc_cont::~stc_cont(){
    if(!this->dataBuff) return;

    delete this->dataBuff; this->dataBuff = (unsigned char *)0x00;
}

/*
    函数名称：stc_init
    函数功能：初始化函数
    传入参数：无
    传出数据：
                 0  运行成功
                -1  串口文件路径有误
                -2  波特率设置有误
                -3  奇偶校验位有误
                -4  数据位有误
                -5  停止位有误
                -6  打开串口文件失败
                -7  获取串口原有配置失败
                -8  提交串口配置失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-25
*/
int stc_cont::stc_init(){
    /*分配缓冲区内存*/
    if(!this->dataBuff) this->dataBuff = new unsigned char[STC_UART_GETDATABUFF_MAXSIZE];

    /*初始化串口*/
    return this->init();
}

/*
    函数名称：stc_getpkg
    函数功能：获取一整包数据
    传入参数：struct stc_getpkg *pkg
    传出数据：
                 0  运行成功
                -1  参数有误
                -2  接收数据有误
                 1  异常错误
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-25
*/
int stc_cont::stc_getpkg(struct stc_package *pkg){
    /*参数校验*/
    if(!pkg) return -0x01;

    /*参数初始化*/
    this->getFlag = 0x00;                                                   //获取到报文头标志 0无 1获取crc8 2获取有效数据长度 3接收指令-参数
    this->getPoint = 0x00;                                                  //获取到数据的长度

    /*轮询接收*/
    int err = 0x00;             
    int _getLen = 0x00;                                                     //获取串口流中，指令参数组合 有效信息的长度
    for(;;){
        this->getPoint = 0x00;                                              //游标回到原位
        if(!this->getFlag) memset((void *)pkg, 0x00, sizeof(struct stc_package));

        /*接收数据*/
        if((err = this->getData(this->dataBuff, STC_UART_GETDATABUFF_MAXSIZE))< 0x00){
            #if DEBUG
            cout<<"接收串口数据有误"<<endl;
            #endif
            return -0x02;
        }

        #if DEBUG
        for(int i=0x00; i<err; i++){ printf("%02x ", (int)this->dataBuff[i]); }
        cout<<endl;
        #endif

        switch(this->getFlag){
            /*等待接收报文头*/
            case 0: {
                for(this->getPoint=0x00; this->getPoint<err-1; this->getPoint++){ 
                    if(*(unsigned short *)&dataBuff[this->getPoint] == 0xAA55){this->getFlag = 0x01; break;} 
                }       
                if(!this->getFlag) break;                                   //判断是否存在报文头
                if(this->getPoint >= err-1) break;                          //判断报文头后面是否还有数据
                this->getPoint += 0x02;                                     //游标移动两位
            }

            /*获取crc8*/
            case 1: {
                if(err - this->getPoint <= 0x00) break;                     //判断是否包含有效数据
                pkg->crc8 = this->dataBuff[this->getPoint];                 //获取crc8
                
                this->getFlag = 0x02;
                this->getPoint++;
            }     
            
            /*等待接收有效数据长度*/
            case 2: {
                if(err - this->getPoint <= 0x00) break;                     //判断是否包含有效数据
                pkg->len = this->dataBuff[this->getPoint];                  //获取参数数据长度

                if(pkg->len > STC_UART_PKGDATABUFF_MAXSIZE){ this->getFlag = 0x00; this->getPoint = 0x00; break;}
                
                this->getFlag = 0x03;
                this->getPoint++;
                _getLen=0x00;
            }    

            /*等待接收参数包含指令*/
            case 3: {
                if(err - this->getPoint <= 0x00) break;                     //判断是否包含有效数据
                int _len = (err - this->getPoint) > (pkg->len - _getLen) ? pkg->len - _getLen : err - this->getPoint; 
                memcpy((void *)(&(&pkg->cmd)[_getLen]), (void *)&this->dataBuff[this->getPoint], _len);
                
                _getLen += _len;

                if(_getLen< pkg->len) break;
                if((CRC8_Table(&pkg->len, pkg->len+1)) != pkg->crc8){ this->getFlag = 0x00; this->getPoint = 0x00; break;}
                pkg->len--; return 0x00;
            }
        }
    }
    
    return 0x01;
}