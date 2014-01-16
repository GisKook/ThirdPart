/*
 * brief: ��װlibcurl�⣬�ṩ��httpЭ�������
 * function list:
 * 
 * author: zhangkai
 * date: 2014��1��14��
 */
#ifndef CNCURL_H_H
#define CNCURL_H_H

#include <string>

typedef void CURL;

class CNCurl{
public:

struct CNCurlResult{
	unsigned char* pData;
	unsigned int nSize;
	unsigned int nCapacity;
}m_dataCurl;

public:
 	CNCurl();
 	~CNCurl();
 	// brief ����url��ַ
 	// param[in] Ҫ���õĲ���
 	void SetURL(std::string strUrl);
 
 	// brief �õ�url��ַ
 	// return URL��ַ
 	std::string GetURL() const;
 
 	// brief �õ����ص�xml
 	std::string GetResult();

	// brief �ڶ���ʹ��ǰҪ�����ݶ����
	void CleanUp(); 

private:
	// brief call back 
	static size_t WriteData(void *pBuf, size_t nSize, size_t nMemb, void *userp);
	
 
 private:
 	// brief libcurl ָ��
 	CURL* m_pCurl;
 
 	// brief url��ַ
 	std::string m_strURL;
};

#endif