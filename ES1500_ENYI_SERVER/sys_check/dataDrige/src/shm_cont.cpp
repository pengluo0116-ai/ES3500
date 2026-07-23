#include "shm_cont.h"
using namespace std;

#define DEBUG 0
/*
    函数名称：shm_cont
    函数功能：构造函数
    传入参数：
                const char *_name       共享变量名称
                unsigned int _shm_len   共享变量尺寸
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-06-24
*/
shm_cont::shm_cont(const char *_name, unsigned int _shm_len):
fd(0x00), name(std::string(_name)), data((unsigned char *)0x00), shm_len(_shm_len){}

/*
    函数名称：~shm_cont
    函数功能：析构函数
    传入参数：
                const char *_name       共享变量名称
                unsigned int _shm_len   共享变量尺寸
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-06-24
*/
shm_cont::~shm_cont(){ this->shm_close(); }

/*
    函数名称：shm_create
    函数功能：创建共享变量
    传入参数：无
    传出数据：0运行成功 非0运行失败
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-06-24
*/
int shm_cont::shm_create(){
    /*校验参数*/
    if(
        this->name.size() <= 0x00 ||
        this->shm_len <= 0x00
    ){
        #if DEBUG
        cout<<"开启共享变量参数错误"<<endl;
        #endif
        return -1;
    }

    /*打开或创建共享内存*/
    this->fd = shm_open(
        this->name.c_str(),             //共享变量名称
        O_RDWR | O_CREAT | O_TRUNC,     //可读写 可创建 如果已经创建，设置大小为0
        S_IRUSR | S_IWUSR               //整个系统的进程，对该共享内存拥有的权限
    );
    if(this->fd <= 0x00){
        #if DEBUG
        cout<<"创建共享内存["<< this->name.c_str() <<"]失败!"<<endl;
        #endif
        
        perror("open shm err");
        this->fd = 0x00;
        return 1;
    }

    /*设置恭喜内存尺寸*/
    if( ftruncate(this->fd, this->shm_len) ){
        #if DEBUG
        cout<<"设置共性内存尺寸失败"<<endl;
        #endif

        this->shm_close();
        return 2;
    }

    /*将共享内存映射到本进程*/
    this->data = (unsigned char *)mmap(
        NULL,
        this->shm_len,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        this->fd,
        0x00
    );
    if(this->data == MAP_FAILED){
        #if DEBUG
        cout<<"将共享内存映射到本进程失败"<<endl;
        #endif

        this->shm_close();
        return 3;
    }

    return 0x00;
}

/*
    函数名称：shm_close
    函数功能：删除共享变量
    传入参数：无
    传出数据：无
    注意事项：无
    编写人员：王凤龙
    编写时间：2016-06-24
*/
void shm_cont::shm_close(){
    /*取消共享内存映射*/
    if(this->data != (unsigned char *)0x00) munmap(this->data, (off_t)this->shm_len);
    this->data = (unsigned char *)0x00;
    this->fd = 0x00;

    /*删除共享内存*/
    shm_unlink(this->name.c_str());     

    #if DEBUG
    cout<<"共享内存删除"<<endl;
    #endif
}

/*
    函数名称：writeData
    函数功能：向共享变量写入数据
    传入参数：
                unsigned char *_data    要写入的数据
                unsigned int _data_len  长度
    传出数据：
                -1  校验有误
                 0  运行正确
                 1  写入失败 
    注意事项：写入数据前，会将数据区清空
    编写人员：王凤龙
    编写时间：2016-06-24
*/
int shm_cont::writeData(const unsigned char *_data, unsigned int _data_len){
    /*写入前的校验*/
    if(
        this->data == (unsigned char *)0x00 ||
        _data == (unsigned char *)0x00  ||
        _data_len > this->shm_len
    ){
        #if DEBUG
        cout<<"向共享变量写入数据失败[校验违法]"<<endl;
        #endif
        return -1;
    }

    /*写入数据*/
    try{
        memset((void *)this->data, 0x00, this->shm_len);
        memcpy((void *)this->data, (void *)_data, _data_len);
    }catch(...){
        #if DEBUG
        cout<<"向共享变量写入数据失败"<<endl;
        #endif

        return 1;
    }

    return 0x00;
}

/*
    函数名称：writeData
    函数功能：从共享变量读取数据
    传入参数：
                unsigned char *_data_buff   读取到的数据写入到该指针指向的缓冲区
                unsigned int _data_len      要读取的数据量
    传出数据：
                -1  校验有误
                 0  运行正确
                 1  读取失败 
    注意事项：数据的数据量必须<=缓冲区大小
    编写人员：王凤龙
    编写时间：2016-06-24
*/
int shm_cont::readData(unsigned char *_data_buff, unsigned int _read_len){
    /*读取前的校验*/
    if(
        this->data == (unsigned char *)0x00 ||
        _read_len > this->shm_len
    ){
        #if DEBUG
        cout<<"读取共享变量失败[校验有误]"<<endl;
        #endif
        
        return -1;
    }

    /*读取数据*/
    try{
        memset((void *)_data_buff, 0x00, _read_len);
        memcpy((void *)_data_buff, (void *)this->data, _read_len);
    }catch(...){
        #if DEBUG
        cout<<"读取共享变量失败"<<endl;
        #endif

        return 1;
    }

    return 0x00;
}

/*
    函数名称：writeStr
    函数功能：向缓冲区写入字符串
    传入参数：const char *_dataStr 要写入的字符串的指针
    传出数据：
                 -1 校验有误
                  0 运行成功
                  1 写入错误
    注意事项：
                写入的字符串必须<=共享变量缓冲区尺寸
                写入数据时，会先将共享变量缓冲区清空
    编写人员：王凤龙
    编写时间：2016-06-24
*/
int shm_cont::writeStr(const char *_dataStr){
    /*写入前的校验*/
    if(
        this->data == (unsigned char *)0x00 ||
        _dataStr == (char *)0x00 ||
        strlen(_dataStr) > this->shm_len
    ){
        #if DEBUG
        cout<<"向共享变量写入数据校验有误"<<endl;
        #endif
        return -1;
    }

    /*写入数据*/
    try{
        memset((void *)this->data, 0x00, this->shm_len);
        memcpy((void *)this->data, (void *)_dataStr, strlen(_dataStr));
    }catch(...){
        #if DEBUG
        cout<<"向共享变量写入字符串失败"<<endl;
        #endif
        
        return 1;
    }

    return 0x00;
}

/*
    函数名称：readStr
    函数功能：从共享内存中读取字符串
    传入参数：std::string &_data 读取到的字符串写入到该变量当中
    传出数据：
               -1   校验有误 
                0   运行成功
                1   读取失败
*/
int shm_cont::readStr(std::string &_data){
    /*读取前的校验*/
    if(this->data == (unsigned char *)0x00){
        #if DEBUG
        cout<<"从共享变量读取字符串校验有误"<<endl;
        #endif
        return -1;
    }
    
    /*读取字符串*/
    try{
        _data = string((const char *)this->data);
    }catch(...){
        #if DEBUG
        cout<<"从共享变量读取字符串失败"<<endl;
        #endif
        return 1;
    }

    return 0x00;
}