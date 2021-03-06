/*
 * brief: 定义一些常用的宏全局函数
 * function list:
 * 
 * author: a staff of CETCNAV
 * date: 2014年8月5日
 */
#ifndef CNDEF_H_H
#define CNDEF_H_H

#define RBDD_API

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#if defined __GNUC__
#define likely(x) __builtin_expect ((x),1)
#define unlikely(x) __builtin_expect((x),0)
#else
#define likely(x)
#define unlikely(x)
#endif

#define min(x,y) ((x>y)?x:y)
static union{
	char c[4];
	unsigned long l;
}endian_test={{'l','?','?','b'}};
#define ENDIANNESS ((char)endian_test.l)
#define ISBIGENDIAN (ENDIANNESS=='b')

#define swab16(x) \
	({\
	 unsigned short _x=(x);\
	 ((unsigned short)(\
		 (((unsigned short)(_x)&(unsigned short)0x00ffU)<<8)|\
		 (((unsigned short)(_x)&(unsigned short)0xff00U)>>8) ));\
	 })

#define swab32(x) \
	({\
	 unsigned int _x=(x);\
	 ((unsigned int)(\
		 (((unsigned int)(_x)&(unsigned int)0x000000ffUL)<<24)|\
		 (((unsigned int)(_x)&(unsigned int)0x0000ff00UL)<<8)|\
		 (((unsigned int)(_x)&(unsigned int)0x00ff0000UL)>>8)|\
		 (((unsigned int)(_x)&(unsigned int)0xff000000UL)>>24)));\
	 })

#endif
