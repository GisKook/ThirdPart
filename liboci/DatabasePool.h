/*
 * brief: 封装一个数据库连接池，使用hash表，链表用来处理冲突
 * function list:
 * 
 * author: zhangkai
 * date: 2014年6月10日
 */
#ifndef DATABASEPOOL_H_H
#define DATABASEPOOL_H_H
#include "list.h"
#include "DatabaseDef.h"
#include "OraDatabase.h"

typedef struct {
	OraDatabase *db;
	list_head *list;
	bool isUsed;
	int nThreadID;
}dbConn;

class DatabasePool{
public:
	DatabasePool();

	// brief 创建
	// param[in] 数据库连接
	// param[in] 池子的大小
	void Create(OraConnInfo& connInfo, int size);

	// brief 销毁
	void Destroy();
	
	// brief 得到数据库连接
	// param[in] 线程id号
	OraDatabase* GetConnection(int nThreadID);

	// brief 将数据库连接放回池子
	void PutbackConnection(int nThreadID);

	// brief 删除
	void DeleteConnection(int nThreadID);

	// brief 得到连接信息
	OraConnInfo* GetConnInfo();

private:
	// brief 得到hash值
	// param[in] 线程id
	int GetHashCode(int nThreadID){ return nThreadID % m_nPoolsize;};

private:
	int m_nPoolsize;
	OraConnInfo m_ConnInfo; 

	list_head* m_pHashTable;
};
#endif