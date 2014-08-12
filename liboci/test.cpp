#include <stdio.h>
#include <vector>

using namespace std;
#include "OraDatabase.h"

int main(){
	OraConnInfo conn;
	conn.dbName="";
	

	conn.host="192.168.2.31";
	conn.port=1521;
	conn.dbName="axttest";
	conn.login="dkpps2";
	conn.passwd="dkpps";
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
	db.ExecSQL("INSERT INTO TEST_TABLE22 VALUES('ÖÐµç¿Æ', 1, 4.4)");
		
	CNVARIANT val[3];
	val[0].eDataType=ORATEXT;
	val[1].eDataType=ORAINT;
	val[2].eDataType=ORAFLOAT;
	
	int retval = db.Query("select NAME, id, tt from TEST_TABLE22 where id=1 ",val,3);
	int a=db.GetColCount();
	retval = db.Fetch();
	while(retval == CN_SUCCESS){
		printf("get data from db :%s,  %d,  %f\n", val[0].pValue, val[1].iValue, val[2].fValue);
		retval = db.Fetch();
	}
	if (!db.IsConnectionValid()) { printf("not connect\n");
	}else{printf("connect\n");}

	
	db.DisConnect();
	if (!db.IsConnectionValid()) { printf("not connect\n");
	}else{printf("connect\n");}
	db.Destroy();
	system("pause");
	return 0;
}