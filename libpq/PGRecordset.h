/*
 * brief: ��ѯ���صĽ��
 * function list:
 * 
 * author: a staff of CETCNAV
 * date: 2014��5��7��
 */
#ifndef GKRECORDSET_H_H
#define GKRECORDSET_H_H

#include "PGRecordset.h"

struct pg_result; 
typedef struct pg_result PGresult;

class PGRecordset{
public:
	// brief ����
	void Create(PGresult* pResult);

	// brief ���Recordset
	void Destroy();

	// brief �õ����Ը���
	int GetFieldCount();

	// brief �õ�Ԫ��ĸ���
	int GetTupleCount(); 

	// brief �õ�ָ����ֵ
	char* GetValue(int nTuple, int nField); 
private:
	PGresult* m_pRecordset; 
};

#endif