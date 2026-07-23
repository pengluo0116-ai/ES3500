#include "radio_download.h"
using namespace std;
#define DEBUG 1

pthread_mutex_t radio_download::lock;
std::string radio_download::recvStr_head;
FILE *radio_download::Frecv_BODY;

/*
    函数名称：init
    函数功能：初始化函数
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-20
*/
int radio_download::init(){
    radio_download::recvStr_head = "";
    radio_download::Frecv_BODY = NULL;
    return pthread_mutex_init(&radio_download::lock, NULL);
}

/*
    函数名称：_http_whead_callback
    函数功能：HTTP会话，获取报文头信息
    传入参数：
                void* buffer            服务器返回的数据指针 
                size_t size             服务器返回的数据的大小 = size * nmemb
                size_t nmemb
                void* argv              通过CURLOPT_WRITEDATA设置的指针
    传出数据：>=0x00 数据长度，<0运行有误
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-20
*/
size_t radio_download::_http_whead_callback(void* buffer, size_t size, size_t nmemb, void* argv){
    std::string *str = dynamic_cast<std::string*>((std::string *)argv);
    if( NULL == str || NULL == buffer ){ return -0x01; }

    char *pData = (char *)buffer;
    str->append(pData, size * nmemb);
    return nmemb;
}

/*
    函数名称：_http_wbody_callback
    函数功能：HTTP会话，获取报文体，追加写入文件
    传入参数：
                void* buffer            服务器返回的数据指针 
                size_t size             服务器返回的数据的大小 = size * nmemb
                size_t nmemb
                void* argv              通过CURLOPT_WRITEDATA设置的指针
    传出数据：>=0x00 数据长度，<0运行有误
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-20
*/
size_t radio_download::_http_wbody_callback(void* buffer, size_t size, size_t nmemb, void* argv){
    FILE *fp = (FILE *)argv;
    if(!fp) return -0x01;

    size_t nWrite = fwrite(buffer, size, nmemb, fp);
    return nWrite;
}

/*
    函数名称：_http_send
    函数功能：HTTP会话，下载语音文件
    传入参数：
                const char *_url                文件下载连接地址
                const char *_down_filePath      文件下载路径
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-20
*/
int radio_download::_http_send(const char *_url, const char *_down_filePath){
    /*参数校验*/
    if(!_url || strlen(_url)<=0x00 || !_down_filePath || strlen(_down_filePath)<=0x00) return -0x01;
    
    #if DEBUG
    cout<< "下载文件路径：" << _down_filePath <<endl;
    #endif

    /*创建HTTP会话*/
    CURL *curl = curl_easy_init();
    if(!curl) return -0x02;

    /*设置HTTP会话--URL*/
    curl_easy_setopt(curl, CURLOPT_URL, _url);

    /*设置HTTP会话--返回报文头*/
    radio_download::recvStr_head = "";
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, radio_download::_http_whead_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&radio_download::recvStr_head);

    /*设置HTTP会话--返回报文体*/
    radio_download::Frecv_BODY = fopen(_down_filePath, "wb");
    if(!radio_download::Frecv_BODY){ curl_easy_cleanup(curl); return -0x03; }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, radio_download::_http_wbody_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)radio_download::Frecv_BODY);

    /*设置HTTP会话--设置超时时间*/
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

    /*提交HTTP会话*/
    int res = curl_easy_perform(curl);
    
    /*释放HTTP会话*/
    curl_easy_cleanup(curl);
    fflush(radio_download::Frecv_BODY);
    fclose(radio_download::Frecv_BODY); radio_download::Frecv_BODY = NULL;

    if(res){ 
        #if DEBUG
        cout<< "获取音频文件：错误码[" << res << "]" <<endl;
        #endif

        ostringstream ostr; ostr.str("");
        ostr << "rm -rf " << _down_filePath;
        lsystem(ostr.str().c_str());
        return -0x04; 
    }

    return 0x00;
}

/*
    函数名称：_http_chcek
    函数功能：校验HTTP会话
    传入参数：无
    传出数据：
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-20
*/
int radio_download::_http_chcek(){
    cout<< "返回数据：" <<endl;
    cout<< radio_download::recvStr_head << endl;

    char *point = (char *)0x00;
    point = strstr((char *)radio_download::recvStr_head.c_str(), "HTTP/1.1 200 OK"); if(!point) return -0x01;
    point = strstr((char *)radio_download::recvStr_head.c_str(), "Accept-Ranges: bytes"); if(!point) return -0x02;

    return 0x00;
}

/*
    函数名称：http_cont
    函数功能：HTTP会话，获取百度语音
    传入参数：const char *_lift_name 电梯名称
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2017-04-20
*/
int radio_download::http_cont(const char *_url, const char *_down_filePath){

    pthread_mutex_lock(&radio_download::lock);
    
    /*发送HTTP会话*/
    if(int err = radio_download::_http_send(_url, _down_filePath)) { 
        #if DEBUG
        cout<< "获取语音文件["<< _url <<"]：发送HTTP会话失败,错误码：" << err <<endl;
        #endif

        pthread_mutex_unlock(&radio_download::lock); 
        return 0x01; 
    }

    /*校验HTTP会话*/
    if(int err = radio_download::_http_chcek()){
        #if DEBUG
        cout<< "获取语音文件["<< _url <<"]：校验有误,错误码：" << err <<endl;
        #endif

        pthread_mutex_unlock(&radio_download::lock); 
        return 0x02; 
    }
    
     pthread_mutex_unlock(&radio_download::lock); 
    
    return 0x00;
}