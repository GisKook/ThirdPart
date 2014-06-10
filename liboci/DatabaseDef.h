/*
 * brief: 放置类型，基础函数
 * function list:
 * 
 * author: zhangkai
 * date: 2014年6月10日
 */
#ifndef DATABASEDEF_H_H
#define DATABASEDEF_H_H

#if defined(_WIN32)
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API
#endif // _DEBUG

typedef struct{
	char* host;
	int port;
	char* dbName;
	char* login;
	char* passwd; 
}OraConnInfo;

#endif
