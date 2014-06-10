#include "DatabasePool.h"
#include <assert.h>


DatabasePool::DatabasePool():
m_nPoolsize(0),
m_pHashTable(NULL)
{
	memset(&m_ConnInfo, 0, sizeof(OraConnInfo));
}

void DatabasePool::Create( OraConnInfo& connInfo, int size ) {
	memcpy(&m_ConnInfo, &connInfo, sizeof(OraConnInfo));
	m_nPoolsize = size;
	m_pHashTable = new list_head[size]; 
	for (int i = 0; i<size; i++) {
	//	m_pHashTable[i].list = new list_head;
		INIT_LIST_HEAD(&m_pHashTable[i]);
	//	m_pHashTable[i].db=NULL;
	//	m_pHashTable[i].isUsed = false;
	//	m_pHashTable[i].nThreadID= 0;

	}
}

void DatabasePool::Destroy() {
	if (m_pHashTable!=NULL) { 
		list_head *pos;
		list_head *n;
		dbConn *pConn;
		for (int i = 0; i < m_nPoolsize; i++) {
			list_for_each_safe(pos,n, &m_pHashTable[i])
			{ 
				pConn = list_entry(&m_pHashTable[i], dbConn, list);
				assert(pConn->isUsed == false);
	//			pConn->db->DisConnect();
		//		pConn->db->Destroy();
				pConn->db=NULL;
				list_del(pConn->list);

				delete pConn->list;
				pConn->list = NULL;
			}
		} 
		delete []m_pHashTable;
		m_pHashTable=NULL;
	} 
}

OraDatabase* DatabasePool::GetConnection( int nThreadID ) {
	int nHashkey = GetHashCode(nThreadID); 
	dbConn *pConn = NULL;
	if (list_empty(&m_pHashTable[nHashkey])) {
		pConn = new dbConn;
		pConn->db = new OraDatabase;
		pConn->list = new list_head;
	//	pConn->db->Init();
		//pConn->db->Connect(m_ConnInfo);
		pConn->isUsed=true;
		pConn->nThreadID=nThreadID;
		list_add_tail(pConn->list, &m_pHashTable[nHashkey]);
		return pConn->db;
	}else{
		list_head *pos;
		list_head *n;
		list_for_each_safe(pos, n, &m_pHashTable[nHashkey]){ 
			pConn = container_of(pos, dbConn, list);
			if (pConn->nThreadID == nThreadID && pConn->isUsed) {
				return pConn->db;
			}
		}
		list_for_each_safe(pos, n, &m_pHashTable[nHashkey]){ 
			pConn = container_of(pos, dbConn, list);
			if (!pConn->isUsed){
				return pConn->db;
			}
		}

		pConn = new dbConn;
		pConn->db = new OraDatabase;
		pConn->list = new list_head;
	//	pConn->db->Init();
		//pConn->db->Connect(m_ConnInfo);
		pConn->isUsed=true;
		pConn->nThreadID=nThreadID;
		list_add_tail(pConn->list, &m_pHashTable[nHashkey]);
		return pConn->db;
	}

	return NULL;
}

void DatabasePool::PutbackConnection( int nThreadID ) {
	int nHashkey=GetHashCode(nThreadID); 
	assert(!list_empty(&m_pHashTable[nHashkey]));
	dbConn *pConn = NULL;
	list_head *pos, *n;
	list_for_each_safe(pos, n, &m_pHashTable[nHashkey]){
		pConn = list_entry(pos, dbConn, list);
		if (pConn->nThreadID == nThreadID) { 
			assert(pConn->isUsed == true);
			pConn->isUsed=false;
		}
	}
}

void DatabasePool::DeleteConnection(int nThreadID) { 
	int nHashkey=GetHashCode(nThreadID); 
	assert(!list_empty(&m_pHashTable[nHashkey]));
	dbConn *pConn = NULL;
	list_head *pos, *n;
	list_for_each_safe(pos, n, &m_pHashTable[nHashkey]){
		pConn = container_of(pos, dbConn, list);
		if (pConn->nThreadID == nThreadID) { 
	//		pConn->db->DisConnect();
		//	pConn->db->Destroy();
			pConn->db=NULL;
			list_del(pConn->list);
			if (pConn->list!=NULL) {
				delete pConn->list;
				pConn->list = NULL;
			}
		}
	}
}