#include <stdio.h>
#include <vector>

using namespace std;
#include "OraDatabase.h"

int main(){
	OraConnInfo conn;
	conn.dbName="";
	

	conn.host="192.168.1.166";
	conn.port=1521;
	conn.dbName="test";
	conn.login="zhangkai";
	conn.passwd="zhangkai";
	OraDatabase db;
	db.Init();
	if (!db.IsConnectionValid()) { printf("not connect\n");
	}else{printf("connect\n");}
	
	db.Connect(conn);

	OraConnInfo* temp=db.GetConnInfo();

	if (!db.IsConnectionValid()) { printf("not connect\n");
	}else{printf("connect\n");}

	db.ExecSQL("drop table TEST_TABLE22");
	db.ExecSQL("CREATE TABLE TEST_TABLE22(NAME VARCHAR2(50 CHAR), id INT, tt float )");
	db.ExecSQL("INSERT INTO TEST_TABLE22 VALUES('zhangkaiaaa', 1, 4.4)");
	db.ExecSQL("INSERT INTO TEST_TABLE22 VALUES('zhangkai',2,33.3 )");
	db.ExecSQL("INSERT INTO TEST_TABLE22 VALUES('zhangkaidddd',3,35.5)");
	
	vector<CNVARIANT> vVal; 
	vVal.push_back(CNVARIANT(ORATEXT));
	vVal.push_back(CNVARIANT(ORAINT));
	vVal.push_back(CNVARIANT(ORAFLOAT));
	int retval = db.Query("select * from TEST_TABLE22 where name='zhangkai' ",vVal);
	int a=db.GetColCount();
	retval = db.Fetch();
	while(retval == CN_SUCCESS){
		printf("get data from db :%s,  %d,  %f\n", vVal[0].pValue, vVal[1].iValue, vVal[2].fValue);
		retval = db.Fetch();
	}
	if (!db.IsConnectionValid()) { printf("not connect\n");
	}else{printf("connect\n");}
	db.DisConnect();
	if (!db.IsConnectionValid()) { printf("not connect\n");
	}else{printf("connect\n");}
	db.Destroy();
	return 0;
}