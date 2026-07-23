#include "usart_read.h"
using namespace std;
#define DEBUG 0

/*
    函数名称：usart_read
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
    编写时间：2016-10-24
*/
usart_read::usart_read(const char *_ttl_path, int _nSpeed, char _nEvent, int _nPartity, int _nStop):
fd(0x00), ttl_path(string(_ttl_path)), nSpeed(_nSpeed), nEvent(_nEvent), nPartity(_nPartity), nStop(_nStop){}

/*
    函数名称：~usart_read
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-24
*/
usart_read::~usart_read(){
    this->uclose();  
}

/*
    函数名称：uclose
    函数功能：关闭串口
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-24
*/
void usart_read::uclose(){
    if(!this->fd) return;

    close(this->fd); this->fd = 0x00;
}

/*
    函数名称：check
    函数功能：校验参数
    传入参数：无
    传出数据：
                 0  运行成功
                -1  串口文件路径有误
                -2  波特率设置有误
                -3  奇偶校验位有误
                -4  数据位有误
                -5  停止位有误
    注意事项：串口开启前，进行校验
    编写人员：王凤龙
    编写时间：2016-10-24
*/
int usart_read::check(){
    /*串口文件路径校验*/
    if(this->ttl_path.size() <= 0x00) return -0x01;

    /*波特率校验*/
    switch(this->nSpeed){
        case 2400:  break;
        case 4800:  break;
        case 9600:  break;
        case 57600: break;
        case 115200: break;
        default: return -0x02;
    }

    /*奇偶校验位*/
    switch(this->nEvent){
        case 'o':   break;
        case 'O':   break;
        case 'e':   break;
        case 'E':   break;
        case 'n':   break;
        case 'N':   break;
        default: return -0x03;
    }

    /*数据位校验*/
    switch(this->nPartity){
        case 7: break;
        case 8: break;
        default: return -0x04;
    }

    /*停止位校验*/
    switch(this->nStop){
        case 1: break;
        case 2: break;
        default: return -0x05;
    }

    return 0x00;
}

/*
    函数名称：sopen
    函数功能：打开串口文件
    传入参数：无
    传出数据：0 运行成功 -6 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-24
*/
int usart_read::sopen(){
    if((this->fd = open(this->ttl_path.c_str(), O_RDONLY | O_NOCTTY)) <= 0x00) return -0x06;

    return 0x00;
}

/*
    函数名称：setUsart
    函数功能：设置串口
    传入参数：无
    传出数据：
                 0  运行成功
                -7  获取串口原有配置失败
                -8  提交串口配置失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-24
*/
int usart_read::setUsart(){
    struct termios newtio;

    /*获取原有配置*/
    if(tcgetattr(this->fd, &newtio)){
        #if DEBUG
        cout<< "获取原有配置失败" <<endl;
        #endif

        this->uclose();
        return -0x07;
    }

    /*清除一下输入队列*/
    tcflush(this->fd, TCIFLUSH);

    /*设置本地连接和读取*/
    memset((void *)&newtio, 0x00, sizeof(struct termios));
    newtio.c_cflag |= (CLOCAL | CREAD);
    newtio.c_cflag &= ~CSIZE;
    newtio.c_cc[VTIME] = 0x00;                              //设置读取超时时间
    newtio.c_cc[VMIN] = 0x01;                               //设置读取最少字节数量
    newtio.c_cflag &= ~(ICANON | ECHO | ECHOE | ISIG);      //原始输入，输入字符只是被原封不动的接收
    newtio.c_iflag &= ~(IXON | IXOFF | IXANY);              //软件流控制无效，因为硬件没有硬件流控制，所以就不需要管了
    newtio.c_oflag &= ~(ONLCR | OCRNL);
    newtio.c_iflag &= ~(ICRNL | INLCR);

    /*设置波特率*/
    switch(this->nSpeed){
        case 2400: cfsetispeed(&newtio, B2400); break;
        case 4800: cfsetispeed(&newtio, B4800); break;
        case 9600: cfsetispeed(&newtio, B9600); break;
        case 57600: cfsetispeed(&newtio, B57600); break;
        case 115200: cfsetispeed(&newtio, B115200); break;
    }

    /*设置奇偶校验位*/
    switch(this->nEvent){
        case 'o':   //奇校验
        case 'O': 
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
            break;

        case 'e':   //偶校验
        case 'E':
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
            break;

        case 'n':   //无校验
        case 'N':
            newtio.c_cflag &= ~PARENB;
            break;
    }

    /*设置数据位*/
    switch(this->nPartity){
        case 7: newtio.c_cflag |= CS7; break;
        case 8: newtio.c_cflag |= CS8; break;
    }

    /*设置停止位*/
    switch(this->nStop){
        case 1: newtio.c_cflag &= ~CSTOPB; break;
        case 2: newtio.c_cflag |= CSTOPB; break;
    }

    /*清除输入队列*/
    tcflush(this->fd, TCIFLUSH);

    /*提交配置信息*/
    if((tcsetattr(this->fd, TCSANOW, &newtio))){
        #if DEBUG
        cout<< "设置配置信息失败" <<endl;
        #endif

        this->uclose();
        return -0x08;
    }

    return 0x00;
}

/*
    函数名称：init
    函数功能：初始化
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
    编写时间：2016-10-24
*/
int usart_read::init(){
    int err = 0x00;

    /*进程内关闭串口操作，防止多次打开*/
    this->uclose();

    /*参数校验*/
    if(err = this->check()) return err;

    /*打开串口*/
    if(err = this->sopen()) return err;

    /*设置串口数据*/
    if(err = this->setUsart()) return err;

    return 0x00;
}

/*
    函数名称：getData
    函数功能：读取串口数据
    传入参数：
                unsigned char *dataBuff     获取到的数据存储到该指针指向的存储空间 
                int len                     存储空间大小
    传出数据：
                <0  获取数据失败
                >=0 获取到的实际数据
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-25
*/
int usart_read::getData(unsigned char *dataBuff, int len){
    /*参数校验*/
    if(dataBuff == (unsigned char *)0x00 || len <= 0x00) return -0x02;
    
    /*读取数据*/
    return read(this->fd, dataBuff, len);
}