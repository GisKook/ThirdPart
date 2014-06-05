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

#if defined(_WIN32)
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API
#endif // _DEBUG

typedef struct OCIEnv           OCIEnv;            /* OCI environment handle */
typedef struct OCIError         OCIError;                /* OCI error handle */
typedef struct OCISvcCtx        OCISvcCtx;             /* OCI service handle */
typedef struct OCIServer        OCIServer;              /* OCI Server handle */
typedef struct OCISession       OCISession;     /* OCI Authentication handle */
typedef struct OCIStmt          OCIStmt;             /* OCI statement handle */
typedef struct OCITrans         OCITrans;          /* OCI Transaction handle */

#define CN_SUCCESS 0
#define CN_FAIL   1
#define CN_NODATA 2
#define CN_NEEDMORE 3

typedef enum{
	ORANULL = 0,
	ORAINT,
	ORAFLOAT,
	ORATEXT,
}OraDatatype;

class ENGINE_API CNVARIANT{
public:
	CNVARIANT();
	CNVARIANT(OraDatatype datatype){
		eDataType=datatype;
	}
	OraDatatype eDataType; // �������� 0.int;1.text;2.double;3.blob
	typedef union{
		int iValue;
		float fValue;
		char pValue[255]; 
	} CNVAR;
	CNVAR value;
#define iValue value.iValue
#define fValue value.fValue
#define pValue value.pValue
};

typedef struct{
	char* host;
	char* port;
	char* dbName;
	char* login;
	char* passwd; 
}OraConnInfo;

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
	int Query(const char* strSQL, std::vector<CNVARIANT>& vVal);

	// brief �õ���һ������
	int Fetch();

	// brief �õ�select�б��е��и���
	int GetColCount();
	//int Query(const char* strSQL, std::vector<CNVARIANT>& vVal, int nRow);
private:
	OCIEnv* m_pEnv; 
	OCIError* m_pErr;
	OCISvcCtx* m_pSvcCtx;
	OCIServer* m_pServer;
	OCISession* m_pSession;
	OCITrans* m_pTrans;
	OCIStmt* m_pStmt;
	
};
#endif