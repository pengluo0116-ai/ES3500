#include "main.h"
using namespace std;

int main(){
    try{
        getPostData();
    }catch(...){ err_msg("访问有误，请稍候再试");}
    
    return 0;
}