/*
 * brief: 封装 Oracle 查询记录
 * function list:
 * 
 * author: a staff of CETCNAV
 * date: 2014年6月3日
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
	OraDatatype eDataType; // 数据类型 0.int;1.text;2.double;3.blob
	typedef union{
		int iValue;
		double dValue;
		char* pValue; 
		unsigned char* pData; 
	} CNVAR;
	int nCount; // 字符长度,内容长度
	CNVAR value;
#define iValue value.iValue
#define dValue value.dValue
#define pValue value.pValue
}CNVARIANT;

class OraRecordset{
public:
	OraRecordset();
public:
	// brief 创建记录集
	// param[in] db数据库
	int Create(OraDatabase* db);

	// brief 销毁记录集
	int Destroy();
	
	// brief 得到值
	// param[out] 目标值
	// param[in] 行
	// param[in] 列
	int GetValue(CNVARIANT &value,int i=0, int j=0);

private:
	OCIStmt *m_pStmt; 

	OraDatabase *m_pDatabase;
};

#endif