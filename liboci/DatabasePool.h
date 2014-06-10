/*
 * brief: ��װһ�����ݿ����ӳأ�ʹ��hash���������������ͻ
 * function list:
 * 
 * author: zhangkai
 * date: 2014��6��10��
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

	// brief ����
	// param[in] ���ݿ�����
	// param[in] ���ӵĴ�С
	void Create(OraConnInfo& connInfo, int size);

	// brief ����
	void Destroy();
	
	// brief �õ����ݿ�����
	// param[in] �߳�id��
	OraDatabase* GetConnection(int nThreadID);

	// brief �����ݿ����ӷŻس���
	void PutbackConnection(int nThreadID);

	// brief ɾ��
	void DeleteConnection(int nThreadID);

	// brief �õ�������Ϣ
	OraConnInfo* GetConnInfo();

private:
	// brief �õ�hashֵ
	// param[in] �߳�id
	int GetHashCode(int nThreadID){ return nThreadID % m_nPoolsize;};

private:
	int m_nPoolsize;
	OraConnInfo m_ConnInfo; 

	list_head* m_pHashTable;
};
#endif