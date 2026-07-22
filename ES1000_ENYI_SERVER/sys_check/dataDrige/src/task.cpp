#include "task.h"
using namespace std;
#define DEBUG 0

/*
    函数名称：task
    函数功能：构造函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-25
*/
task::task():stc_uart((stc_cont *)0x00), utf8StrBuff((unsigned char *)0x00){
    for(int i=0x00; i<SHM_MAXNUM; i++){ this->str_shm_list[i] = (shm_cont *)0x00; }
}

/*
    函数名称：~task
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-25
*/
task::~task(){
    /*删除串口*/
    if(this->stc_uart){ delete this->stc_uart; this->stc_uart = (stc_cont *)0x00; } 
    
    /*删除utf8字符串缓冲区*/
    delete this->utf8StrBuff; this->utf8StrBuff = (unsigned char *)0x00;

    /*删除共享内存*/
    for(int i=0x00; i<SHM_MAXNUM; i++){
        if(!this->str_shm_list[i]) continue;
        delete this->str_shm_list[i]; this->str_shm_list[i] = (shm_cont *)0x00;
    }
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
    注意事项：如果该步骤没有成功，需要立刻结束进程
    编写人员：王凤龙
    编写时间：2016-10-25
*/
int task::init(){
    int err = 0x00;

    /*开启串口*/
    this->stc_uart = new stc_cont(STC_UART_PATH, STC_UART_SPEED, STC_UART_ENEVT, STC_UART_BITS, STC_UART_STOP);
    if(err = this->stc_uart->stc_init()) return err;

    /*开启共享内存*/
    ostringstream ostr; 
    for(int i=0x00; i<SHM_MAXNUM; i++){
        ostr.str("");
        ostr << (101 + i) << "_shm";
        this->str_shm_list[i] = new shm_cont(ostr.str().c_str(), SHM_DATABUFF_MAXSIZE);

        if(this->str_shm_list[i]->shm_create()) return 0x09;
    }

    /*设置utf8缓存字符串区域*/
    this->utf8StrBuff = new unsigned char[UTF8_STRBUFF_SIZE];

    return 0x00;
}

/*
    函数名称：RUN
    函数功能：工作函数
    传入参数：无
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-25
*/
int task::RUN(){
    struct stc_package pkg;

    for(;;){
        /*接收数据*/
        if(this->stc_uart->stc_getpkg(&pkg)) break;

        cout<< "crc8:" << (int)pkg.crc8 <<endl;
        cout<< "长度:" << (int)pkg.len<<endl;
        cout<<"命令：" << (int)pkg.cmd<<endl;

        /*系统处理*/
        if(this->cmd_contrl(&pkg)) break;
    }

    return 0x01;
}
/***************************************************************************************/
/*指令处理*/
/*
    函数名称：_cmd_restore
    函数功能：恢复出厂设置
    传入参数：无
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-25
*/
static int _cmd_restore(){
    /*组长发送报文*/
    tcp_package_modle _pakage;
    dataToPakage("{\"cmd\":3}", _pakage);

    /*建立TCP连接*/
    tcp_sock_client _client("127.0.0.1", 9191);
    if(_client.connectToServer()) { return -1;}

    /*发送数据*/
    if(_client.sendData((unsigned char *)&_pakage, sizeof(tcp_package_modle)) <= 0x00) { return -2;}

    /*接收数据*/
    int reg = 0x00;
    if((reg =_client.getData((unsigned char *)&_pakage, sizeof(tcp_package_modle))) <= 0x00){ return -3;}

    /*关闭连接*/
    _client.tcpClose();

    /*解析数据*/
    Json::Value jsonData;
    if(pakageToJson((unsigned char *)&_pakage, reg, jsonData)){ return 1;}
    
    if(jsonData["error"].asInt()) return 0x01;

    return 0x00;
}

struct show_str_model{
    unsigned char x;
    unsigned char y;
    char str[STC_SHOWSTR_MAXSIZE];
};

string& trim(string &s)   
{  
    if (s.empty())   
    {  
        return s;  
    }  
    s.erase(0,s.find_first_not_of(" "));  
    s.erase(s.find_last_not_of(" ") + 1);  
    return s;  
}  

/*
    函数名称：cmd_showStr
    函数功能：向指定的行打印数据
    传入参数：struct stc_package *pkg
    传出数据：0 运行正确 非0 运行错误
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-26
*/
int task::cmd_showStr(struct stc_package *pkg){

    struct show_str_model *_cmd_show = (struct show_str_model *)pkg->data;
    cout<<"X坐标:" << (int)_cmd_show->x <<endl;
    cout<<"Y坐标:" << (int)_cmd_show->y <<endl;
    
    /*写入共享变量*/
    if(_cmd_show->x<0x00 || _cmd_show->x>SHM_MAXNUM || strlen(_cmd_show->str)<=0x00) return 0x00;             //行数超限
    if(g2u((char *)_cmd_show->str, strlen(_cmd_show->str), (char *)this->utf8StrBuff,  UTF8_STRBUFF_SIZE)) return 0x01; 
    
    cout<<"长度:" << strlen((const char *)this->utf8StrBuff) << "数据：" << this->utf8StrBuff<<endl;
    
    string osd = string((const char *)this->utf8StrBuff);
    osd = trim(osd);

    if(this->str_shm_list[_cmd_show->x]->writeStr(osd.c_str())) return 0x02;
    // if(this->str_shm_list[_cmd_show->x]->writeStr((const char *)this->utf8StrBuff)) return 0x02;

    return 0x00;
}

/***************************************************************************************/

/*
    函数名称：cmd_contrl
    函数功能：串口指令处理
    传入参数：struct stc_package *pkg
    传出数据：0 运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-25
*/
int task::cmd_contrl(struct stc_package *pkg){ 
    /*参数校验*/
    if(!pkg) return -0x01;

    /*指令处理*/
    switch(pkg->cmd){
        
        /*恢复出厂设置*/
        case 0x0B: cout<< "恢复出厂设置" <<endl; _cmd_restore(); break;

        /*电梯数据*/
        case 0X0A: cout<< "电梯数据" <<endl; break;

        /*透传信息*/
        case 0X10: this->cmd_showStr(pkg); break;
        
        /*位置错误*/
        default: cout<< "未知数据" <<endl; 
    }


    return 0x00;
}