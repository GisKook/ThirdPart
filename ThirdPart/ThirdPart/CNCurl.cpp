#include "CNCurl.h"
#include <stdio.h>
#include <curl/curl.h>

using namespace std;


CNCurl::CNCurl()
{ 
	m_dataCurl.pData = (unsigned char*)malloc(1);
	m_dataCurl.nSize = 0;
	m_dataCurl.nCapacity = 0;
	curl_global_init(CURL_GLOBAL_ALL);
	m_pCurl = curl_easy_init();
}

CNCurl::~CNCurl()
{
	if (m_dataCurl.pData) {
		free(m_dataCurl.pData);
	}
	curl_easy_cleanup(m_pCurl);
	curl_global_cleanup();
}

void CNCurl::SetURL( std::string strUrl )
{ 
	m_strURL = strUrl;
}

std::string CNCurl::GetURL() const
{ 
	return m_strURL;
}

void CNCurl::CleanUp()
{ 
	memset((void*)m_dataCurl.pData, 0, m_dataCurl.nCapacity);
	m_dataCurl.nSize = 0;
}
std::string CNCurl::GetResult()
{ 
	if (m_pCurl!=NULL) {
	    curl_easy_setopt(m_pCurl, CURLOPT_URL, m_strURL.c_str());
		curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, WriteData);
		curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void*)&m_dataCurl);
		CURLcode res = curl_easy_perform(m_pCurl);
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
	}
	return string((char*)m_dataCurl.pData);
}

size_t CNCurl::WriteData( void *pBuf, size_t nSize, size_t nMemb, void *userp)
{ 
	size_t nRealsize = nSize*nMemb;
	struct CNCurlResult *pResult = (struct CNCurlResult*)userp;
	
	if (pResult->nCapacity<pResult->nSize + nRealsize + 1) {
		pResult->pData = (unsigned char*)realloc(pResult->pData, pResult->nSize + nRealsize + 1);
	}
	if(pResult->pData == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}
	
	memcpy(&(pResult->pData[pResult->nSize]), pBuf, nRealsize);
	pResult->nSize += nRealsize;
	pResult->nSize > pResult->nCapacity?pResult->nCapacity=pResult->nSize:pResult->nCapacity=pResult->nCapacity;
	pResult->pData[pResult->nSize]=0;
	
	return nRealsize;
}