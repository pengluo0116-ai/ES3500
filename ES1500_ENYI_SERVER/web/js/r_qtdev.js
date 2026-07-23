/*
    设备控制
    使用该脚本时，需要加载jquery脚本
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
    }
}


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
               QTcmd_device_getAll(temp.data);
               alert(xhr.getResponseHeader('Date'));
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
                treeItem_createAll();
                $("#pframe").attr("src", "/real/lift_index.html");
        },
        "html");
}
