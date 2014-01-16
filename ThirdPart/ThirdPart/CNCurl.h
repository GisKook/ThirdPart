/*
 * brief: 封装libcurl库，提供对http协议的请求
 * function list:
 * 
 * author: zhangkai
 * date: 2014年1月14日
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
 	// brief 设置url地址
 	// param[in] 要设置的参数
 	void SetURL(std::string strUrl);
 
 	// brief 得到url地址
 	// return URL地址
 	std::string GetURL() const;
 
 	// brief 得到返回的xml
 	std::string GetResult();

	// brief 第二次使用前要把数据都清空
	void CleanUp(); 

private:
	// brief call back 
	static size_t WriteData(void *pBuf, size_t nSize, size_t nMemb, void *userp);
	
 
 private:
 	// brief libcurl 指针
 	CURL* m_pCurl;
 
 	// brief url地址
 	std::string m_strURL;
};

#endif