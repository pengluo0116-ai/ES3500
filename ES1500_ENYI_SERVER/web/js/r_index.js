/*
    实时处理：首页
*/

var CMD_DEV = {
    "group" : {
        "add" : 1,                  //添加小组
        "update" : 2,               //修改小组
        "get": 3,                   //获取小组信息列表
        "del" : 4                   //删除小组
    },

    "dev" :{
        "get" : 5,                  //获取设备信息列表
        "add" : 6,                  //添加设备
        "update" : 7,               //修改设备
        "del" : 8                   //删除设备
    },

    "smt" : 11
}


var rindex_option = {
    "dev_group" :{ 
        "list":null,                //类列表
        "md5" : null                //摘要信息
    },

    "dev_list" :{
        "list" : null,              //类列表
        "md5" : null                //摘要信息
    },

    "checkCyc" : 60000,             //设备检测周期
    "deviation" : 0                 //服务器与本地的时间差（服务器时间-本地时间）
};

/*
    函数名称：QTcmd_group_getAll
    函数功能：获取所有小组信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
function QTcmd_group_getAll(){
    var jsonPost = {};
    jsonPost.display = CMD_DEV.group.get;

    var _load = layer.load();
    $.post(
            "/cgi-bin/dev.cgi",
            JSON.stringify(jsonPost),
            function (data, status, xhr) {
                layer.close(_load);
                if (status != "success") { layer.alert("连接服务器失败"); return; }
                var temp = eval('(' + data + ')');
                switch (parseInt(temp.error)) {
                    case 399: location.href = "login.html"; return;
                    case 0: break;
                    default: layer.alert(temp.data); return;
                }
               rindex_option.dev_group.md5 = rindex_jsonToMD5(temp.data);
               QTcmd_device_getAll(temp.data);
               rindex_getDeviation(xhr.getResponseHeader('Date')); 
            },
        "html");
}

/*
    函数名称：QTcmd_device_getAll
    函数功能：获取设备
    传入参数：groupList  组信息列表
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
function QTcmd_device_getAll(groupList){
    jsonPost = {};
    jsonPost.display = CMD_DEV.dev.get;

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

                tree_init(groupList, temp.data);
                rindex_option.dev_list.md5 = rindex_jsonToMD5(temp.data);
                treeItem_createAll();
                rindex_option_init();
                rindex_checkRemark();
                
                rindex_getMtInfo();
                $("#pframe").attr("src", "/real/lift_index.html");
        },
        "html");
}

/*
    函数名称：rindex_getMtInfo
    函数功能：获取MQTT服务器配置信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rindex_getMtInfo(){
    jsonPost = {};
    jsonPost.display = CMD_DEV.smt;

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
                mt_getClient(temp.ip, temp.port, temp.t, _mt_onMessageArrived);
        },
        "html");
}

/*
    函数名称：rindex_option_init
    函数功能：配置参数初始化
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间:2017-06-20
*/
function rindex_option_init(){
    rindex_option.dev_group.list = lift_group_TABLE;
    rindex_option.dev_list.list = dev_info_TABLE;
}

/*
    函数名称：_mt_onMessageArrived
    函数功能：MQTT接收数据处理
    传入参数：message
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-20
*/
function _mt_onMessageArrived(message){
    /*解析数据*/
    var msgStr = message.payloadString;

    var jsonData = null;
    try{ jsonData = eval('('+ msgStr +')'); }catch(e){ return; }

    /*获取设备类对象*/
    var dev_cobj = get_devInfoItem_btid(jsonData["Parameters"]["ID"]);
    if(dev_cobj == null){ return; }
    
    dev_cobj.jsonReal = jsonData;
}

/*
    函数名称：rindex_GMTtoTimeStamp
    函数功能：格林尼治时间转换成成时间戳
    传入参数：GMTStr 格林尼治时间字符串
    传出数据：<0 运行有误 >=0 时间戳
    注意事项：转换成的数据是本地时间的时间戳
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rindex_GMTtoTimeStamp(GMTStr){
    var GMTDate = null;
    try{ GMTDate = new Date(GMTStr); }catch(e){ return -0x01; }
    return GMTDate.getTime();
}

/*
    函数名称：rindex_nowToTimeStamp
    函数功能：当前时间转换成时间戳
    传入参数：无
    传出数据：时间戳
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rindex_nowToTimeStamp(){
    var NowDate = new Data;
    return NowDate.getTime();
}

/*
    函数名称：rindex_timeStamp_to_DateStr
    函数功能：时间戳转换成本地时间字符串
    传入参数：timeStamp  时间戳
    传出数据：
                字符串     日期时间
                null        运行有误
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rindex_timeStamp_to_DateStr(timeStamp){
    try{
        var newDate = new Date();
        newDate.setTime(timeStamp);
        return newDate.toLocaleString();
    }catch(e){ return null; }
}

/*
    函数名称：rindex_getDeviation
    函数功能：计算服务器时间与本地时间的时间差
    传入参数：GMTStr 格林尼治时间字符串
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rindex_getDeviation(GMTStr){
    /*获取本地时间戳*/
    var nowDate = new Date;

    /*获取服务器时间戳*/
    var serverDateStamp = rindex_GMTtoTimeStamp(GMTStr);
    if(serverDateStamp == -0x01) return;

    /*获取时间差*/
    rindex_option.deviation = serverDateStamp - nowDate.getTime();
}

/*
    函数名称：rindex_getSDateTime
    函数功能：获取服务器时间
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-20
*/
function rindex_getSDateTime(){
    var localDate = new Date();
    return rindex_timeStamp_to_DateStr(localDate.getTime() + rindex_option.deviation);
}

/*
    函数名称：rindex_getRealData
    函数功能：获取实时数据
    传入参数：jonsData   获取到实时数据存储到该引用指向的存储空间
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-20
*/
function rindex_getRealData(){
    var jsonData = {};
    var jsonList = [];

    for(var i=0x00; i<rindex_option.dev_list.list.length; i++){
        if(rindex_option.dev_list.list[i].jsonReal == null) continue;
        jsonList.push(rindex_option.dev_list.list[i].jsonReal);
    }

    jsonData["datetime"] = rindex_getSDateTime();
    jsonData["data"] = jsonList;
    jsonData["error"] = 0;

    return jsonData;
}

/*
    函数名称：rindex_jsonToMD5
    函数功能：json数据转换成MD5
    传入参数：jsonData
    传出数据：
                字符串     生成的MD5字符串
                null        错误 
    注意事项：需要添加MD5库
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rindex_jsonToMD5(jsonData){
    try{
        var _jsonDataStr = JSON.stringify(jsonData);
        var hash = md5(_jsonDataStr);
        return hash;
    }catch(e){ return null; }
}

/*
    函数名称：rindex_checkGroupRemark
    函数功能：检测设备组信息摘要
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rindex_checkGroupRemark(){
    var jsonPost = {};
    jsonPost.display = CMD_DEV.group.get;

    $.post(
            "/cgi-bin/dev.cgi",
            JSON.stringify(jsonPost),
            function (data, status, xhr) {
                if (status != "success") { return; }
                var temp = eval('(' + data + ')');
                switch (parseInt(temp.error)) {
                    case 399: location.href = "login.html"; return;
                    case 0: break;
                    default: layer.alert(temp.data); return;
                }
               var hash = rindex_jsonToMD5(temp.data);
               if(rindex_option.dev_group.md5 != hash) location.href = location.href;
               rindex_getDeviation(xhr.getResponseHeader('Date'));
            },
        "html");
}

/*
    函数名称：rindex_checkDeviceRemark
    函数功能：检测设备信息摘要
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rindex_checkDeviceRemark(){
    var jsonPost = {};
    jsonPost.display = CMD_DEV.dev.get;

    $.post(
            "/cgi-bin/dev.cgi",
            JSON.stringify(jsonPost),
            function (data, status) {
                if (status != "success") { return; }
                var temp = eval('(' + data + ')');
                switch (parseInt(temp.error)) {
                    case 399: location.href = "login.html"; return;
                    case 0: break;
                    default: return;
                }
                var hash = rindex_jsonToMD5(temp.data);
                if(rindex_option.dev_list.md5 != hash) location.href = location.href;
        },
        "html");
}

/*
    函数名称：rindex_getRemark
    函数功能：获取设备组和设备的摘要信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-06-21
*/
function rindex_checkRemark(){
    try{
        rindex_checkGroupRemark();
        rindex_checkDeviceRemark();
    }catch(e){}

    setTimeout(rindex_checkRemark, rindex_option.checkCyc);
}