#include "DatabasePool.h"
int main(){
	
	OraConnInfo connInfo;
	connInfo.dbName="ORCL";
	connInfo.host="192,168.1.166";
	connInfo.port = 1521;
	connInfo.login ="zhangkai";
	connInfo.passwd = "zhangkai";
	DatabasePool dbpool;
	dbpool.Create(connInfo, 10);
	OraDatabase *pOraDb;
	for ( int i = 1; i <200 ; i++) {
		pOraDb = dbpool.GetConnection(i);
	}
	for ( int i = 1; i < 200; i++) {
		dbpool.PutbackConnection(i);
	}
 	for ( int i = 1; i < 200; i++) {
 		dbpool.DeleteConnection(i);
 	}
	
	return 0;
}