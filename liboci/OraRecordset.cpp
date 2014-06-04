#include <assert.h>
#include "oci.h" // 头文件包含顺序有影响 mark
#include "OraRecordset.h"
#include "OraDatabase.h"
#include <stdio.h>

OraRecordset::OraRecordset() :
m_pStmt(NULL)
{

}

int OraRecordset::Create(OraDatabase* db) {
	sword swRetval = OCIHandleAlloc(db->GetEnv(),(void**)&m_pStmt, OCI_HTYPE_STMT, 0, NULL); assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);

	return 0;
}

int OraRecordset::Destroy() {
	sword swRetval = OCIHandleFree(m_pStmt, OCI_HTYPE_STMT); assert(swRetval==OCI_SUCCESS || swRetval==OCI_SUCCESS_WITH_INFO);	
	return 0;
}

int OraRecordset::GetValue( CNVARIANT &value,int i/*=0*/, int j/*=0*/ ) { 

}
