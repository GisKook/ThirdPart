//#include <afx.h>
#include "oci.h" // 头文件位置有关系
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "OraDatabase.h"
#include "stdio.h"
using namespace std;

//#define CNPRINTF TRACE0 
#define CNPRINTF printf

#define bit(a,b) ((a)&(b))

OCIError* g_pErr;
OCIEnv* g_pEnv;
int checkerr(OCIError *errhp,sword status,const char* strSQL = "" )
{
	text errbuf[512]={0};
	sb4 errcode = 0;

	switch (status)
	{
	case OCI_SUCCESS:
		break;
	case OCI_NO_DATA:
		break;
	case OCI_SUCCESS_WITH_INFO:
		CNPRINTF("Error - OCI_SUCCESS_WITH_INFO\n");
		CNPRINTF(strSQL);
		CNPRINTF("\r\n");
		break;
	case OCI_NEED_DATA:
		CNPRINTF("Error - OCI_NEED_DATA\n");
		CNPRINTF(strSQL);
		CNPRINTF("\r\n");
		break;
	case OCI_ERROR:
		OCIErrorGet((dvoid *)errhp, (ub4) 1, (text *) NULL, &errcode,
			errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
		CNPRINTF((char*)errbuf);
		CNPRINTF(strSQL);
		CNPRINTF("\r\n");
		//CNPRINTF("Error - %.*s\n", 512, errbuf);
		break;
	case OCI_STILL_EXECUTING:
		CNPRINTF("Error - OCI_STILL_EXECUTE\n");
		CNPRINTF(strSQL);
		CNPRINTF("\r\n");
		break;
	case OCI_INVALID_HANDLE:
		CNPRINTF("Error - OCI_INVALID_HANDLE\n");
		CNPRINTF(strSQL);
		CNPRINTF("\r\n");
		break;
	case OCI_CONTINUE:
		CNPRINTF("Error - OCI_CONTINUE\n");
		CNPRINTF(strSQL);
		CNPRINTF("\r\n");
		break;
	default:
		CNPRINTF(strSQL);
		CNPRINTF("\r\n");
		break;
	}
	return status;
} /* end checkerr() */


OraDatabase::OraDatabase() :
m_pEnv(NULL), 
m_pErr(NULL),
m_pSvcCtx(NULL),
m_pServer(NULL),
m_pSession(NULL),
m_pTrans(NULL),
m_pStmt(NULL)
{
	memset(&m_ConnInfo, 0, sizeof(m_ConnInfo));
}

int OraDatabase::Init() {
	sword swRetval = OCIEnvCreate(&m_pEnv, OCI_THREADED|OCI_EVENTS|OCI_OBJECT, NULL,NULL, NULL,NULL, 0, NULL);
	assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	if (!(swRetval==OCI_SUCCESS_WITH_INFO || swRetval == OCI_SUCCESS)) {
		return CN_FAIL;
	}
	g_pEnv = m_pEnv;
	swRetval = OCIHandleAlloc(m_pEnv,(void**)&m_pErr, OCI_HTYPE_ERROR, 0, NULL); assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	g_pErr = m_pErr;
	
	swRetval = OCIHandleAlloc(m_pEnv,(void**)&m_pSvcCtx, OCI_HTYPE_SVCCTX, 0, NULL); assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	swRetval = OCIHandleAlloc(m_pEnv,(void**)&m_pServer, OCI_HTYPE_SERVER , 0, NULL); assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	swRetval = OCIAttrSet(m_pSvcCtx,OCI_HTYPE_SVCCTX, m_pServer, 0,OCI_ATTR_SERVER, m_pErr);assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	swRetval = OCIHandleAlloc(m_pEnv,(void**)&m_pSession, OCI_HTYPE_SESSION , 0, NULL); assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	swRetval = OCIAttrSet(m_pSvcCtx,OCI_HTYPE_SVCCTX, m_pSession, 0,OCI_ATTR_SESSION, m_pErr);assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	swRetval = OCIHandleAlloc(m_pEnv,(void**)&m_pTrans, OCI_HTYPE_TRANS, 0, NULL); assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	swRetval = OCIHandleAlloc(m_pEnv,(void**)&m_pStmt, OCI_HTYPE_STMT, 0, NULL); assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	swRetval = OCIHandleAlloc (m_pEnv, (dvoid **) &m_pSubscription,OCI_HTYPE_SUBSCRIPTION, (size_t) 0, (dvoid **) 0);assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);

	//CNPRINTF(".");
	return CN_SUCCESS; 
}

char g_fmt[128]="(DESCRIPTION = (ADDRESS_LIST = (ADDRESS = (PROTOCOL = TCP)(HOST = %s)(PORT = %d))) (CONNECT_DATA = (SERVICE_NAME = %s)))";
int OraDatabase::Connect( const OraConnInfo& dbConnInfo ) {
	char conn[256]={0};
	sprintf_s(conn,g_fmt,dbConnInfo.host, dbConnInfo.port, dbConnInfo.dbName);
	
	sword swRetval = checkerr(m_pErr, OCIServerAttach(m_pServer, m_pErr, (const OraText*)conn, strlen(conn), OCI_DEFAULT));
	if (!(swRetval==OCI_SUCCESS_WITH_INFO || swRetval == OCI_SUCCESS)) {
		return CN_FAIL;
	}

	swRetval = checkerr(m_pErr, OCIAttrSet(m_pSession, OCI_HTYPE_SESSION, (void*)dbConnInfo.login, (ub4)strlen(dbConnInfo.login), OCI_ATTR_USERNAME, m_pErr));
	if (!(swRetval==OCI_SUCCESS_WITH_INFO || swRetval == OCI_SUCCESS)) {
		return CN_FAIL;
	}
	
	swRetval=checkerr(m_pErr, OCIAttrSet(m_pSession, OCI_HTYPE_SESSION, (void*)dbConnInfo.passwd, (ub4)strlen(dbConnInfo.passwd), OCI_ATTR_PASSWORD, m_pErr));
	assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	if (!(swRetval==OCI_SUCCESS_WITH_INFO || swRetval == OCI_SUCCESS)) {
		return CN_FAIL;
	}

	swRetval = checkerr(m_pErr, OCISessionBegin(m_pSvcCtx, m_pErr, m_pSession, OCI_CRED_RDBMS,OCI_DEFAULT));
	if (!(swRetval==OCI_SUCCESS_WITH_INFO || swRetval == OCI_SUCCESS)) {
		return CN_FAIL;
	}
	assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO); 

	memcpy(&m_ConnInfo, &dbConnInfo, sizeof(m_ConnInfo));

	return CN_SUCCESS;
}

int OraDatabase::Destroy() { 

	memset(&m_ConnInfo, 0, sizeof(m_ConnInfo));

	sword swRetval = OCIHandleFree(m_pErr,OCI_HTYPE_ERROR);
	assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO); 
	swRetval = OCIHandleFree(m_pSvcCtx,OCI_HTYPE_SVCCTX);
	assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO); 
	swRetval = OCIHandleFree(m_pServer,OCI_HTYPE_SERVER);
	assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO); 
	swRetval = OCIHandleFree(m_pSession,OCI_HTYPE_SESSION);
	assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO); 
	swRetval = OCIHandleFree(m_pTrans,OCI_HTYPE_TRANS);
	assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO); 
	swRetval = OCIHandleFree(m_pStmt,OCI_HTYPE_STMT);
	assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO); 
	swRetval = OCIHandleFree(m_pEnv,OCI_HTYPE_ENV);
	assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO); 

	return CN_SUCCESS;
}

int OraDatabase::DisConnect() {
	sword swRetval=OCISessionEnd(m_pSvcCtx, m_pErr, m_pSession, OCI_DEFAULT);
	assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO); 
	swRetval = OCIServerDetach(m_pServer, m_pErr, OCI_DEFAULT);
	assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO); 

	return CN_SUCCESS; 
}

bool OraDatabase::ExecSQL( const char* strSql )
{
	if(OCI_SUCCESS != checkerr(m_pErr, OCIStmtPrepare(m_pStmt,
		m_pErr, (OraText*)strSql, (ub4) strlen((char *)strSql),
		(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT),strSql))
		return FALSE;

	if(OCI_SUCCESS != checkerr(m_pErr, OCIStmtExecute(m_pSvcCtx,m_pStmt, m_pErr,
		(ub4)1,(ub4)0, (OCISnapshot *) NULL, 
		(OCISnapshot *) NULL, (ub4)OCI_DEFAULT),strSql))
		return FALSE;
	if(OCI_SUCCESS != checkerr(m_pErr, OCITransCommit(m_pSvcCtx,
		m_pErr,
		OCI_DEFAULT),strSql))
		return FALSE;

	return TRUE;
}

int OraDatabase::Query( const char* strSQL, CNVARIANT &value){
	OCIDefine *defnp = (OCIDefine *) NULL;
	if(OCI_SUCCESS != checkerr(m_pErr, OCIStmtPrepare(m_pStmt,
		m_pErr, (OraText*)strSQL, (ub4) strlen(strSQL), 
		(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT),strSQL))
		return CN_FAIL;
	switch(value.eDataType)
	{
	case ORAINT:
		if(OCI_SUCCESS != checkerr(m_pErr,OCIDefineByPos(m_pStmt, &defnp, m_pErr,
			1,(dvoid*) &value.iValue, sizeof(sword), 
			(ub2)SQLT_INT, (dvoid*) 0, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT),strSQL))
			return CN_FAIL;
		break;
	case ORAFLOAT:
		if(OCI_SUCCESS != checkerr(m_pErr,OCIDefineByPos(m_pStmt, &defnp, m_pErr,
			1,(dvoid*) &value.fValue, sizeof(float), 
			(ub2)SQLT_FLT, (dvoid*) 0, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT),strSQL))
			return CN_FAIL;
		break; 

	case ORATEXT:
		if(OCI_SUCCESS != checkerr(m_pErr,OCIDefineByPos(m_pStmt, &defnp, m_pErr,
			(ub4) 1,(dvoid*) value.pValue, 255, 
			(ub2)SQLT_STR, (dvoid*) 0, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT),strSQL))
			return CN_FAIL;
		break;
	case ORANULL:
		break;
	default:
		break;
	}
	sword swRetval=checkerr(m_pErr,OCIStmtExecute(m_pSvcCtx, m_pStmt,
		m_pErr, (ub4)1, (ub4)0,
		(OCISnapshot *) NULL, (OCISnapshot *) NULL,
		(ub4)OCI_DEFAULT),strSQL);

	if(OCI_NO_DATA == swRetval)
		return CN_NODATA;
	else if(OCI_SUCCESS != swRetval)
		return CN_FAIL;

	return CN_SUCCESS; 
}

int OraDatabase::Query( const char* strSQL, CNVARIANT* p, int nCount){
	if (p==NULL) {
		return CN_FAIL;
	}
	OCIDefine *defnp = (OCIDefine *) NULL;
	if(OCI_SUCCESS != checkerr(m_pErr, OCIStmtPrepare(m_pStmt,
		m_pErr, (OraText*)strSQL, (ub4) strlen(strSQL), 
		(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT),strSQL))
		return CN_FAIL;
	int i=0,j=0;
	for (;i<nCount;i++) {
		j++;
		switch(p[i].eDataType)
		{
		case ORAINT:
			if(OCI_SUCCESS != checkerr(m_pErr,OCIDefineByPos(m_pStmt, &defnp, m_pErr,
				j,(dvoid*) &p[i].iValue, sizeof(int), 
				(ub2)SQLT_INT, (dvoid*) 0, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT),strSQL))
				return CN_FAIL;
			break;
		case ORAFLOAT:
			if(OCI_SUCCESS != checkerr(m_pErr,OCIDefineByPos(m_pStmt, &defnp, m_pErr,
				j,(dvoid*) &p[i].fValue, sizeof(float), 
				(ub2)SQLT_FLT, (dvoid*) 0, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT),strSQL))
				return CN_FAIL;
			break; 

		case ORATEXT:
			if(OCI_SUCCESS != checkerr(m_pErr,OCIDefineByPos(m_pStmt, &defnp, m_pErr,
				j,(dvoid*) &p[i].pValue, 255, 
				(ub2)SQLT_STR, (dvoid*) 0, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT),strSQL))
				return CN_FAIL;
			break;
		case ORANULL:
			break;
		default:
			assert(0);
			break;
		}
	}

	sword swRetval=checkerr(m_pErr,OCIStmtExecute(m_pSvcCtx, m_pStmt, m_pErr, 
		(ub4)0, (ub4)0, (OCISnapshot *) NULL, (OCISnapshot *) NULL, (ub4)OCI_DEFAULT),strSQL);
	if(OCI_NO_DATA == swRetval)
		return CN_NODATA;
	else if(OCI_SUCCESS != swRetval)
		return CN_FAIL;

	return CN_SUCCESS;
}

int OraDatabase::Fetch() {
	sword swRetval=OCIStmtFetch2(m_pStmt, m_pErr, (ub4) 1, (ub2) OCI_FETCH_NEXT,
		(sb4) 0, OCI_DEFAULT);
	if (swRetval == OCI_NO_DATA) { 
		return CN_NODATA;
	}else if (swRetval == OCI_NEED_DATA) {
		return CN_NEEDMORE;
	}else if(swRetval != OCI_SUCCESS){
		return CN_FAIL;
	}

	return CN_SUCCESS;
}

int OraDatabase::GetColCount() {
	ub4 paramcnt;
	OCIAttrGet((dvoid*)m_pStmt, (ub4)OCI_HTYPE_STMT,(dvoid*)&paramcnt,  (ub4)0, (ub4)OCI_ATTR_PARAM_COUNT, m_pErr);


	OCIAttrGet((dvoid*)m_pStmt, (ub4)OCI_HTYPE_STMT,(dvoid*)&paramcnt,  (ub4)0, (ub4)OCI_ATTR_LIST_COLUMNS, m_pErr);

	return paramcnt;
}

bool OraDatabase::IsConnectionValid() {
	ub4 ServerStatus = 0;
	OCIServer* srv = (OCIServer*)NULL;
	checkerr(m_pErr, OCIAttrGet(m_pSvcCtx, OCI_HTYPE_SVCCTX, &srv, 
		0,OCI_ATTR_SERVER, m_pErr));
	checkerr(m_pErr, OCIAttrGet(srv, OCI_HTYPE_SERVER,&ServerStatus, (ub4*)NULL, OCI_ATTR_SERVER_STATUS, m_pErr));
	if (ServerStatus == OCI_SERVER_NOT_CONNECTED) {
		return false;
	}

	return true; 
}

/*int OraDatabase::Query( const char* strSQL, std::vector<CNVARIANT>& vVal, int nRow ) {
	OCIDefine *defnp = (OCIDefine *) NULL;
	//OCIDefine *defnp1 = (OCIDefine *) NULL;
	//OCIDefine *defnp2 = (OCIDefine *) NULL;
	if(OCI_SUCCESS != checkerr(m_pErr, OCIStmtPrepare(m_pStmt,
		m_pErr, (OraText*)strSQL, (ub4) strlen(strSQL), 
		(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT)))
		return CN_FAIL;
	vector<CNVARIANT>::iterator it = vVal.begin();
	int j=0;
	for (;it!=vVal.end();it++) {
		j++;
		switch(it->eDataType)
		{
		case ORAINT:
			if(OCI_SUCCESS != checkerr(m_pErr,OCIDefineByPos(m_pStmt, &defnp, m_pErr,
				j,(dvoid*) &it->iValue, sizeof(int), 
				(ub2)SQLT_INT, (dvoid*) 0, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT)))
				return CN_FAIL;
			break;
		case ORAFLOAT:
			if(OCI_SUCCESS != checkerr(m_pErr,OCIDefineByPos(m_pStmt, &defnp, m_pErr,
				j,(dvoid*) &it->fValue, sizeof(float), 
				(ub2)SQLT_FLT, (dvoid*) 0, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT)))
				return CN_FAIL;
			break; 

		case ORATEXT:
			if(OCI_SUCCESS != checkerr(m_pErr,OCIDefineByPos(m_pStmt, &defnp, m_pErr,
				j,(dvoid*) it->pValue, 255, 
				(ub2)SQLT_STR, (dvoid*) 0, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT)))
				return CN_FAIL;
			break;
		case ORANULL:
			break;
		default:
			assert(0);
			break;
		}
	}


	sword status = checkerr(m_pErr,OCIStmtExecute(m_pSvcCtx, m_pStmt,
		m_pErr, (ub4)0, (ub4)0,
		(OCISnapshot *) NULL, (OCISnapshot *) NULL,
		(ub4)OCI_DEFAULT));
	if (status == OCI_NO_DATA) {
		return CN_NODATA;
	}else if(status != OCI_SUCCESS) {
		return CN_FAIL; 
	}

	int flag = 0;
	while ((status = OCIStmtFetch2(m_pStmt, m_pErr, (ub4) 1, (ub2) OCI_FETCH_NEXT,
		(sb4) 0, OCI_DEFAULT)) == OCI_SUCCESS) {
			if (nRow == flag++) {
				break;
			}
	}
	if (status == OCI_NO_DATA) {
		return CN_NODATA;
	}else if(status == OCI_NEED_DATA){
		return CN_NEEDMORE;
	} else if (OCI_SUCCESS != status) {
		checkerr(m_pErr, status);
		return CN_FAIL;
	}

	return CN_SUCCESS;
}



*/