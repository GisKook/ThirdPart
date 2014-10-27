/*
 * brief: ��װoracle oci��c�ӿ�
 * function list:
 * 
 * author: a staff of CETCNAV
 * date: 2014��5��26��
 */

#ifndef ORADATABASE_H_H
#define ORADATABASE_H_H
#include <vector> 
#include "DatabaseDef.h"


class ENGINE_API OraDatabase{
public:
	OraDatabase();
public:
	// brief ��ʼ������
	int Init();

	// brief �ͷ� ��ӦInit
	int Destroy();

	// brief �������ݿ�
	// param[in] ��������
	int Connect(const OraConnInfo& dbConnInfo); 

	// �Ͽ����ݿ�����
	int DisConnect(); 

	// brief ִ��sql���
	bool ExecSQL(const char* strSql);

	// brief �õ�select ���Ľ��
	// param[in] Ҫִ�е�sql���
	// remark ֻ����һ��Ԫ��
	int Query(const char* strSQL, CNVARIANT& val);

	// brief �õ���select ���Ľ��
	// param[in] sql���
	// param[in/out] �󶨵����� ����ָ���������ͣ��õ���ֵ
	// param[in] �еĸ���
	int Query(const char* strSQL, CNVARIANT* p, int nColCount);

	// brief �õ���һ������
	// param[in] ����FetchǰҪ��Query
	int Fetch();

	// brief �õ�select�б��е��и���
	int GetColCount(); 

	// brief �ж������Ƿ����
	// remark �ú������Բ������Ӳ����õ�����������жϣ�
	// this attribute is sensitive to high availability evnets on the database
	// that result in dead connection from the client(e.g. a database instance 
	// going down) and to some of specific errors(such as ORA-3113) that are 
	// known to drop connections
	// ����dbaɱ��������̻�����������ʧ����û�з��������жϵģ�(�����֪���Ļ�
	// ��ֻ�ܵ�ִ����һ��oci����ʱ���صĴ���֪����:( )
	bool IsConnectionValid();

	// brief �õ������ַ���
	OraConnInfo* GetConnInfo(){return &m_ConnInfo;};

	//int Query(const char* strSQL, std::vector<CNVARIANT>& vVal, int nRow);

private:
	OCIEnv* m_pEnv; 
	OCIError* m_pErr;
	OCISvcCtx* m_pSvcCtx;
	OCIServer* m_pServer;
	OCISession* m_pSession;
	OCITrans* m_pTrans;
	OCIStmt* m_pStmt;
	OCISubscription* m_pSubscription;
	
private:
	OraConnInfo m_ConnInfo;
};

#endif
