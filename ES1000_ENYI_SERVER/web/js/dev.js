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
    函数名称：cmd_group_add
    函数功能：添加小组
    传入参数：obj_id  list_组ID
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-22
*/
function cmd_group_add(obj_id){
    var gid = obj_id.replace("list_", "");
    if(isNaN(gid)){ 
        layer.alert("页面加载有误");
        location.href = location.href; 
        return; 
    }

    jsonGroup = {"display":CMD_DEV.group.add, "pid":~~gid, "name":"新建小组"};

    var _load = layer.load();
    $.post(
            "cgi-bin/dev.cgi",
            JSON.stringify(jsonGroup),
            function (data, status) {
                layer.close(_load);
                if (status != "success") { layer.alert("连接服务器失败"); return; }
                var temp = eval('(' + data + ')');
                switch (parseInt(temp.error)) {
                    case 399: location.href = "login.html"; return;
                    case 0: break;
                    default: layer.alert(temp.data); return;
                }

                /*添加至树形列表*/
                add_group(temp.data);

                /*添加右键事件*/
                if(!~~gid){
                    $("#list_" + temp.data.id).contextMenu("pg_rcont",{
                        bindings:{
                            "pg_adddev" : g_adddev_callback,
                            "pg_updatename" : g_updatename_callback,
                            "pg_addgroup" : pg_addgroup_callback,
                            "pg_del" : g_del
                        }
                    });
                }
                else{
                    $("#list_" + temp.data.id).contextMenu("g_rcont",{
                        bindings:{
                            "g_adddev" : g_adddev_callback,
                            "g_updatename" : g_updatename_callback,
                            "g_del": g_del
                        }
                    });
                }
            },
            "html");
}

/*
    函数名称：cmd_group_update
    函数功能：修改小组名称
    传入参数：
                id      组id
                name    组名称
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
function cmd_group_update(id, name){
    var class_obj = get_group_classOBJ(id);
    if(class_obj == null){ alert("数据加载有误，请刷新页面"); return; }
    if(class_obj.name == name){
        $("#list_" + id).css("display", "block");
        $("#gname_" + id).remove();
    }

    var jsonGroup = {};
    jsonGroup.display = CMD_DEV.group.update;
    jsonGroup.id = ~~id;
    jsonGroup.name = name;
    jsonGroup.pid = class_obj.pid;

    var _load = layer.load();
    $.post(
            "cgi-bin/dev.cgi",
            JSON.stringify(jsonGroup),
            function (data, status) {
                layer.close(_load);
                if (status != "success") { layer.alert("连接服务器失败"); return; }
                var temp = eval('(' + data + ')');
                switch (parseInt(temp.error)) {
                    case 399: location.href = "login.html"; return;
                    case 0: break;
                    default: layer.alert(temp.data); return;
                }

                $("#list_" + id).css("display", "block");
                $("#gname_" + id).remove();
                update_group(jsonGroup);
            },
        "html");
}

/*
    函数名称：cmd_group_getAll
    函数功能：获取所有小组信息
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
function cmd_group_getAll(){
    var jsonPost = {};
    jsonPost.display = CMD_DEV.group.get;

    var _load = layer.load();
    $.post(
            "cgi-bin/dev.cgi",
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
                // layer.alert("成功");
                cmd_device_getAll(temp.data);
            },
        "html");
}

/*
    函数名称：cmd_group_del
    函数功能：删除组
    传入参数：obj_id 控件ID
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
function cmd_group_del(obj_id){
    var id = obj_id.replace("list_", "");
    var jsonPost = {};
    jsonPost.display = CMD_DEV.group.del;
    jsonPost.id = ~~id;

    var _load = layer.load();
    $.post(
            "cgi-bin/dev.cgi",
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
               
                del_group(id);
                location.href = location.href; 
            },
        "html");
}

/*
    函数名称：cmd_device_getAll
    函数功能：获取设备
    传入参数：groupList  组信息列表
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
function cmd_device_getAll(groupList){
    jsonPost = {};
    jsonPost.display = CMD_DEV.dev.get;

    var _load = layer.load();
    // layer.alert("ceshi1");
    $.post(
            "cgi-bin/dev.cgi",
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
                // layer.alert("ceshi");
                tree_init(groupList, temp.data);
                treeItem_createAll();
        },
        "html");
}

/*
    函数名称：mtreeView_create
    函数功能：创建树形图
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
function mtreeView_create(){
    cmd_group_getAll();
}

/**************互动**************/

/*
    函数名称：cont_devInfo_check
    函数功能：设备信息校验
    传入参数：id_head    //ID前缀
    传出数据：true 校验成功 false 校验失败
    注意事项：添加设备，修改设备时可以使用该函数进行校验
    编写人员：王凤龙
    编写时间：2017-05-24
*/
function cont_devInfo_check(id_head){
    var tmp = "";
    var obj = null;
    var postData = {};

    /*组ID*/
    obj = $("#" + id_head + "gid");
    postData.gid = ~~trimStr(obj.val());

    /*设备ID*/
    obj = $("#" + id_head + "id");
    tmp = trimStr(obj.val());
    if(tmp=="" || tmp.length<=0x00){ layer.tips("设备ID为空", "#" + id_head + "id"); return null; }
    postData.id = tmp;

    /*设备名称*/
    obj = $("#" + id_head + "name");
    tmp = trimStr(obj.val());
    if(tmp=="" || tmp.length<=0x00){ layer.tips("设备名称为空", "#" + id_head + "name"); return null; }
    postData.name = tmp;

    /*url*/
    postData.url = "";
    
    /*x*/
    obj = $("#" + id_head + "x");
    postData.x = ~~obj.val();

    /*y*/
    obj = $("#" + id_head + "y");
    postData.y = ~~obj.val();

    /*madein--生成厂商*/
    obj = $("#" + id_head + "madein");
    postData.madein = trimStr(obj.val());

    /*property--物业公司*/
    obj = $("#" + id_head + "property");
    postData.property = trimStr(obj.val());

    /*mainter--维保公司*/
    obj = $("#" + id_head + "mainter");
    postData.mainter = trimStr(obj.val());

    /*mainter_tel--维保电话*/
    obj = $("#" + id_head + "mainter_tel");
    postData.mainter_tel = trimStr(obj.val());

    /*mainter_period--维保周期*/
    obj = $("#" + id_head + "mainter_period");
    postData.mainter_period = obj.val();

    /*address--地理位置*/
    obj = $("#" + id_head + "address");
    postData.address = trimStr(obj.val());

    /*备注信息*/
    obj = $("#" + id_head + "note");
    postData.note = trimStr(obj.val());

    /*上一次维保时间，默认值为0.0*/ 
    // if(id_head == "dct_adddev_"){
    //     postData.mainter_time = "0";
    // }
    

    return postData;
}

/*
    函数名称：cont_device_addInit
    函数功能：添加设备初始化
    传入参数：group_objID  list_组ID
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
function cont_device_addInt(group_objID){
    var id = ~~group_objID.replace("list_", "");
    var class_obj = get_group_classOBJ(id);
    if(class_obj == null){ alert("页面加载数据有误，请刷新页面"); return; }

    $("#browser").find("a").css("color", "");
    $(".tdr_cont").css("display", "none");
    $("#dct_adddev_gid").val(id);                   //组ID
    $("#dct_adddev_gname").html(class_obj.name);     //组名称
    $("#dct_adddev_id").val("");                    //设备ID
    $("#dct_adddev_name").val("");                  //设备名称
    $("#dct_adddev_madein").val("");                //电梯厂商
    $("#dct_adddev_property").val("");              //物业公司
    $("#dct_adddev_mainter").val("");               //维保公司
    $("#dct_adddev_mainter_tel").val("");           //维保电话
    $("#dct_adddev_mainter_period").val("");        //维保时间
    $("#dct_adddev_address").val("");               //地理位置
    $("#dct_adddev_note").val("");                  //备注信息
    $("#dct_adddev_x").val("0");                    //X坐标
    $("#dct_adddev_y").val("0");                    //Y坐标
    $("#dct_adddev").css("display", "block");
}

/*
    函数名称：cont_device_addSubmit
    函数功能：添加设备
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-24
*/
function cont_device_addSubmit(){
    var postData = cont_devInfo_check("dct_adddev_");
    if(postData == null || postData.length<=0x00) return;

    postData.display = CMD_DEV.dev.add;
    var _load = layer.load();
    $.post(
            "cgi-bin/dev.cgi",
            JSON.stringify(postData),
            function (data, status) {
                layer.close(_load);
                if (status != "success") { layer.alert("连接服务器失败"); return; }
                var temp = eval('(' + data + ')');
                switch (parseInt(temp.error)) {
                    case 399: location.href = "login.html"; return;
                    case 0: break;
                    default: layer.alert(temp.data); return;
                }

                add_dev(postData);
                cont_device_addInt("list_" + postData.gid);
                $("#dev_" + postData.id).contextMenu("d_rcont", {
                    bindings:{
                        "d_del" : d_del_callback
                    }
                });
        },
        "html");
}

/*
    函数名称：cont_device_udpInt
    函数功能：修改设备属性初始化
    传入参数：设备ID
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-25
*/
function cont_device_udpInt(id){
    /*获取设备类对象*/
    var class_obj_dev = get_dev_classOBJ(id);
    if(class_obj_dev == null){ alert("页面加载数据有误，请刷新页面"); return; }
    
    /*获取组对象*/
    var  class_obj_group = get_group_classOBJ(class_obj_dev.id_parentGroup);
    if(class_obj_group == null){ alert("页面加载数据有误，请刷新页面"); return; }

    /*初始化*/
    $("#browser").find("a").css("color", "");
    $(".tdr_cont").css("display", "none");
    $("#dct_upddev_gid").val(class_obj_group.id);                       //组ID
    $("#dct_upddev_gname").html(class_obj_group.name);                  //组名称
    $("#dct_upddev_id").val(class_obj_dev.id);                          //设备ID
    $("#dct_upddev_name").val(class_obj_dev.name);                      //设备名称
    $("#dct_upddev_madein").val(class_obj_dev.madein);                  //电梯厂商
    $("#dct_upddev_property").val(class_obj_dev.property);              //物业公司
    $("#dct_upddev_mainter").val(class_obj_dev.mainter);                //维保公司
    $("#dct_upddev_mainter_tel").val(class_obj_dev.mainter_tel);        //维保电话
    $("#dct_upddev_mainter_period").val(class_obj_dev.mainter_period);  //维保周期
    $("#dct_upddev_address").val(class_obj_dev.address);                //地理位置
    $("#dct_upddev_note").val(class_obj_dev.note);                      //备注信息
    $("#dct_upddev_x").val(class_obj_dev.x);                            //X坐标
    $("#dct_upddev_y").val(class_obj_dev.y);                            //Y坐标
    $("#dct_upddev_point").html('<a href="javascript:;" onclick="map_pageCont(\''+ id +'\')">地图位置查看设置</a>');
    $("#dct_upddev").css("display", "block");
    $("#dev_" + id).find("a").css("color", "green");
}

/*
    函数名称：cont_device_udpSubmit
    函数功能：修改设备
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-25
*/
function cont_device_udpSubmit(){
    var postData = cont_devInfo_check("dct_upddev_");
    if(postData == null || postData.length <= 0x00) return;

    postData.display = CMD_DEV.dev.update;
    var _load = layer.load();
    $.post(
            "cgi-bin/dev.cgi",
            JSON.stringify(postData),
            function (data, status) {
                layer.close(_load);
                if (status != "success") { layer.alert("连接服务器失败"); return; }
                var temp = eval('(' + data + ')');
                switch (parseInt(temp.error)) {
                    case 399: location.href = "login.html"; return;
                    case 0: break;
                    default: layer.alert(temp.data); return;
                }
                
                layer.msg("修改成功");
                update_dev(postData);
                cont_device_udpInt(postData.id);
        },
        "html");
}

/*
    函数名称：cont_device_del
    函数功能：删除设备
    传入参数：id 设备ID
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-25
*/
function cont_device_del(id){
    var postData = {};
    postData.display = CMD_DEV.dev.del;
    postData.id = id;

    var _load = layer.load();
    $.post(
            "cgi-bin/dev.cgi",
            JSON.stringify(postData),
            function (data, status) {
                layer.close(_load);
                if (status != "success") { layer.alert("连接服务器失败"); return; }
                var temp = eval('(' + data + ')');
                switch (parseInt(temp.error)) {
                    case 399: location.href = "login.html"; return;
                    case 0: break;
                    default: layer.alert(temp.data); return;
                }

                del_dev(id);
        },
        "html");
}

/*
    函数名称：cont_device_update_point
    函数功能：设置设备的地图坐标点--仅限修改设备信息
    传入参数：x,y
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-25
*/
function cont_device_update_point(x, y){
    /*DOM设置*/
    $("#dct_upddev_x").val(x);
    $("#dct_upddev_y").val(y);
}

/***************************************************************************/

/*
    函数名称：map_upload
    函数功能：上传地图图片
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-25
*/
function map_upload(){
    layer.open({
        type : 2,
        title : "地图图片上传",
        shadeClose : false,
        shade : 0.8,
        area : ['450px', '220px'],
        content : "/map_upload.html"
    });
}

/*
    函数名称：map_pageCont
    函数功能：设备的地图控制
    传入参数：id 设备ID
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-05-25
*/
function map_pageCont(id){
    var dtitle = "";
    var dev_class = null;
    var g_class = null;
    var gg_class = null;

    dev_class = get_dev_classOBJ(id);
    if(dev_class == null){ alert("页面加载有误，请刷新页面"); return; }

    g_class = dev_class.obj_parentGroup;
    gg_class = g_class.obj_parent;

    if(gg_class != null) dtitle += gg_class.name + "/";
    if(g_class != null) dtitle += g_class.name + "/";
    dtitle += dev_class.name;

    layer.open({
        type : 2,
        title : "地图坐标点配置--->" + dtitle,
        shadeClose : true,
        shade : 0.8,
        area : ['800px', '600px'],
        content : "/dev_map.html?id=" + id
    });
}