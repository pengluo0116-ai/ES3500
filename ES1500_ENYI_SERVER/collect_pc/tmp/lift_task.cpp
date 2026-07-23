#include "lift_task.h"
using namespace std;
#define DEBUG 0

/*
    函数名称：lift_task
    函数功能：构造函数
    传入参数：
                const char *_lift_conf_path     电梯配置文件路径
                const char *_alarm_conf_path    报警配置文件路径
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-22
*/
lift_task::lift_task(const char *_lift_conf_path, const char *_alarm_conf_path):
lift_conf_path(string(_lift_conf_path)), alarm_conf_path(string(_alarm_conf_path)){
    this->stc_uart = (stc_cont *)0x00;
    this->str_shm = (shm_cont *)0x00;
    this->alarm_cont = (lift_alarm *)0x00;
    this->floor_num = 0x00;
}

/*
    函数名称：~lift_task
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-22
*/
lift_task::~lift_task(){
    /*删除串口*/
    if(this->stc_uart){ delete this->stc_uart; this->stc_uart = (stc_cont *)0x00; } 

    /*删除共享内存*/
    if(this->str_shm){ delete this->str_shm; this->str_shm = (shm_cont *)0x00; }

    /*删除报警处理*/
    if(this->alarm_cont){ delete this->alarm_cont; this->alarm_cont = (lift_alarm *)0x00; }
}

/*
    函数名称：init
    函数功能：初始化
    传入参数：无
    传出数据：
                 0  运行成功
                -10 配置文件路径为空
                -20 读取配置文件失败
                -30 配置文件格式有误
                -100  串口文件路径有误
                -200  波特率设置有误
                -300  奇偶校验位有误
                -400  数据位有误
                -500  停止位有误
                -600  打开串口文件失败
                -700  获取串口原有配置失败
                -800  提交串口配置失败
                -1000 共享变量名称有误
                1000  创建共享内存失败
                2000  设备共享内存尺寸失败
                3000  将共享内存映射到本进程失败
                -1    报警处理-读取电梯配置文件失败
                -2    报警处理-读取报警配置文件失败
                -3    报警处理-校验配置文件有误
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-22
*/
int lift_task::init(){
    int err = 0x00;

    /*读取配置文件*/
    if(err = JreadConf(this->lift_conf_path.c_str(), this->jsonConf)) return err * 10;

    /*获取楼层数*/
    this->floor_num = this->jsonConf["floordata"].size();

    /*开启串口*/
    this->stc_uart = new stc_cont(STC_UART_PATH, STC_UART_SPEED, STC_UART_ENEVT, STC_UART_BITS, STC_UART_STOP);
    if(err = this->stc_uart->stc_init()) return err * 100;

    /*开启共享内存*/
    this->str_shm = new shm_cont(jsonConf["sernum"].asString().c_str(), SHM_DATABUFF_MAXSIZE);
    if(err = this->str_shm->shm_create()) return err * 1000;

    /*初始化报警处理*/
    this->alarm_cont = new lift_alarm(this->lift_conf_path.c_str(), this->alarm_conf_path.c_str());
    if(err = this->alarm_cont->init()) return err;

    /*清空实时备份数据和重复计数*/
    memset((void *)&this->pkg_bak, 0x00, sizeof(struct lift_real_info));
    this->pak_cf = 0x00;

    return 0x00;
}

/*
    函数名称：cmd_contrl
    函数功能：将接收到的解析后的底层数据包进行处理
    传入参数：struct stc_package *pkg 底层接收的完整数据包
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-22
*/
int lift_task::cmd_contrl(struct stc_package *pkg){
    /*参数校验*/
    if(!pkg) return -0x01;

    /*指令处理*/
    switch(pkg->cmd){

        /*恢复出厂设置*/
        case 0x0B: return this->_cmd_restore(); break;

        /*获取传感器信息*/
        case 0x0A: this->_cmd_liftCont(pkg); break;

        /*未知错误*/
        default: break;

    }

    return 0x00;
}

/*
    函数名称：RUN
    函数功能：工作函数
    传入参数：无
    传出数据：0运行成功 非0 运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-22
*/
int lift_task::RUN(){
    int err = 0x00;
    if( err = this->init()){ cout<<"初始化错误码:" << err <<endl; return -0x01; }

    struct stc_package pkg;
    for(;;){
        /*接收数据*/
        if(this->stc_uart->stc_getpkg(&pkg)) break;
        #if DEBUG
        cout<<"接受数据成功"<<endl;
        #endif

        /*系统处理*/
        if(err = this->cmd_contrl(&pkg)) {cout<<"系统处理错误:" << err <<endl; break; }
    }

    return 0x01;
}

/*****************************************************************************************/
/*
    该区域为指令的处理
*/

/*
    函数名称：_cmd_restore
    函数功能：恢复出厂设置
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-25
*/
int lift_task::_cmd_restore(){
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

/*
    函数名称：wirteConfig_toSTC
    函数功能：将配置文件信息更新到stc中
    传入参数：无
    传出数据：
                 0  运行成功 
                -1  连接服务器失败
                -2  发送数据到服务器失败
                -3  从服务器接收数据失败
                其它错误关注tcp服务器指令处理函数的相应错误
    注意事项：内部函数
    编写人员：王凤龙
    编写时间：2016-09-01
*/
static int wirteConfig_toSTC(){
    /*组长发送报文*/
    tcp_package_modle _pakage;
    dataToPakage("{\"cmd\":2}", _pakage);

    /*建立TCP连接*/
    tcp_sock_client _client("127.0.0.1", 9191);
    if(_client.connectToServer()) return -1;

    /*发送数据*/
    if(_client.sendData((unsigned char *)&_pakage, sizeof(tcp_package_modle)) <= 0x00) return -2;

    /*接收数据*/
    int reg = 0x00;
    if((reg =_client.getData((unsigned char *)&_pakage, sizeof(tcp_package_modle))) <= 0x00) return -3;

    /*关闭连接*/
    _client.tcpClose();

    /*解析数据*/
    Json::Value jsonData;
    if(pakageToJson((unsigned char *)&_pakage, reg, jsonData)) return -3;

    return jsonData["error"].asInt();
}

/*
    函数名称：_cmd_liftCont
    函数功能：电梯实时处理
    传入参数：struct stc_package *pkg STC数据包
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-11-22
*/
int lift_task::_cmd_liftCont(struct stc_package *pkg){
    if(!pkg) return -0x01;
    
    /*数据转换*/
    struct lift_real_info *tmp = (struct lift_real_info *)0x00;
    tmp = (struct lift_real_info *)pkg->data;

    /*判断重复*/
    if(
        *((unsigned int *)&this->pkg_bak) == *((unsigned int *)tmp) && 
        ((this->pak_cf++) <= LIFT_STC_CH_MAXNUM) 
    ) return 0x00;
    *((unsigned int *)&this->pkg_bak) = *((unsigned int *)tmp); this->pak_cf = 0x00;

    /*检测楼层信息是否正确*/
    if(tmp->floor >= this->floor_num){
        wirteConfig_toSTC(); sleep(2);      //将楼层信息写入stc
        return -0x02;
    }

    /*获取楼层信息及运行状态,写入共享内存，方便相机获取*/
    this->osdstr.str("");
    this->osdstr << this->jsonConf["title"].asString().c_str()
         << this->jsonConf["floordata"][tmp->floor].asString().c_str() << " ";
    switch(tmp->status){
        case 1: this->osdstr << "上行"; break;
        case 2: this->osdstr << "下行"; break;
        default: this->osdstr<< "停止"; break;
    }

    #if DEBUG
    cout<<"实时数据添加到共享变量"<<endl;
    #endif
    if(this->str_shm->writeStr(this->osdstr.str().c_str())) return -0x03;

    /*加入报警处理*/
    if(this->alarm_cont->add_realData(tmp, this->osdstr.str().c_str())) return -0x04;

    return 0x00;
}

/*****************************************************************************************/