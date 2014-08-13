/*
 * brief: libpq.lib��wrapper,���ݿ⼶��Ĳ���
 * function list:
 * 
 * author: a staff of CETCNAV
 * date: 2014��5��7��
 */
#ifndef GKDATABASEPG_H_H
#define GKDATABASEPG_H_H
#include "PGRecordset.h"

typedef struct{
	char* pghost;
	char* pgport;
	char* dbName;
	char* login;
	char* passwd; 
}PGConnInfo;

struct pg_conn;
typedef struct pg_conn PGconn;

class PGDatabase{
public:
	// brief �������ݿ�
	// param[in] ��������
	int Connect(const PGConnInfo& dbConnInfo); 

	// �Ͽ����ݿ�����
	int DisConnect(); 

	// brief ִ��Select���� �첽����
	// param[in] sql Ҫ��ִ�е�sql
	//bool SendQuery(const char* strQuery);

	// brief �ռ���SendQuery���͵�����Ľ��
	//PGRecordset* GetRecordset();

	// brief ִ��������insert/update����
	bool Exec(const char* strSQL);

	// brief ִ��������select ����
	PGRecordset* Query(const char* strSQL);

	// brief ��������
	bool BeginTransaction();

	// brief ��������
	bool Commit();
private:
	PGconn* m_pConnect;
	
};


#endif