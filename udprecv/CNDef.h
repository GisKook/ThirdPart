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


#endif
