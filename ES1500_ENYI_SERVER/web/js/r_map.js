
/*控制体类*/
function DEV_CONT(){
    this.serverDate = "2010-01-01 12:00:00";            //系统时间
    this.now_date = 0;                                  //当前系统时间转换成秒数
}

var rmap_option = {
    liftList : [],                                      //设备列表属性信息
    dev_cont : null,                                    //设备控制对象
    md5 : null,                                         //设备摘要
    cmd : {
        dev_get : 5,                                    //获取设备信息
        smt: 11                                         //获取mqtt配置信息
    },

    checkCyc : 60000,                                   //设备检测周期
    deviation : 0                                       //服务器与本地的时间差
};

/*
    函数名称：cmd_getDev
    函数功能：获取设备信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-10-18
*/
function cmd_getDev() {
    var postJson = {};
    postJson.display = rmap_option.cmd.dev_get;
    var _load = layer.load();

    $.post(
        "/cgi-bin/dev.cgi",
        JSON.stringify(postJson),
        function (data, status, xhr) {
            layer.close(_load);
            if (status != "success") { layer.alert("连接服务器失败"); return; }
            var temp = eval('(' + data + ')');
            switch (parseInt(temp.error)) {
                case 399: location.href = "login.html"; return;
                case 0: break;
                case 101: layer.alert(temp.data); return;
                default: layer.alert(temp.data); return;
            }
            rmap_option.md5 =  rmap_jsonToMD5(temp.data); 
            map_load(temp.data);
            if(temp.data.length > 0x00) rmap_option.liftList =temp.data;
            rmap_liftList_realInit(rmap_option.liftList);
            rmap_getDeviation(xhr.getResponseHeader('Date'));

            rmap_getMtInfo();
            rmap_showReal();
            rmap_checkRemark();
        },
        "html");
}

/*
    函数名称：rmap_liftList_realInit
    函数功能：设备列表实时数据初始化
    传入参数：jsonLiftList   设备属性信息列表
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rmap_liftList_realInit(jsonLiftList){
    for(var i=0x00; i<jsonLiftList.length; i++){ jsonLiftList[i]["real"] = null; }
}

/*
    函数名称：rmap_setLiftReal
    函数功能：设置指定设备的实时数据
    传入参数：
                id          设备数据
                jsonReal    实时数据
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function ramp_setLiftReal(id, jsonReal){
    for(var i=0x00; i<rmap_option.liftList.length; i++){
        if(rmap_option.liftList[i]["id"] != id) continue;

        rmap_option.liftList[i]["real"] = jsonReal;
        break;
    }
}

/*
    函数名称：rmap_mt_onMessageArrived
    函数功能：MQTT接收数据处理
    传入参数：message
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rmap_mt_onMessageArrived(message){
    /*解析数据*/
    var msgStr = message.payloadString;

    var jsonData = null;
    try{ jsonData = eval('('+ msgStr +')'); }catch(e){ return; }

    /*设置设备的实时数据*/
    ramp_setLiftReal(jsonData["Parameters"]["ID"], jsonData);
}

/*
    函数名称：rmap_getMtInfo
    函数功能：获取MQTT服务器配置信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rmap_getMtInfo(){
    jsonPost = {};
    jsonPost.display = rmap_option.cmd.smt;

    var _load = layer.load();
    $.post(
            "/cgi-bin/dev.cgi",
            JSON.stringify(jsonPost),
            function (data, status) {
                layer.close(_load);
                if (status != "success") { layer.alert("连接服务器失败"); return; }
                var temp = eval('(' + data + ')');
                switch (parseInt(temp.error)) {
                    case 399: location.href = "login.html"; return;
                    case 0: break;
                    default: layer.alert(temp.data); return;
                }
                
                /*创建MQTT客户端*/
                mt_getClient(temp.ip, temp.port, temp.t, rmap_mt_onMessageArrived);
        },
        "html");
}

/*
    函数名称：rmap_GMTtoTimeStamp
    函数功能：格林尼治时间转换成成时间戳
    传入参数：GMTStr 格林尼治时间字符串
    传出数据：<0 运行有误 >=0 时间戳
    注意事项：转换成的数据是本地时间的时间戳
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rmap_GMTtoTimeStamp(GMTStr){
    var GMTDate = null;
    try{ GMTDate = new Date(GMTStr); }catch(e){ return -0x01; }
    return GMTDate.getTime();
}

/*
    函数名称：rmap_nowToTimeStamp
    函数功能：当前时间转换成时间戳
    传入参数：无
    传出数据：时间戳
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rmap_nowToTimeStamp(){
    var NowDate = new Data;
    return NowDate.getTime();
}

/*
    函数名称：rmap_timeStamp_to_DateStr
    函数功能：时间戳转换成本地时间字符串
    传入参数：timeStamp  时间戳
    传出数据：
                字符串     日期时间
                null        运行有误
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rmap_timeStamp_to_DateStr(timeStamp){
    try{
        var newDate = new Date();
        newDate.setTime(timeStamp);
        return newDate.toLocaleString();
    }catch(e){ return null; }
}

/*
    函数名称：rmap_getDeviation
    函数功能：计算服务器时间与本地时间的时间差
    传入参数：GMTStr 格林尼治时间字符串
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rmap_getDeviation(GMTStr){
    /*获取本地时间戳*/
    var nowDate = new Date;

    /*获取服务器时间戳*/
    var serverDateStamp = rmap_GMTtoTimeStamp(GMTStr);
    if(serverDateStamp == -0x01) return;

    /*获取时间差*/
    rmap_option.deviation = serverDateStamp - nowDate.getTime();
}

/*
    函数名称：rmap_getSDateTime
    函数功能：获取服务器时间
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rmap_getSDateTime(){
    var localDate = new Date();
    return rmap_timeStamp_to_DateStr(localDate.getTime() + rmap_option.deviation + 10000);
}

/*
    函数名称：rmap_jsonToMD5
    函数功能：json数据转换成MD5
    传入参数：jsonData
    传出数据：
                字符串     生成的MD5字符串
                null        错误 
    注意事项：需要添加MD5库
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rmap_jsonToMD5(jsonData){
    try{
        var _jsonDataStr = JSON.stringify(jsonData);
        var hash = md5(_jsonDataStr);
        return hash;
    }catch(e){ return null; }
}

/*
    函数名称：rmap_getRealData
    函数功能：获取实时数据
    传入参数：无
    传出数据：实时数据 josn
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rmap_getRealData(){
    var jsonData = {};
    var jsonList = [];

    for(var i=0x00; i<rmap_option.liftList.length; i++){
        if(rmap_option.liftList[i]["real"] == null) continue;
        jsonList.push(rmap_option.liftList[i]["real"]);
    }

    jsonData["datetime"] = rmap_getSDateTime();
    jsonData["data"] = jsonList;
    jsonData["error"] = 0;

    return jsonData;
}

/*
    函数名称：rmap_realShow
    函数功能：展示实时数据
    传出参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rmap_showReal(){
    try{
        var jsonData = rmap_getRealData();
        show_realData(jsonData);
    }catch(e){}
    setTimeout(rmap_showReal, 1000);
}

/*
    函数名称：rmap_checkDeviceRemark
    函数功能：检测设备摘要信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rmap_checkDeviceRemark(){
    var jsonPost = {};
    jsonPost.display = rmap_option.cmd.dev_get;

    $.post(
            "/cgi-bin/dev.cgi",
            JSON.stringify(jsonPost),
            function (data, status, xhr) {
                if (status != "success") { return; }
                var temp = eval('(' + data + ')');
                switch (parseInt(temp.error)) {
                    case 399: location.href = "login.html"; return;
                    case 0: break;
                    default: return;
                }
                var hash = rmap_jsonToMD5(temp.data);
                if(rmap_option.md5 != hash) location.href = location.href;
                rmap_getDeviation(xhr.getResponseHeader('Date'));
        },
        "html");
}

/*
    函数名称：rmap_checkRemark
    函数功能：获取设备摘要信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rmap_checkRemark(){
    try{ rmap_checkDeviceRemark(); }catch(e){} 
    setTimeout(rmap_checkRemark, rmap_option.checkCyc);
}