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
	int port;
	char* dbName;
	char* login;
	char* passwd; 
}OraConnInfo;

class ENGINE_API OraDatabase{
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

	// brief 得到的select 语句的结果
	// param[in] sql语句
	// param[in/out] 绑定的数据 传入指定数据类型，得到的值
	int Query(const char* strSQL, std::vector<CNVARIANT>& vVal);

	// brief 得到下一条数据
	// param[in] 调用Fetch前要先Query
	int Fetch();

	// brief 得到select列表中的列个数
	int GetColCount(); 

	// brief 判断连接是否可用
	// remark 该函数仅对部分连接不可用的情况进行了判断：
	// this attribute is sensitive to high availability evnets on the database
	// that result in dead connection from the client(e.g. a database instance 
	// going down) and to some of specific errors(such as ORA-3113) that are 
	// known to drop connections
	// 对于dba杀掉服务进程或者网络连接失败是没有方法进行判断的，(如果想知道的话
	// 就只能等执行下一个oci函数时返回的错误知道了:( )
	bool IsConnectionValid();

	// brief 得到链接字符串
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
	
private:
	OraConnInfo m_ConnInfo;
};
#endif
