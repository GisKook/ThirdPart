/*
 * brief: 放置类型，基础函数
 * function list:
 * 
 * author: zhangkai
 * date: 2014年6月10日
 */
#ifndef DATABASEDEF_H_H
#define DATABASEDEF_H_H

#if defined(_WIN32)
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API
#endif 

typedef struct{
	char* host;
	int port;
	char* dbName;
	char* login;
	char* passwd; 
}OraConnInfo;

typedef struct OCIEnv           OCIEnv;            /* OCI environment handle */
typedef struct OCIError         OCIError;                /* OCI error handle */
typedef struct OCISvcCtx        OCISvcCtx;             /* OCI service handle */
typedef struct OCIServer        OCIServer;              /* OCI Server handle */
typedef struct OCISession       OCISession;     /* OCI Authentication handle */
typedef struct OCIStmt          OCIStmt;             /* OCI statement handle */
typedef struct OCITrans         OCITrans;          /* OCI Transaction handle */
typedef struct OCISubscription  OCISubscription;      /* subscription handle */

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
	CNVARIANT(){};
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

static union{
	char c[4];
	unsigned long l;
}endian_test={{'l','?','?','b'}};
#define ENDIANNESS ((char)endian_test.l)
#define ISBIGENDIAN (ENDIANNESS=='b')

#if defined(_WIN32)
#define swap16(x) ((*(((unsigned short*)(x))) >> 8) | (*(x) << 8))
#elif defined(__linux__)
#define swap16(x) \
	({\
	unsigned short _x=(x);\
	((unsigned short)(\
	(((unsigned short)(_x)&(unsigned short)0x00ffU)<<8)|\
	(((unsigned short)(_x)&(unsigned short)0xff00U)>>8) ));\
})
#endif


//extern char g_fmt[128];
#endif
