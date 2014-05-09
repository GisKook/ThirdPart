/*
 * brief: libpq.lib的wrapper,数据库级别的操作
 * function list:
 * 
 * author: a staff of CETCNAV
 * date: 2014年5月7日
 */
#ifndef GKDATABASEPG_H_H
#define GKDATABASEPG_H_H
#include "PGRecordset.h"

typedef struct{
	char* pghost;
	char* pgport;
	char* pgoptions;
	char* pgtty;
	char* dbName;
	char* login;
	char* passwd; 
}PGConnInfo;

struct pg_conn;
typedef struct pg_conn PGconn;

class PGDatabase{
public:
	// brief 连接数据库
	// param[in] 连接内容
	int Connect(const PGConnInfo& dbConnInfo); 

	// 断开数据库连接
	int DisConnect(); 

	// brief 执行Select操作 异步操作
	// param[in] sql 要被执行的sql
	bool SendQuery(const char* strQuery);

	// brief 收集由SendQuery发送的命令的结果
	PGRecordset* GetRecordset();

	// brief 开启事务
	void BeginTransaction();

	// brief 结束事务
	void Commit();
private:
	PGconn* m_pConnect;
	
};


#endif