/*
 * brief: 封装oracle oci的c接口
 * function list:
 * 
 * author: a staff of CETCNAV
 * date: 2014年5月26日
 */
#ifndef ORADATABASE_H_H
#define ORADATABASE_H_H
#include <vector>

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

class CNVARIANT{
public:
	CNVARIANT();
	CNVARIANT(OraDatatype datatype){
		eDataType=datatype;
	}
	OraDatatype eDataType; // 数据类型 0.int;1.text;2.double;3.blob
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

class OraDatabase{
public:
	OraDatabase();
public:
	// brief 初始化环境
	int Init();

	// brief 释放 对应Init
	int Destroy();

	// brief 连接数据库
	// param[in] 连接内容
	int Connect(const OraConnInfo& dbConnInfo); 

	// 断开数据库连接
	int DisConnect(); 

	// brief 执行sql语句
	bool ExecSQL(const char* strSql);

	// brief 得到select 语句的结果
	// param[in] 要执行的sql语句
	// remark 只返回一个元组
	int Query(const char* strSQL, CNVARIANT& val);

	int Query(const char* strSQL, std::vector<CNVARIANT>& vVal);
	int Query(const char* strSQL, std::vector<CNVARIANT>& vVal, int nRow);

	// brief 执行Select操作 异步操作
	// param[in] sql 要被执行的sql
	bool SendQuery(const char* strQuery);

	// brief 收集由SendQuery发送的命令的结果
	void* GetRecordset();

	// brief 开启事务
	void BeginTransaction();

	// brief 结束事务
	void Commit();
public:
	OCIEnv* GetEnv(){return m_pEnv;};
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