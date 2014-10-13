#include "oci.h"
#include "oradb_monitor.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

extern char g_fmt[128];
struct oradb_monitor *g_odbm;

#define CNPRINTF printf
int checkerr(OCIError *errhp,sword status)
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
		break;
	case OCI_NEED_DATA:
		CNPRINTF("Error - OCI_NEED_DATA\n");
		break;
	case OCI_ERROR:
		OCIErrorGet((dvoid *)errhp, (ub4) 1, (text *) NULL, &errcode,
			errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
		CNPRINTF((char*)errbuf);
		//CNPRINTF("Error - %.*s\n", 512, errbuf);
		break;
	case OCI_STILL_EXECUTING:
		CNPRINTF("Error - OCI_STILL_EXECUTE\n");
		break;
	case OCI_INVALID_HANDLE:
		CNPRINTF("Error - OCI_INVALID_HANDLE\n");
		break;
	case OCI_CONTINUE:
		CNPRINTF("Error - OCI_CONTINUE\n");
		break;
	default:
		break;
	}
	return status;
}; /* end checkerr() */

#if 0
void processrowchanges(OCIEnv *envhp, OCIError *errhp, OCIStmt *stmthp,
					   OCIColl *row_changes)
{
	dvoid *row_descp;
	dvoid *row_desc;
	boolean exist; 
	ub2 i;
	dvoid *elemind = (dvoid *)0;
	oratext *row_id;
	ub4 row_op;


	sb4 num_rows;
	if (!row_changes) return;
	checkerr(errhp, OCICollSize(envhp, errhp,
		(CONST OCIColl *) row_changes, &num_rows));
	for (i=0; i<num_rows; i++)
	{
		checkerr(errhp, OCICollGetElem(envhp,
			errhp, (OCIColl *) row_changes,
			i, &exist, &row_descp, &elemind));

		row_desc = row_descp;
		checkerr(errhp, OCIAttrGet (row_desc, 
			OCI_DTYPE_ROW_CHDES, (dvoid *)&row_id,
			NULL, OCI_ATTR_CHDES_ROW_ROWID, errhp));
		checkerr(errhp, OCIAttrGet (row_desc, 
			OCI_DTYPE_ROW_CHDES, (dvoid *)&row_op, 
			NULL, OCI_ATTR_CHDES_ROW_OPFLAGS, errhp));

		printf ("Row changed is %s row_op %d\n", row_id, row_op);
		fflush(stdout); 
	}  
}

void processtablechanges(struct oradb_monitor* odbm, OCIColl *table_changes)
{
	dvoid *table_descp;
	dvoid *table_desc;
	dvoid **row_descp;
	dvoid *row_desc;
	OCIColl *row_changes = (OCIColl *)0;
	boolean exist; 
	ub2 i;
	dvoid *elemind = (dvoid *)0;
	oratext *table_name;
	ub4 table_op;


	sb4 num_tables;
	if (!table_changes) return;
	checkerr(odbm->err, OCICollSize(odbm->env, odbm->err,
		(CONST OCIColl *) table_changes, &num_tables));
	for (i=0; i<num_tables; i++)
	{
		checkerr(odbm->err, OCICollGetElem(odbm->env,
			odbm->err, (OCIColl *) table_changes,
			i, &exist, &table_descp, &elemind));

		table_desc = table_descp;
		checkerr(odbm->err, OCIAttrGet (table_desc, 
			OCI_DTYPE_TABLE_CHDES, (dvoid *)&table_name,
			NULL, OCI_ATTR_CHDES_TABLE_NAME, odbm->err));
		checkerr(odbm->err, OCIAttrGet (table_desc, 
			OCI_DTYPE_TABLE_CHDES, (dvoid *)&table_op, 
			NULL, OCI_ATTR_CHDES_TABLE_OPFLAGS, odbm->err));
		checkerr(odbm->err, OCIAttrGet (table_desc, 
			OCI_DTYPE_TABLE_CHDES, (dvoid *)&row_changes, 
			NULL, OCI_ATTR_CHDES_TABLE_ROW_CHANGES, odbm->err));

		printf ("Table changed is %s table_op %d\n", table_name,table_op);
		fflush(stdout); 
		//	if (!bit(table_op, OCI_OPCODE_ALLROWS))
		//		processRowChanges(envhp, errhp, stmthp, row_changes);
	}  
}

void processquerychanges(struct oradb_monitor* odbm, OCIColl *query_changes)
{
	sb4 num_queries;
	ub8 queryid;
	ub4 queryop;
	dvoid *elemind = (dvoid *)0;
	dvoid *query_desc;
	dvoid *query_descp;
	ub2 i;
	boolean exist;
	OCIColl *table_changes = (OCIColl *)0;

	if (!query_changes) return;
	checkerr(odbm->err, OCICollSize(odbm->env, odbm->err,
		(CONST OCIColl *) query_changes, &num_queries));
	for (i=0; i < num_queries; i++)
	{
		checkerr(odbm->err, OCICollGetElem(odbm->env,
			odbm->err, (OCIColl *) query_changes,
			i, &exist, &query_descp, &elemind));

		query_desc = query_descp;
		checkerr(odbm->err, OCIAttrGet (query_desc,
			OCI_DTYPE_CQDES, (dvoid *)&queryid,
			NULL, OCI_ATTR_CQDES_QUERYID, odbm->err));
		checkerr(odbm->err, OCIAttrGet (query_desc,
			OCI_DTYPE_CQDES, (dvoid *)&queryop,
			NULL, OCI_ATTR_CQDES_OPERATION, odbm->err));
		printf(" Query %llu is changed queryop is %d\n", queryid, queryop);
		if (queryop == OCI_EVENT_DEREG)
			printf("Query Deregistered\n");
		checkerr(odbm->err, OCIAttrGet (query_desc,
			OCI_DTYPE_CQDES, (dvoid *)&table_changes,
			NULL, OCI_ATTR_CQDES_TABLE_CHANGES, odbm->err));

		processtablechanges(odbm,table_changes);
	}
}
#endif
void registersubscriptioncallback(void *ctx, OCISubscription *subscrhp, void *payload, unsigned int *payl, void *descriptor, unsigned int mode){
	/*
	按照oracle官网上"Continuous Query Notification(version 11.2)"根本就行不通，也不知道是哪里错了，但是可以根据payload中的内存中分析出表名称、操作、rowid
	先用分析内存的方法搞吧。
	不用通过subscrhp得到通知类型了，在register时已经定死了。就是query change 类型。
	*/ 
	if (*g_odbm->callback== NULL ) {
		fprintf(stdout, "oracle database monitor is not set callback function.\n");
		return;
	} 
	if((int)(payl) <= 54){  // 54 magic number get from list codes 12+dbnamelen+35 plus 18 byte rowid 
		fprintf(stdout, "oracle call back function get payload is not current.\n");
		return ;
	}
	struct oradb_monitor_result omr; 
	/*
	 *接下来这段代码是要分析内存.超级多的magic number都是根据payload分析出来的. :P 这超级多的magic number 都是在在register 时设置了
	 OCI_ATTR_CHNF_ROWIDS 和 OCI_SUBSCR_CQ_QOS_QUERY两个。如果这两个改了分析内存的代码也得跟着改.目前在两个oracle上测试都能正常使用。

	 checkerr(odbm->err, OCIAttrSet(odbm->subscription, OCI_HTYPE_SUBSCRIPTION,
	 (dvoid *)&rowids, sizeof(ub4), 
	 OCI_ATTR_CHNF_ROWIDS, odbm->err));

	// 	ub4 qosflags = OCI_SUBSCR_CQ_QOS_QUERY;
	checkerr(odbm->err, OCIAttrSet(odbm->subscription, OCI_HTYPE_SUBSCRIPTION,
	(dvoid *)&qosflags, sizeof(ub4),
		OCI_ATTR_SUBSCR_CQ_QOSFLAGS, odbm->err));
	 */
	unsigned char dbnamelen = 0;
	dbnamelen = *(((unsigned char*)payload)+11);
	omr.dbname = (char*)malloc(dbnamelen+1);
	memcpy((void*)omr.dbname, (((unsigned char*)(payload))+12), dbnamelen);
	omr.dbname[dbnamelen] = '\0';
	unsigned char tablenamelen = 0;
	tablenamelen = *(((unsigned char*)payload)+12+dbnamelen+35);
	omr.tablename = (char*)malloc(tablenamelen+1);
	memcpy((void*)omr.tablename, (((unsigned char*)(payload))+12+dbnamelen+36), tablenamelen);
	omr.tablename[tablenamelen] = 0;
	memcpy(omr.rowid, ((unsigned char*)payload) + (int)payl - 18, 18);
	omr.rowid[18] = '\0';
	omr.opcode = *((unsigned char*)(((unsigned char*)payload)+(int)payl-21));
	(g_odbm->callback)(&omr);

	/*OCIColl *table_changes = (OCIColl *)0;
	OCIColl *row_changes = (OCIColl *)0;
	dvoid *change_descriptor = descriptor;
	ub4 notify_type;
	OCIColl *query_changes = (OCIColl *)0;

	OCIAttrGet (change_descriptor, OCI_DTYPE_CHDES, (dvoid *) &notify_type,
		NULL, OCI_ATTR_CHDES_NFYTYPE, g_odbm->err);

	if (notify_type == OCI_EVENT_SHUTDOWN ||
		notify_type == OCI_EVENT_SHUTDOWN_ANY)
	{
		printf("SHUTDOWN NOTIFICATION RECEIVED\n");
		fflush(stdout);
		return;
	}
	if (notify_type == OCI_EVENT_STARTUP)
	{
		printf("STARTUP NOTIFICATION RECEIVED\n");
		fflush(stdout);
		return;
	}
	if (notify_type == OCI_EVENT_OBJCHANGE)
	{
		printf("notify obj\n");
		checkerr(g_odbm->err, OCIAttrGet (change_descriptor,
			OCI_DTYPE_CHDES, &table_changes, NULL,
			OCI_ATTR_CHDES_TABLE_CHANGES, g_odbm->err));
		processtablechanges(g_odbm, table_changes);
	}
	else if (notify_type == OCI_EVENT_QUERYCHANGE)
	{
		checkerr(g_odbm->err, OCIAttrGet (change_descriptor,
			OCI_DTYPE_CHDES, &query_changes, NULL,
			OCI_ATTR_CHDES_QUERIES, g_odbm->err));
		processquerychanges(g_odbm,query_changes);
		printf("notify\n");
	}else{
		printf("....%d\n", notify_type);
	}
	*/
};

struct oradb_monitor* oradb_monitor_create(OraConnInfo * conninfo)
{
	struct oradb_monitor* odbm = (struct oradb_monitor*)malloc(sizeof(struct oradb_monitor));
	memset(odbm, 0, sizeof(struct oradb_monitor));
	sword swRetval = ::OCIEnvCreate(&odbm->env, OCI_EVENTS|OCI_OBJECT, NULL,NULL, NULL,NULL, 0, NULL);
	assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	if (!(swRetval==OCI_SUCCESS_WITH_INFO || swRetval == OCI_SUCCESS)) {
		return NULL;
	}
	swRetval = OCIHandleAlloc(odbm->env,(void**)&odbm->err, OCI_HTYPE_ERROR, 0, NULL); assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	swRetval = OCIHandleAlloc(odbm->env,(void**)&odbm->svcctx, OCI_HTYPE_SVCCTX, 0, NULL); assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	swRetval = OCIHandleAlloc(odbm->env,(void**)&odbm->server, OCI_HTYPE_SERVER , 0, NULL); assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	swRetval = OCIAttrSet(odbm->svcctx,OCI_HTYPE_SVCCTX, odbm->server, 0,OCI_ATTR_SERVER, odbm->err);assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	swRetval = OCIHandleAlloc(odbm->env,(void**)&odbm->session, OCI_HTYPE_SESSION , 0, NULL); assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	swRetval = OCIAttrSet(odbm->svcctx,OCI_HTYPE_SVCCTX, odbm->session, 0,OCI_ATTR_SESSION, odbm->err);assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	//swRetval OCIHandleAlloc(odbm->env,(void**)&m_pTrans, OCI_HTYPE_TRANS, 0, NULL); assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	swRetval = OCIHandleAlloc(odbm->env,(void**)&odbm->stmt, OCI_HTYPE_STMT, 0, NULL); assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	swRetval = OCIHandleAlloc(odbm->env, (dvoid **)&odbm->subscription,OCI_HTYPE_SUBSCRIPTION, (size_t) 0, (dvoid **) 0);

	char conn[256]={0};
	sprintf_s(conn,g_fmt,conninfo->host, conninfo->port, conninfo->dbName);

	swRetval = checkerr(odbm->err, OCIServerAttach(odbm->server, odbm->err, (const OraText*)conn, strlen(conn), OCI_DEFAULT));
	if (!(swRetval==OCI_SUCCESS_WITH_INFO || swRetval == OCI_SUCCESS)) {
		return NULL;
	}

	swRetval = checkerr(odbm->err, OCIAttrSet(odbm->session, OCI_HTYPE_SESSION, (void*)conninfo->login, (ub4)strlen(conninfo->login), OCI_ATTR_USERNAME, odbm->err));
	if (!(swRetval==OCI_SUCCESS_WITH_INFO || swRetval == OCI_SUCCESS)) {
		return NULL;
	}

	swRetval=checkerr(odbm->err, OCIAttrSet(odbm->session, OCI_HTYPE_SESSION, (void*)conninfo->passwd, (ub4)strlen(conninfo->passwd), OCI_ATTR_PASSWORD, odbm->err));
	assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);
	if (!(swRetval==OCI_SUCCESS_WITH_INFO || swRetval == OCI_SUCCESS)) {
		return NULL;
	}

	swRetval = checkerr(odbm->err, OCISessionBegin(odbm->svcctx, odbm->err, odbm->session, OCI_CRED_RDBMS,OCI_DEFAULT));
	if (!(swRetval==OCI_SUCCESS_WITH_INFO || swRetval == OCI_SUCCESS)) {
		return NULL;
	}
	assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO); 

	g_odbm = odbm;
	return odbm; 
}

void oradb_monitor_destroy( struct oradb_monitor* odbm)
{
	checkerr(odbm->err, OCISubscriptionUnRegister(odbm->svcctx, odbm->subscription, odbm->err, OCI_DEFAULT));
	checkerr(odbm->err, OCISessionEnd(odbm->svcctx, odbm->err, odbm->session, OCI_DEFAULT));
	checkerr(odbm->err, OCIServerDetach(odbm->server, odbm->err, OCI_DEFAULT));

	OCIHandleFree(odbm->err,OCI_HTYPE_ERROR);
	OCIHandleFree(odbm->svcctx,OCI_HTYPE_SVCCTX);
	OCIHandleFree(odbm->server,OCI_HTYPE_SERVER);
	OCIHandleFree(odbm->session,OCI_HTYPE_SESSION);
	OCIHandleFree(odbm->env,OCI_HTYPE_ENV);
}

int oradb_monitor_register( struct oradb_monitor* odbm)
{
	ub4 subnamespace = OCI_SUBSCR_NAMESPACE_DBCHANGE;
	boolean rowids = TRUE;
	ub4 qosflags = OCI_SUBSCR_CQ_QOS_QUERY;
	ub4 qosstatus = OCI_SUBSCR_QOS_RELIABLE;
	ub4 timeout = 1;

	//ub4 opcode = OCI_OPCODE_ALLOPS; //OCI_OPCODE_DELETE;

	/* set the namespace to DBCHANGE */
 	checkerr(odbm->err, OCIAttrSet(odbm->subscription, OCI_HTYPE_SUBSCRIPTION,
 		(dvoid *)&subnamespace, sizeof(ub4),
 		OCI_ATTR_SUBSCR_NAMESPACE, odbm->err));
 
 	checkerr(odbm->err, OCIAttrSet (odbm->subscription, OCI_HTYPE_SUBSCRIPTION,
 		(void *)registersubscriptioncallback, 0, OCI_ATTR_SUBSCR_CALLBACK, odbm->err));

 	/* Allow extraction of rowid information 
 	OCI_ATTR_CHNF_ROWIDS - A Boolean attribute (default FALSE). 
 	If set to TRUE, then the continuous query notification message includes row-level details such as operation type and ROWID.
	*/
 	checkerr(odbm->err, OCIAttrSet(odbm->subscription, OCI_HTYPE_SUBSCRIPTION,
 		(dvoid *)&rowids, sizeof(ub4), 
 		/*OCI_ATTR_CHNF_OPERATIONS*/OCI_ATTR_CHNF_ROWIDS, odbm->err));

    checkerr(odbm->err, OCIAttrSet(odbm->subscription, OCI_HTYPE_SUBSCRIPTION,
 		(dvoid *)&qosflags, sizeof(ub4),
 		OCI_ATTR_SUBSCR_CQ_QOSFLAGS, odbm->err));

//	checkerr(odbm->err, OCIAttrSet(odbm->subscription, OCI_HTYPE_SUBSCRIPTION,
//		(dvoid *)&timeout, sizeof(ub4),
//		OCI_ATTR_SUBSCR_TIMEOUT, odbm->err));
// 	checkerr(odbm->err, OCIAttrSet(odbm->subscription, OCI_HTYPE_SUBSCRIPTION,
// 		(dvoid *)&qosstatus, sizeof(ub4),
// 		OCI_ATTR_SUBSCR_QOSFLAGS,odbm->err));
		
 	// OCI_ATTR_CHNF_OPERATIONS - Use this ub4 flag to selectively filter notifications based on operation type. 
 	// This option is ignored if the registration is of query-level granularity. Flags stored are as follows:	
//  	checkerr(odbm->err, OCIAttrSet(odbm->subscription, OCI_HTYPE_SUBSCRIPTION,
//  		(dvoid*)&opcode, sizeof(ub4),
//  		OCI_ATTR_CHNF_OPERATIONS, odbm->err));
 
 	/* Create a new registration in the DBCHANGE namespace */

 	checkerr(odbm->err, OCISubscriptionRegister(odbm->svcctx, &odbm->subscription, 1, odbm->err, OCI_DEFAULT));
	return 0;
}

int oradb_monitor_monitor(struct oradb_monitor* odbm, const char* sql, CNVARIANT * val, unsigned char valcount)
{
	OCIDefine *defnp = (OCIDefine *)0;
	int count = 0;

	checkerr(odbm->err, OCIStmtPrepare (odbm->stmt, odbm->err, 
		(const OraText*)sql, (ub4)strlen((const char *)sql), OCI_V7_SYNTAX, OCI_DEFAULT));

	int i=0,j=0;
	for (;i<valcount;i++) {
		j++;
		switch(val[i].eDataType)
		{
		case ORAINT:
			if(OCI_SUCCESS != checkerr(odbm->err,OCIDefineByPos(odbm->stmt, &defnp, odbm->err,
				j,(dvoid*) &val[i].iValue, sizeof(int), 
				(ub2)SQLT_INT, (dvoid*) 0, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT)))
				return CN_FAIL;
			break;
		case ORAFLOAT:
			if(OCI_SUCCESS != checkerr(odbm->err,OCIDefineByPos(odbm->stmt, &defnp, odbm->err,
				j,(dvoid*) &val[i].fValue, sizeof(float), 
				(ub2)SQLT_FLT, (dvoid*) 0, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT)))
				return CN_FAIL;
			break; 

		case ORATEXT:
			if(OCI_SUCCESS != checkerr(odbm->err,OCIDefineByPos(odbm->stmt, &defnp, odbm->err,
				j,(dvoid*) &val[i].pValue, 255, 
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


	/* Associate the statement with the subscription handle */
	OCIAttrSet (odbm->stmt, OCI_HTYPE_STMT, odbm->subscription, 0,
		OCI_ATTR_CHNF_REGHANDLE, odbm->err);

	/* Execute the statement, the execution performs object registration */
	checkerr(odbm->err, OCIStmtExecute (odbm->svcctx, odbm->stmt, odbm->err, (ub4) 1, (ub4) 0,
		(CONST OCISnapshot *) NULL, (OCISnapshot *) NULL ,
		OCI_DEFAULT));

//	ub8 qid;
//	OCIAttrGet(odbm->stmt, OCI_HTYPE_STMT, &qid, (ub4 *)0,
//		OCI_ATTR_CQ_QUERYID, odbm->err);
//	printf("Query Id %d\n", qid);

	/* commit */
	checkerr(odbm->err, OCITransCommit(odbm->svcctx, odbm->err, (ub4) 0));
	return 0;
}

void oradb_monitor_setcallback( struct oradb_monitor* odbm, oradb_monitor_callback callback)
{
	odbm->callback = callback; 
}

