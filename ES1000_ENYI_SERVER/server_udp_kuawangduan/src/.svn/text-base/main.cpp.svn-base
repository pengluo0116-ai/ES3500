#include "main.h"
using namespace std;

int main(){
    
    /*配置udp Server*/
    udp_sock_server hostServer(FAMILY_SERVER_PORT);
    if(hostServer.create_server()) return 0x01;
    
    /*开始工作*/
    try{ SERVER_RUN(&hostServer); }catch(...){}
    
    /*结束工作*/
    hostServer.close_server();
    
    return 0x00;
}