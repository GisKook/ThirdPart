/*
 * brief: wrapper of Continuous Query Notification. 
 * function list:
 * 
 * author: a staff of CETCNAV
 * date: 2014Äê10ÔÂ10ÈÕ
 */

#include "DatabaseDef.h"

#ifndef LIBOCI_ORADB_MONITOR
#define LIBOCI_ORADB_MONITOR

struct oradb_monitor_result{
	char * dbname;
	char * tablename;
	unsigned char opcode; 
	char rowid[19]; // magic 18 for rowid is 18 length
};

typedef void (*oradb_monitor_callback)(struct oradb_monitor_result *);

struct oradb_monitor{ 
	OCIEnv* env; 
	OCIError* err;
	OCISvcCtx* svcctx;
	OCIServer* server;
	OCISession* session;
	//OCITrans* trans;
	OCIStmt* stmt;
	OCISubscription* subscription;
	oradb_monitor_callback callback;
};

enum oradb_monitor_opcode{ 
	oradb_monitor_insert = 2,
	oradb_monitor_update = 4,
	oradb_monitor_delete = 8,
};

ENGINE_API struct oradb_monitor* oradb_monitor_create(OraConnInfo * conninfo);

ENGINE_API void oradb_monitor_destroy(struct oradb_monitor*);

ENGINE_API int oradb_monitor_register(struct oradb_monitor*);

ENGINE_API int oradb_monitor_monitor(struct oradb_monitor* ,const char* sql, CNVARIANT* val, unsigned char valcount);

ENGINE_API void oradb_monitor_setcallback(struct oradb_monitor*, oradb_monitor_callback);

#endif