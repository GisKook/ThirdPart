#include "PGDatabase.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
	PGConnInfo conn;
	conn.pghost = "192.168.1.155";
	conn.pgport = "5432";
	conn.login = "postgres";
	conn.passwd = "cetc";
	conn.dbName = "beidou";

	PGDatabase db;
	db.Connect(conn);
	db.BeginTransaction();
	db.Exec("drop table data");
	db.Exec("create table data( index integer, value text)");
	char sql[128] = {0};
	for (int i = 0; i < 100; i++) {
		sprintf(sql, "insert into data(index, value) values(%d,\'%s\')",i, "beidou");
		db.Exec(sql);
	}
	PGRecordset* res = db.Query("select * from data");
	if (res != NULL) {
		printf("has %d rows", res->GetTupleCount());
		printf("has %d col", res->GetFieldCount());
		printf(" %s\n",res->GetValue(0,0));
		printf(" %s\n",res->GetValue(0,1));
		res->Destroy();
	}


	db.Commit();
	db.DisConnect();
	
	return 1;
}