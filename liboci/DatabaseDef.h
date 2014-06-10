/*
 * brief: �������ͣ���������
 * function list:
 * 
 * author: zhangkai
 * date: 2014��6��10��
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
