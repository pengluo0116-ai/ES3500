#include "str_replace.h"
using namespace std;

/*
    函数名称：string_replace
    函数功能：字符串替换函数
    传入参数：
                std::string&s1          源字符串
                const std::string&s2    被替换的字符串
                const std::string&s3    替换成的字符串
    传出数据：替换后的字符串
    编写人员：佚名
    编写时间：无
*/
std::string string_replace(std::string&drc,const std::string&s2,const std::string&s3){
    std::string s1 = drc;
	string::size_type pos=0;
	string::size_type a=s2.size();
	string::size_type b=s3.size();
	while((pos=s1.find(s2,pos))!=string::npos)
	{
		s1.replace(pos,a,s3);
		pos+=b;
	}
    return s1;
}