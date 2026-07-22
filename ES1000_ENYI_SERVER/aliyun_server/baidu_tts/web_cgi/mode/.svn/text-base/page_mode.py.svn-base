#coding:utf-8

class page_mode:
    upload_tts_recv = '''
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <title>语音解析</title>
    <link href="/css/tts_recv.css" rel="stylesheet" type="text/css" />
    <script src="/js/jquery-1.8.3.min.js" type="text/javascript"></script>
    <script src="/js/layer/layer.js" type="text/javascript"></script>
    <script src="/js/layer/extend/layer.ext.js" type="text/javascript"></script>
    <!--[if lt IE 9]>
    <script src="/js/json/json2.js" type="text/javascript"></script>
    <![endif]-->
    <script src="/js/JS.js" type="text/javascript"></script>
</head>
<body>
    <form enctype="multipart/form-data" method="POST" action="/cgi-bin/upload">
    <div class="head_by">
        <!--# include file="/topnav/topnav.html" --> 
    </div>
    <div class="body_by">
        <div class="zc_title">在线获取语音包</div>
        <div class="zc_by">
            <div class="zc_lift" id="d_show">
                <table class="zc_tb" style="margin-bottom:0px; background-color: #B8B9B9;" border="0" cellpadding="0" cellspacing="0">
                    <tr>
                        <td class="zc_td1">语音内容</td>
                        <td class="zc_td2">操作结果</td>
                    </tr>
                </table>
                <table class="zc_tb" id="tts_list" style="margin-bottom:30px; border-top-width: 0px;" border="0" cellpadding="0" cellspacing="0">
                </table>
            </div>
            <div class="zc_right" id="d_res">
            </div>
        </div>
    </div>
    </form>
</body>
</html>

<script type="text/javascript">

recvJson = $RECVSTR$;

/*显示语音列表*/
function ttsList_show(){
    $("#tts_list").empty();
    for(var i=0x00; i<recvJson["data"].length; i++){
        var tmp = "";
        tmp += '<tr>';
        tmp += '<td class="zc_td1">'+ recvJson["data"][i]["name"] +'</td>';
        tmp += recvJson["data"][i]["error"] ? '<td class="zc_td2" style="color:red;">'+ recvJson["data"][i]["log"]  +'</td>' : '<td class="zc_td2">'+ recvJson["data"][i]["log"]  +'</td>';
        tmp += '</tr>';

        $("#tts_list").append(tmp);
    }
}

/*创建成功--right*/
function creatOk(){
    var tts_num = recvJson["data"].length;
    var tts_cok = 0;
    for(var i=0x00; i<recvJson["data"].length; i++){
        if(recvJson["data"][i]["error"] == 0) tts_cok++;
    }
    var tts_cerr = tts_num - tts_cok;

    var tmp = "";
    tmp += '<div class="zc_right_d"><a class="d_button" href="'+ recvJson["download"] +'">点击下载语音包</a></div>';
    tmp += '<div class="zc_right_log">语音文件：'+ tts_num +'(创建成功:'+ tts_cok +' 创建失败:'+ tts_cerr +')</div>';
    tmp += '<div class="zc_right_log" style="margin-top:30px;"><a href="/index.html"><<返回首页</a></div>';

    $("#d_res").empty();
    $("#d_res").html(tmp);
}

/*创建失败*/
function creatErr(){
    $("#d_show").empty();
    var tmp = "";
    tmp += '<div class="zc_left_err"><img src="/image/crt_err.png" /></div>';
    $("#d_show").html(tmp);

    $("#d_res").empty();
    tmp = "";
    tmp += '<div class="zc_right_err">创建失败：'+ recvJson["data"] +'</div>';
    tmp += '<div class="zc_right_d"><a class="idx_button" href="/index.html">返回首页</a></div>';
    $("#d_res").html(tmp);
}

function recv_contrl(){
    if(recvJson["error"] != 0x00){
        creatErr();
    }

    else{
        creatOk();
        ttsList_show();
    }
}

$(document).ready(function(){
    recv_contrl();
});
</script>

    '''
