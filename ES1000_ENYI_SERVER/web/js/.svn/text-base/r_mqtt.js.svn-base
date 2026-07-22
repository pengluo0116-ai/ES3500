/*
import mqttws31.min.js
*/

var mt_options = {
    "client" : null,                            //MQTT客户端
    "sip" : null,                               //服务器IP
    "port": 0,                                  //服务器端口
    "topic" : "#",                              //订阅TOPIC
    "username" : null,                          //登录账号
    "userpasd" : null,                          //登录密码
    "cleanSession" : true,                      //是否清楚session
    "useTLS" : false,
    "path" : "/mqtt",
    "keepAliveInterval" : 20,                   //心跳时间
    "reconnectTimeout" : 2000,                  //重连超时时间
    "timeout" : 3,                              //超时时间
    "onMessageArrived" : null                   //数据接收处理函数
};

/*
    函数名称：mt_getClinet
    函数功能：获取mqtt客户端配置信息
    传入参数：
                _sip                            //服务器IP
                _sport                          //服务器端口
                _stopic                         //服务器topic
                _onMessageArrived               //接收数据处理函数
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-20
*/
function mt_getClient(_sip, _sport, _stopic, _onMessageArrived){
    /*服务器参数*/
    mt_options.sip = _sip;
    mt_options.port = _sport;
    mt_options.topic = _stopic;

    /*接收数据处理函数*/
    if(!$.isFunction(_onMessageArrived)){ mt_options.onMessageArrived = null; }
    else mt_options.onMessageArrived = _onMessageArrived;

    mt_init();
}

/*
    函数名称：mt_init
    函数功能：创建mqtt客户端
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-20
*/
function mt_init(){
    /*创建客户端*/
    mt_options.client = new Paho.MQTT.Client(
        mt_options.sip,
        mt_options.port,
        mt_options.path,
        "wcl_" + parseInt(Math.random() * 100, 10)
    );

    /*参数配置*/
    mt_options.client.onConnectionLost = mt_onConnecttionLost;
    if(mt_options.onMessageArrived == null) mt_options.client.onMessageArrived = mt_onMessageArrived;
    else mt_options.client.onMessageArrived = mt_options.onMessageArrived;

    var option = {
        timeout : mt_options.timeout,
        useSSL  : mt_options.useTLS,
        cleanSession : mt_options.cleanSession,
        onSuccess :mt_onConnect,
        keepAliveInterval : mt_options.keepAliveInterval,
        onFailure: function(message){
            console.log("连接服务器失败:" + message.errorMessage);
            setTimeout(mt_init, mt_options.reconnectTimeout);
        }
    };

    if(mt_options.username != null){
        option.userName = mt_options.username;
        option.userPasd = mt_options.userpasd;
    }

    /*连接服务器*/
    mt_options.client.connect(option);
}

/*
    函数名称：mt_onConnect
    函数功能：连接服务器成功处理
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-20
*/
function mt_onConnect(){
    layer.msg("连接服务器成功");
    mt_options.client.subscribe(mt_options.topic, {qos: 0});
}

/*
    函数名称：mt_onConnecttionLost
    函数功能：mqtt客户端连接中断处理
    传入参数：response 连接断响应消息
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-20
*/
function mt_onConnecttionLost(response){
    layer.msg("与服务器断开，自动尝试重新连接...");
    setTimeout(mt_init, mt_options.reconnectTimeout);
}

/*
    函数名称：mt_onMessageArrived
    函数功能：mqtt接收到数据处理
    传入参数：message    服务器返回数据
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-20
*/
function mt_onMessageArrived(message){
    var topic = message.destinationName;
    var payload = message.payloadString;
    console.log("topic:" + topic + " / value:" + payload);
    
}