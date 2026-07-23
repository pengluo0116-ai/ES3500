#include "mq_collecter.h"
using namespace std;
#define DEBUG 1

/*
    函数名称：mq_collecter
    函数功能：构造函数
    传入参数：
                const char *_addr       MQ服务器地址
                const char *_clientID   客户端ID
                const char *_topic      订阅主题
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-02-28
*/
mq_collecter::mq_collecter(const char *_addr, const char *_clientID, const char *_topic):
addr(_addr), clientID(_clientID), topic(_topic), conn_opts(MQTTClient_connectOptions_initializer){
}

/*
    函数名称：~mq_collecter
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-02-28
*/
mq_collecter::~mq_collecter(){
    /*关闭并且销毁客户端*/
    this->_close();
}

/*
    函数名称：_init
    函数功能：初始化
    传入参数：无
    传出数据：
                 0  运行成功
                -1  参数校验非法
                 1  创建客户端失败
                 2  连接服务器失败
                 3  订阅主题失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-02-28
*/
int mq_collecter::_init(){
    int err = 0x00;

    /*参数校验*/
    if(
        this->addr.size() <=0x00 ||
        this->clientID.size() <=0x00 ||
        this->topic.size() <= 0x00 
    ) return -0x01;

    /*创建客户端*/
    err = MQTTClient_create(
        &this->client,
        this->addr.c_str(),
        this->clientID.c_str(),
        MQTTCLIENT_PERSISTENCE_NONE,
        NULL
    );
    if(err){
        #if DEBUG
        cout<< "MQTT创建客户端失败[" << err << "]" <<endl;
        #endif
        return 0x01;
    }

    /*连接服务器*/
    this->conn_opts.keepAliveInterval = 20;
    this->conn_opts.cleansession = 1;
    err = MQTTClient_connect(this->client, &this->conn_opts);
    if(err){
        #if DEBUG
        cout<< "MQTT连接服务器失败[" << err << "]" <<endl;
        #endif
        return 0x02;
    }

    /*订阅主题--至少一次接收配置*/
    err = MQTTClient_subscribe(this->client, (char *)this->topic.c_str(), 0x01);
    if(err){
        #if DEBUG
        cout<< "MQTT订阅失败[" << err << "]" <<endl;
        #endif
        return 0x03;
    }

    return 0x00;
}

/*
    函数名称：_recvCont
    函数功能：接收并且处理
    传入参数：无
    传出数据：
    注意事项：将接收到的数据存入数据容器中
    编写人员：王凤龙
    编写时间：2017-02-28
*/
int mq_collecter::_recvCont(){
    int err = 0x00;
    
    /*判断是否连接成功*/
    if(!MQTTClient_isConnected(this->client)) return -0x01;

    /*接收数据*/
    char *topicName;
    int topicName_len;
    char recvData[ MQTTCLIENT_DATABUFF_MAXSIZE ] = { 0x00 };
    MQTTClient_message *get_msg = (MQTTClient_message *)0x00;

    for(;;){
        get_msg = (MQTTClient_message *)0x00;
        err = MQTTClient_receive(this->client, &topicName, &topicName_len, &get_msg, MQTT_RECV_TIMOUT);

        switch(err){
            case MQTTCLIENT_SUCCESS: break;                             //运行成功
            case MQTTCLIENT_TOPICNAME_TRUNCATED: continue;              //超时
            default: return -0x02;
        }

        /*处理数据*/
        if(!get_msg) continue;
        memset((void *)recvData, 0x00, MQTTCLIENT_DATABUFF_MAXSIZE);
        memcpy((void *)recvData, (void *)get_msg->payload, get_msg->payloadlen);
        MQTTClient_freeMessage(&get_msg);

        cout<< recvData <<endl;
        if(lift_vector::put_msg(string(recvData))) return 0x01;         //添加进数据容器
    }

    return 0x00;
}

/*
    函数名称：_close
    函数功能：关闭连接并且销毁客户端
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-02-28
*/
int mq_collecter::_close(){
    if(MQTTClient_isConnected(this->client)){
        MQTTClient_disconnect(this->client, 10000);
        MQTTClient_destroy(&this->client);
    }

    return 0x00;
}

/*
    函数名称：RUN
    函数功能：工作函数
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-02-28
*/
int mq_collecter::RUN(){
    int err = 0x00;

    /*初始化*/
    err = this->_init();
    if(err){
        #if DEBUG
        cout<< "MQTT初始化失败[" << err << "]" <<endl;
        #endif
        goto MQTT_RUN;
    }
    
    /*接收并且处理*/
    err = this->_recvCont();
    #if DEBUG
    cout<< "MQTT数据接收失败[" << err << "]" <<endl;
    #endif

    MQTT_RUN:
    this->_close();
    exit(-1);

    return err;
}
