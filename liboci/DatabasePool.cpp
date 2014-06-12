#include <windows.h>
#include "DatabasePool.h"
#include <assert.h>

volatile long uID=0;

DatabasePool::DatabasePool():
m_nPoolsize(0),
m_pHashTable(NULL)
{
	memset(&m_ConnInfo, 0, sizeof(OraConnInfo));
}

void DatabasePool::Create( OraConnInfo& connInfo, int size ) {
	m_ConnInfo.dbName = strdup(connInfo.dbName);
	m_ConnInfo.host = strdup(connInfo.host);
	m_ConnInfo.login = strdup(connInfo.login);
	m_ConnInfo.passwd = strdup(connInfo.passwd);
	m_ConnInfo.port = connInfo.port;
	m_nPoolsize = size;
	m_pHashTable = new list_head[size]; 
	for (int i = 0; i<size; i++) {
		INIT_LIST_HEAD(&m_pHashTable[i]);
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
				pConn = list_entry(pos, dbConn, list);
				assert(pConn->isUsed == false);
				pConn->db->DisConnect();
				pConn->db->Destroy();
				delete pConn->db;
				pConn->db=NULL;
				list_del(&pConn->list);

				delete pConn;
				pConn = NULL;
			}
		} 
		delete []m_pHashTable;
		m_pHashTable=NULL;
		free(m_ConnInfo.dbName);
		free(m_ConnInfo.host);
		free(m_ConnInfo.login);
		free(m_ConnInfo.passwd);
	} 
}

dbConn* DatabasePool::GetConnection( int nThreadID ) {
	int nHashkey = GetHashCode(nThreadID); 
	dbConn *pConn = NULL;
	if (list_empty(&m_pHashTable[nHashkey])) {
		printf("|");
		pConn = new dbConn;
		pConn->db = new OraDatabase;
		pConn->db->Init();

		if (CN_SUCCESS != pConn->db->Connect(m_ConnInfo)) {
			pConn->db->Destroy();
			delete pConn->db;
			delete pConn;
			pConn = NULL;
			return NULL;
		}
		pConn->isUsed=true;
		pConn->ID=InterlockedExchangeAdd(&uID, 1);
		list_add_tail(&pConn->list, &m_pHashTable[nHashkey]);
		return pConn;
	}else{
		list_head *pos;
		list_head *n;
		list_for_each_safe(pos, n, &m_pHashTable[nHashkey]){ 
			pConn = list_entry(pos, dbConn, list);
			if (!pConn->isUsed){
				printf("-");
				pConn->isUsed = true;
				if(pConn->db->IsConnectionValid()){ 
					return pConn;
				} else{
					printf("e");
					DeleteConnection(nThreadID, pConn);
				}
			}
		}

		pConn = new dbConn;
		pConn->db = new OraDatabase;
		pConn->db->Init();
		printf("+");
		if (CN_SUCCESS != pConn->db->Connect(m_ConnInfo)) {
			pConn->db->Destroy();
			delete pConn->db;
			delete pConn;
			pConn = NULL;
			return NULL;
		}
		pConn->isUsed=true;
		pConn->ID=InterlockedExchangeAdd(&uID, 1);

		list_add_tail(&pConn->list, &m_pHashTable[nHashkey]);
		return pConn;
	}

	return NULL;
}

void DatabasePool::PutbackConnection(int nThreadID, dbConn* conn) {
	int nHashkey=GetHashCode(nThreadID); 
	assert(!list_empty(&m_pHashTable[nHashkey]));
	dbConn *pConn = NULL;
	dbConn *pConn2=NULL;
	list_head *pos, *n;
	list_for_each_safe(pos, n, &m_pHashTable[nHashkey]){
		pConn = list_entry(pos, dbConn, list);
		if (pConn->ID == conn->ID) { 
			assert(pConn->isUsed == true);
			pConn->isUsed=false;
			return;
		}
	}
}

void DatabasePool::DeleteConnection(int nThreadID, dbConn* conn) { 
	int nHashkey=GetHashCode(nThreadID); 
	assert(!list_empty(&m_pHashTable[nHashkey]));
	dbConn *pConn = NULL;
	list_head *pos, *n;
	list_for_each_safe(pos, n, &m_pHashTable[nHashkey]){
		pConn = list_entry(pos, dbConn, list);
		if (pConn->ID==conn->ID) { 
			pConn->db->DisConnect();
			pConn->db->Destroy();
			delete pConn->db;
			pConn->db=NULL;
			list_del(&pConn->list);
			delete pConn;
			pConn = NULL;
		}
	}
}