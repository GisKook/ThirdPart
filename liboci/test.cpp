#include <stdio.h>
#include <vector>
#include <windows.h>

#include "OraDatabase.h"
#include "oradb_monitor.h"
using namespace std;

void (my_oradb_monitor_callback)(struct oradb_monitor_result * omr){ 
	printf("db    name is : %s\n", omr->dbname);
	printf("table name is : %s\n", omr->tablename);
	printf("opcode     is : %d\n", omr->opcode);
	printf("rowid      is : %s\n", omr->rowid);
}

int main(){
	OraConnInfo conn;
 	conn.host="192.168.2.31";
 	conn.port=1521;
 	conn.dbName="axttest";
 	conn.login="dkpps2";
 	conn.passwd="dkpps";
//	conn.host="192.168.1.166";
//	conn.port=1521;
//	conn.dbName="TEST";
//	conn.login="zhangkai";
//	conn.passwd="zhangkai";
	OraDatabase db;
	db.Init();
	if (!db.IsConnectionValid()) { printf("not connect\n");
	}else{printf("connect\n");}
	
	db.Connect(conn);

	struct oradb_monitor* odbm = oradb_monitor_create(&conn);
	oradb_monitor_register(odbm);
	CNVARIANT valm;
	valm.eDataType = ORAINT;
	oradb_monitor_monitor(odbm, "select id from test_table21", &valm, 1);
	CNVARIANT valm2;
	valm2.eDataType = ORATEXT;
	oradb_monitor_monitor(odbm, "select name from test_table21", &valm2, 1);
	oradb_monitor_setcallback(odbm, my_oradb_monitor_callback);

	OraConnInfo* temp=db.GetConnInfo();

	if (!db.IsConnectionValid()) { printf("not connect\n");
	}else{printf("connect\n");}

	//db.ExecSQL("drop table TEST_TABLE22");
	//db.ExecSQL("CREATE TABLE TEST_TABLE21(NAME VARCHAR2(50 CHAR), id INT, tt float )");
	db.ExecSQL("INSERT INTO TEST_TABLE21 VALUES('id1', 1, 4.4)");
	//db.ExecSQL("commit");
	db.ExecSQL("update test_table21 set name='   gggg' where id=1");
	//db.ExecSQL("commit");
	Sleep(60000);
	CNVARIANT val[3];
	val[0].eDataType=ORATEXT;
	val[1].eDataType=ORAINT;
	val[2].eDataType=ORAFLOAT;
	
	int retval = db.Query("select NAME, id, tt from TEST_TABLE21 where id=1 ",val,3);
	int a=db.GetColCount();
	retval = db.Fetch();
	while(retval == CN_SUCCESS){
		printf("get data from db :%s,  %d,  %f\n", val[0].pValue, val[1].iValue, val[2].fValue);
		retval = db.Fetch();
	}
	db.ExecSQL("delete from TEST_TABLE21 where id = 1");
//	db.ExecSQL("commit");
	//db.ExecSQL("commit");
	//db.ExecSQL("delete from TEST_TABLE21 where id = 1");
	Sleep(1000);
	if (!db.IsConnectionValid()) { printf("not connect\n");
	}else{printf("connect\n");}

	
	db.DisConnect();
	if (!db.IsConnectionValid()) { printf("not connect\n");
	}else{printf("connect\n");}
	db.Destroy();
	system("pause");
	return 0;
}