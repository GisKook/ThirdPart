/*
 * brief: ��װ Oracle ��ѯ��¼
 * function list:
 * 
 * author: a staff of CETCNAV
 * date: 2014��6��3��
 */
#ifndef ORARECORDSET_H_H
#define ORARECORDSET_H_H

typedef struct OCIStmt          OCIStmt;             /* OCI statement handle */
typedef struct OCIError         OCIError;                /* OCI error handle */
class OraDatabase;

typedef enum{
	ORAINT=0,
	ORATEXT,
	ORADOUBLE,
	ORABLOB,
	ORANULL
}OraDatatype;

typedef struct _CNVARIANT{
	OraDatatype eDataType; // �������� 0.int;1.text;2.double;3.blob
	typedef union{
		int iValue;
		double dValue;
		char* pValue; 
		unsigned char* pData; 
	} CNVAR;
	int nCount; // �ַ�����,���ݳ���
	CNVAR value;
#define iValue value.iValue
#define dValue value.dValue
#define pValue value.pValue
}CNVARIANT;

class OraRecordset{
public:
	OraRecordset();
public:
	// brief ������¼��
	// param[in] db���ݿ�
	int Create(OraDatabase* db);

	// brief ���ټ�¼��
	int Destroy();
	
	// brief �õ�ֵ
	// param[out] Ŀ��ֵ
	// param[in] ��
	// param[in] ��
	int GetValue(CNVARIANT &value,int i=0, int j=0);

private:
	OCIStmt *m_pStmt; 

	OraDatabase *m_pDatabase;
};

#endif