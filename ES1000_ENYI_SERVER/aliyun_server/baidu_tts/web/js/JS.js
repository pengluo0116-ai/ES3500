/// <reference path="jquery-1.8.3.min.js" />
/*去除首尾空格*/
function trimStr(str){return str.replace(/(^\s*)|(\s*$)/g,"");}

/*isNaN() 判断字符串是否为数字，如果是返回false 不是 返回true*/

/*
函数功能：Table标签隔行换色，鼠标放在行上时换色
传入参数：table_id    -> 要进行操作的table标签的ID号
          even_color  -> 奇数行的颜色
          odd_color   -> 偶数行的颜色
          mouse_color -> 鼠标滑过时的颜色
传出数据：无
注意事项：使用该函数时，需要在调用该函数的页面引入jquery脚本
          Table_LineColor("show_cmp", "#fff", "#c5dee0", "#f8cd90");
*/
function Table_LineColor(table_id, even_color, odd_color, mouse_color){ 

    $("#"+ table_id +" tr:even").css({ "background": even_color }); //奇数行
    $("#"+ table_id +" tr:odd").css({ "background": odd_color });  //偶数行

    /*保存奇数偶数颜色*/
    $("#"+ table_id +" tr:even").attr("bg", even_color);
    $("#"+ table_id +" tr:odd").attr("bg",odd_color);

    $("#"+ table_id +" tr").mouseover(function(){$(this).css("background", mouse_color)}); //鼠标滑上

    $("#"+ table_id +" tr").mouseout(function(){var bgc = $(this).attr("bg"); $(this).css("background", bgc);}); //鼠标滑出
        
}


function getQueryString(name) {
        var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)", "i");
        var r = window.location.search.substr(1).match(reg);
        if (r != null) return unescape(r[2]); return null;
    }

/*英文，数字，下划线，以字母开头*/
function checkUser(str){
    var re = /^[a-zA-z]\w{3,15}$/;
    if(re.test(str)){ return true;}
    else{ return false;}          
}

/*ip校验*/
function isIP(ip){
      
    var reSpaceCheck = /^(\d+)\.(\d+)\.(\d+)\.(\d+)$/;
    if (reSpaceCheck.test(ip)) {
        ip.match(reSpaceCheck);
        if (
            RegExp.$1 <= 255 && RegExp.$1 >= 0
            && RegExp.$2 <= 255 && RegExp.$2 >= 0
            && RegExp.$3 <= 255 && RegExp.$3 >= 0
            && RegExp.$4 <= 255 && RegExp.$4 >= 0
        ) return true; else return false;
    }
    else return false; 
}  
    
    
/*ajax提交处理复制*/
/*
    需要引入jquery.js 和 layer.js 
*/
//var postjson = {};
// //在此写入你要提交的数据       
// $.post(
//    "partnerdis.ashx",
//    encodeURI(JSON.stringify(postjson)),
//    function(data,status){
//        if(status != "success") {layer.alert("连接服务器失败"); return;}
//        var temp = eval('('+ data +')')
//        if( parseInt(temp.error) != 0){layer.alert(temp.data); return;}
//        //在此写入你要处理的操作
// });