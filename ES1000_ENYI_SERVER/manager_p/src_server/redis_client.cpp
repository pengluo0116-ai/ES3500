#include "redis_client.h"
using namespace std;
#define DEBUG 0

/*
    REDIS_REPLY_STATUS：
    返回执行结果为状态的命令。比如set命令的返回值的类型是REDIS_REPLY_STATUS，然后只有当返回信息是"OK"时，才表示该命令执行成功。可以通过reply->str得到文字信息，通过reply->len得到信息长度。
    
    REDIS_REPLY_ERROR：
    返回错误。错误信息可以通过reply->str得到文字信息，通过reply->len得到信息长度。

    REDIS_REPLY_INTEGER：
    返回整型标识。可以通过reply->integer变量得到类型为long long的值。
    
    REDIS_REPLY_NIL:
    返回nil对象，说明不存在要访问的数据。
    
    REDIS_REPLY_STRING:
    返回字符串标识。可以通过reply->str得到具体值，通过reply->len得到信息长度。
    
    REDIS_REPLY_ARRAY:
    返回数据集标识。数据集中元素的数目可以通过reply->elements获得，每个元素是个redisReply对象，元素值可以通过reply->element[..index..].*形式获得，用在获取多个数据结果的操作。
*/

/*
    函数名称：redis_client
    函数功能：构造函数
    传入参数：
                const char *_sip    服务器ip
                int _port           服务器端口
                const char *_pasd   服务器密码
    传出数据：无
    注意事项：如果redis没有设置密码，_pasd可以设置为NULL
    编写人员：王凤龙
    编写时间：2016-10-13
*/
redis_client::redis_client(const char *_sip, int _port, const char *_pasd):
sip(string(_sip)), port(_port), pasd(string(_pasd ? _pasd : "")), client((redisContext *)0x00), replay((redisReply *)0x00){}

/*
    函数名称：~redis_client
    函数功能：析构函数
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-13
*/
redis_client::~redis_client(){
    this->closeClient();
}

/*
    函数名称：loginSetPasd
    函数功能：提交登录密码
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-13
*/
int redis_client::loginSetPasd(){
    /*判断是否需要密码校验*/
    if(this->pasd.size() <= 0x00) return 0x00; 

    /*发送指令*/
    if(this->replay) { freeReplyObject(this->replay); this->replay = (redisReply *)0x00; }
    this->replay = (redisReply*)redisCommand(this->client, "AUTH %s", this->pasd.c_str());

    /*对返回结果进行校验*/
    if(!this->replay){
        #if DEBUG
        cout<< "校验登录密码失败" <<endl;
        #endif
        return -0x03;
    }

    if(this->replay->type == REDIS_REPLY_ERROR){
        #if DEBUG
        cout<< "登录密码错误" <<endl;
        #endif
        return -0x04;
    }

    return 0x00;
}

/*
    函数名称：createClient
    函数功能：创建客户端并且连接到服务器
    传入参数：无
    传出数据：
                 -1     服务器配置数据有误
                 -2     连接服务器失败
                  0     运行成功
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-13
*/
int redis_client::createClient(){
    /*数据校验*/
    if(this->sip.size()<=0x00 || this->port==0x00) return -0x01;

    /*创建客户端*/
    this->closeClient();
    this->client = redisConnect(this->sip.c_str(), this->port);
    if(!this->client || this->client->err){ 
        #if DEBUG
        cout<< "创建客户端失败" <<endl;
        #endif

        this->closeClient(); return -0x02; 
    }

    /*登录密码校验*/
    return this->loginSetPasd();
}

/*
    函数名称：closeClient
    函数功能：关闭客户端
    传入参数：无
    传出数据：默认 0x00
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-13
*/
int redis_client::closeClient(){
    #if DEBUG
    cout<<"关闭redis客户端"<<endl;
    #endif

    if(this->client) { redisFree(this->client); this->client = (redisContext *)0x00; }
    if(this->replay) { freeReplyObject(this->replay); this->replay = (redisReply *)0x00; }

    return 0x00;
}

/*
    函数名称：setStr
    函数功能：设置key值
    传入参数：
                const char *_keys       key名称
                const char *_data       要设置的数据
    传出数据：
                -2      参数校验有误
                -3      客户端未连接
                 1      设置key失败
                 0      运行成功
    注意事项：设置的内容为字符串内容
    编写人员：王凤龙
    编写时间：2016-10-13
*/
int redis_client::setStr(const char *_keys, const char *_data){
    /*参数校验*/
    if(!_keys || strlen(_keys)<=0x00  || !_data) return -0x02;

    /*校验客户端是否连接*/
    if(!this->client) return -0x03;

    /*设置数据*/
    if(this->replay) { freeReplyObject(this->replay); this->replay = (redisReply *)0x00; }
    this->replay = (redisReply*)redisCommand(this->client, "SET %s %s", _keys, _data);

    if(!this->replay){
        #if DEBUG
        cout<< "设置KEY("<< _keys <<")失败" <<endl;
        #endif
        return 0x01;
    }

    if(!(this->replay->type == REDIS_REPLY_STATUS && strcasecmp(this->replay->str, "OK")==0x00)){
        #if DEBUG
        cout<< "设置KEY("<< _keys <<")失败" <<endl;
        #endif
        return 0x01;
    }

    return 0x00;
}

/*
    函数名称：getStr
    函数功能：获取key值
    传入参数：
                const char *_keys       key名称 
                char *_dataBuff         获取数据后，存储到该指针指向的缓冲区
                int _len                缓冲区大小
    传出数据：
                -1      不存在该key
                -2      参数有误
                -3      客户端未连接
                -4      操作失败
                -5      未知错误
                >=0     返回获取数据的长度
    注意事项：
    编写人员：王凤龙
    编写时间：2016-10-13
*/
int redis_client::getStr(const char *_keys, char *_dataBuff, int _len){
    /*参数校验*/
    if(!_keys || strlen(_keys)<=0x00 || !_dataBuff || _len<=0x00) return -0x02;

    /*校验客户端是否连接*/
    if(!this->client) return -0x03;

    /*发送 获取key值 命令*/
    if(this->replay) { freeReplyObject(this->replay); this->replay = (redisReply *)0x00; }
    this->replay = (redisReply*)redisCommand(this->client, "GET %s", _keys);

    /*对返回结果进行校验*/
    if(!this->replay){
        #if DEBUG
        cout<< "获取KEY("<< _keys <<")失败" <<endl;
        #endif
        return -0x04;
    }

    int err = 0x00;
    switch(this->replay->type){
        case REDIS_REPLY_STRING:    break;                  //返回数据为字符串
        case REDIS_REPLY_NIL:       err = -0x01; break;     //不存在该key值
        case REDIS_REPLY_ERROR:     err = -0x04; break;     //操作失败
        default: err = -0x05; break;                        //未知错误
    }

    if(err) return err;

    if(_len <= (int)this->replay->len) return -0x05;         //判断缓冲区是否充足
    memset((void *)_dataBuff, 0x00, _len);
    memcpy((void *)_dataBuff, (void *)this->replay->str, (int)this->replay->len);

    return (int)this->replay->len;
}

/*
    函数名称：getKeys
    函数功能：获取符合条件的所有key名称
    传入参数：
                const char *_keys                                       key名称 例如：* ,test*
                int (*getKey_callBack)(void *, const char *, int)       回调函数
    传出数据：
                -2          参数有误
                -3          客户端未连接
                -4          操作失败
                -5          未知错误
                <-5         回调函数的返回值错误
                >=0         获取到keys的个数
    注意事项：_keys 可以是模糊键值 例如test*
    编写人员：王凤龙
    编写时间：2016-10-13

    回调函数:
    int (*getKey_callBack)(
        void *                  用户自定义参数
        const char *            传入的key值
        int                     key值得字符串长度
    )
    为了区分调用该函数的父函数体的返回值，建议该返回值取值范围<-5
*/
int redis_client::getKeys(
    const char *_keys, 
    int (*getKey_callBack)(void *, const char *, int),
    void *data_callBack
){
    /*参数校验*/
    if(!getKey_callBack) return -0x02;

    /*校验客户端是否连接*/
    if(!this->client) return -0x03;

    /*获取key搜索值*/
    const char *keys = (char *)0x00;
    keys = (_keys && strlen(_keys)) ? _keys : "*";

    /*发送指令*/
    if(this->replay) { freeReplyObject(this->replay); this->replay = (redisReply *)0x00; }
    this->replay = (redisReply*)redisCommand(this->client, "KEYS %s", keys);

    /*对返回结果进行校验*/
    if(!this->replay){
        #if DEBUG
        cout<< "获取KEY名称列表("<< keys <<")失败" <<endl;
        #endif
        return -0x04;
    }

    int err = 0x00;
    switch(this->replay->type){
        case REDIS_REPLY_ARRAY: break;                      //获取到数据集
        case REDIS_REPLY_ERROR: err = -0x04; break;         //操作失败
        default: err = -0x05; break;                        //位置错误
    }
    if(err) return err;

    for(unsigned int i=0x00; i<(unsigned int)this->replay->elements; i++){
        err = getKey_callBack(data_callBack, this->replay->element[i]->str, this->replay->element[i]->len);
        if(err) return err;
    }

    err = (int)this->replay->elements;
    freeReplyObject(this->replay); this->replay = (redisReply *)0x00;
    return err;
}

/*
    函数名称：delKey
    函数功能：删除key键
    传入参数：const char *_keys
    传出数据：
                -2          参数有误
                -3          客户端未连接
                -4          操作失败
                -5          未知错误
    注意事项：只能删除单个key
    编写人员：王凤龙
    编写时间：2016-10-13
*/
int redis_client::delKey(const char *_keys){
    /*参数校验*/
    if(!_keys || strlen(_keys)<=0x00) return -0x02;

    /*校验客户端是否连接*/
    if(!this->client) return -0x03;

    /*发送指令*/
    if(this->replay) { freeReplyObject(this->replay); this->replay = (redisReply *)0x00; }
    this->replay = (redisReply*)redisCommand(this->client, "DEL %s", _keys);

    /*对返回结果进行校验*/
    if(!this->replay){
        #if DEBUG
        cout<< "删除KEY("<< _keys <<")失败" <<endl;
        #endif
        return -0x04;
    }

    int err = 0x00;
    switch(this->replay->type){
        case REDIS_REPLY_INTEGER: break;
        case REDIS_REPLY_ERROR: err = -0x04; break;         //操作失败
        default: err = -0x05; break;                        //未知错误
    }
    if(err) return err;

    return (int)this->replay->integer;
}
