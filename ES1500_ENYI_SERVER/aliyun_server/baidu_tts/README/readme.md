<font face="微软雅黑">
#项目简介  
> 在云端部署一个在线语音生成系统，提供相应接口供客户端使用，用于文字生成语音文件；系统调用百度语音接口（免费）。
> 客户端通过restful协议，可以生成语音包并进行下载。  
> 客户端同时具有批量生成语音包功能。

#功能实现
* 百度语音账号的token实时更新
* 文字信息生成语音文件（RESTFUL协议）；
* 文字信息批量生成语音文件（RESTFUL协议）；
* 临时语音文件缓存资源回收。

#模块介绍
<table border="0" cellpadding="0" cellspacing="0">
	<tr>
		<td>文件夹</td>
		<td>简单介绍</td>
	</tr>
	<tr>
		<td>conf</td>
		<td>系统的配置信息</td>
	</tr>
	<tr>
		<td>baidu_tts_token</td>
		<td>百度语音账号token实时更新模块</td>
	</tr>
	<tr>
		<td>baidu_tts_clear</td>
		<td>语音文件临时缓存垃圾回收模块</td>
	</tr>
	<tr>
		<td>web</td>
		<td>WEB页面</td>
	</tr>
	<tr>
		<td>web_cgi</td>
		<td>语音生成接口</td>
	</tr>
</table>

#系统部署
###配置前准备
需要安装的服务器：REDIS，NGINX，UWSGI  
新建目录（例如/root/work/目录下，创建文件夹 baidu\_tts），分别将目录conf，baidu\_tts\_token，baidu\_tts\_clear，web\_cgi复制到新建的目录下（baidu\_tts）;  
<font color="red">代码存储目录路径：/root/work/baidu\_tts，以下部署依据此目录进行。</font>  

将web复制到/var/目录下，web目录包含download目录。

###配置文件  
配置文件：tts.conf  
该配置文件用于配置从百度语音接口申请的账号信息，账号信息可以配置多个，配置信息为以下格式：  

	[
		{
			"name":"tts_0", 									
			"api_key":"lXvdTGD9t8ZRAtYr2ydLhBrz", 				
			"secret_key":"2fbafd11dbccb5c43ada49fdc305d501",	
			"update_time":"01",
			"note":"项目"
		}
	]

键值介绍：  
name:   
接口名称（用户自定义，添加多个百度接口，不要重复），创建好的token存储到缓存中，缓存键值：baidu\_tts\_token\_<font color="red">name</font>    
例：baidu\_tts\_token\_tts\_0  

api_key:  
百度接口api\_key，申请百度应用后，从百度平台获取。  

secret_key:  
百度接口secret\_key，申请百度应用后，从百度平台获取。  

update\_time:  
token更新操作时间，取值范围：01-23；（例：01 --> 凌晨1点更新）;

note:  
配置信息，用户自由填写。  

###百度语音账号token实时更新模块
目录：baid\_tts\_token  
修改baidu\_tts\_token/conf/目录下的config.py文件，CONF\_PATH指向配置文件路径（/root/work/baidu\_tts/conf）  
<font color="red">运行：python /root/work/baidu\_tts/baidu\_tts\_token/main.py</font>  

备注信息：该模块可以直接在后台运行，或者开机运行后执行一次；  

###语音文件临时缓存垃圾回收模块
目录：baidu\_tts\_clear  
修改文件baidu\_tts\_clear/main.py  
cont\_dir指向音频文件缓存区路径（/var/web/download）  
<font color="red">运行：python /root/work/baidu\_tts/baidu\_tts\_clear/main.py</font>  

备注信息：该模块执行后，清空完垃圾缓存(音频缓存存储有效期为3天)将自动退出；建议做成定时任务，例如每天早上5点执行一次。  
定时任务（centos）：

	执行指令：crontab -e
	编写配置文件
	# Example of job definition:
	# .---------------- minute (0 - 59)
	# |  .------------- hour (0 - 23)
	# |  |  .---------- day of month (1 - 31)
	# |  |  |  .------- month (1 - 12) OR jan,feb,mar,apr ...
	# |  |  |  |  .---- day of week (0 - 6) (Sunday=0 or 7) OR sun,mon,tue,wed,thu,fri,sat
	# |  |  |  |  |
	# *  *  *  *  * user-name command to be executed
	  0  5  *  *  * /root/work/baidu_tts/baidu_tts_clear/main.py
	
	执行指令：
	/sbin/service crond restart
	

###语音生成接口
目录：web\_cgi  
修改文件web\_cgi/conf/config.py，主要修改地方：  

	CONF_PATH 			"/root/work/baidu_tts/conf/tts.conf"	#配置文件路径  
	MP3_DOWNLOAD_PAHT	"/var/web/download"						#音频文件缓存路径
	WEB_SERVER_NAME		"http://www.emblink.com:9091"			#服务器网址，用于下载使用
	WEB_DOWNLOAD_PATH	"download"								#下载文件夹

uwsgi配置：

	[uwsgi]
	socket = 127.0.0.1:8000
	chdir = /root/work/baidu_tts/web_cgi/
	wsgi-file = main.py
	processes = 4
	threads = 2

nginx配置：  

	#user  nobody;
	worker_processes  1;
	
	#error_log  logs/error.log;
	#error_log  logs/error.log  notice;
	#error_log  logs/error.log  info;
	
	#pid        logs/nginx.pid;
	daemon off;
	master_process off;
	events {
	    worker_connections  1024;
	}
	
	
	http {
	    include       mime.types;
	    default_type  application/octet-stream;
	
	    #log_format  main  '$remote_addr - $remote_user [$time_local] "$request" '
	    #                  '$status $body_bytes_sent "$http_referer" '
	    #                  '"$http_user_agent" "$http_x_forwarded_for"';
	
	    #access_log  logs/access.log  main;
	
	    sendfile        on;
	    #tcp_nopush     on;
	
	    #keepalive_timeout  0;
	    keepalive_timeout  150;
	
	    #gzip  on;
	
	    server {
	     listen       9091;
	     server_name  127.0.0.1;
	     	ssi on;
	     	ssi_silent_errors on;
		root /var/web;
		 
		 location /cgi-bin/ {
				include uwsgi_params;
				uwsgi_pass 127.0.0.1:8000;
				uwsgi_param UWSGI_CHDIR /root/work/baidu_tts/web_cgi;
				uwsgi_connect_timeout 70;
				uwsgi_read_timeout 180;
				uwsgi_param UWSGI_PYHOME $document_root; 
				uwsgi_param UWSGI_CHDIR  $document_root;
	         }
	     error_page   500 502 503 504  /50x.html;
	     location = /50x.html {
	         root   html;
	     }
	  }
	}

#接口调用测试
音频生成：

	客户端发送：	
	curl -d "{\"name\":\"张三\"}" www.emblink.com:9091/cgi-bin/radio
	
	服务端返回：
	{
		"download": "http://www.emblink.com:9091/download/11e08864-7b3f-11e7-8821-00163e322f29_20170807/ced07fb42b05a2ed9efa330250e2bb9175f962ce.mp3", 
		"name": "ced07fb42b05a2ed9efa330250e2bb9175f962ce.mp3", 
		"error": 0
	}

	download的键值为音频下载路径，进行下载即可

音频批量生成：

	新建文件（编码格式：utf8）,按照以下格式填写
	[
		{"name": "中文1"},
		{"name": "中文2"},
		{"name": "中文3"}
	]

	浏览器输入网址（http://www.emblink.com:9091），并打开，上传刚才编写的文件，稍后会生成压缩包，点击“下载按钮”
	
	解压文件后，会发现生成的音频文件。
	
</font>



