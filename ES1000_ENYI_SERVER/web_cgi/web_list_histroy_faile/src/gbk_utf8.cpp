#include "gbk_utf8.h"

/*
    函数名称：code_convert
    函数功能：utf8与gbk转换
	传入参数：
				const char *from_charset	字符原始编码
				const char *to_charset		字符生成编码
				char *inbuf					源码字符内容
				size_t inlen				源码字符内容长度
				char *outbuf				生成的目标编码内容存储的空间
				size_t outlen				生成的目标编码内容存储的空间
	传出数据：0 运行正确 非0 运行失败
	注意事项：无
	编写人员：
	整理人员：王凤龙
	整理时间：2016-02-01
*/
static int code_convert(const char *from_charset, const char *to_charset, char *inbuf, size_t inlen,
		char *outbuf, size_t outlen) {
	iconv_t cd;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset, from_charset);
	if (cd == 0)
		return -1;
	memset(outbuf, 0x00, outlen);
	if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
		return -1;
	iconv_close(cd);
	*pout = '\0';

	return 0;
}

/*
	函数名称：u2g
	函数功能：UTF8转换成GBK
	传入参数：
				char *inbuf					源码字符内容
				size_t inlen				源码字符内容长度
				char *outbuf				生成的目标编码内容存储的空间
				size_t outlen				生成的目标编码内容存储的空间
	传出数据：0 运行正确 非0 运行失败
	注意事项：无
	编写人员：
	
	整理人员：王凤龙
	整理时间：2016-02-01
*/
int u2g(char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
	return code_convert("utf-8", "gb2312", inbuf, inlen, outbuf, outlen);
}

/*
	函数名称：g2u
	函数功能：GBK转换成UTF8
	传入参数：
				char *inbuf					源码字符内容
				size_t inlen				源码字符内容长度
				char *outbuf				生成的目标编码内容存储的空间
				size_t outlen				生成的目标编码内容存储的空间
	传出数据：0 运行正确 非0 运行失败
	注意事项：无
	编写人员：
	
	整理人员：王凤龙
	整理时间：2016-02-01
*/
int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
	return code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);
}

#if 0
int main(void) {
	char *s = "中国";
	int fd = open("test.txt", O_RDWR|O_CREAT, S_IRUSR | S_IWUSR);
	char buf[10];
	u2g(s, strlen(s), buf, sizeof(buf));
	write(fd, buf, strlen(buf));
	close(fd);

	fd = open("test.txt2", O_RDWR|O_CREAT, S_IRUSR | S_IWUSR);
	char buf2[10];
	g2u(buf, strlen(buf), buf2, sizeof(buf2));
	write(fd, buf2, strlen(buf2));
	close(fd);
	return 1;
}
#endif