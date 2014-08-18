/*
 * brief: 读取配置文件 配置文件使用json格式
 * function list:
 * 
 * author: a staff of CETCNAV
 * date: 2014年8月5日
 */
#ifndef CNCONFIG_H_H
#define CNCONFIG_H_H

#include <string>
#include <map>
#include "CNDef.h"
#define PEERIP "PeerIP"
#define PEERPORT "PeerPort"
#define BINDPORT "BindPort"

class CNConfig{
public:
	static CNConfig& GetInstance();
	
	bool LoadFile(const char*);

	const char* GetValue(const char*);

private:
	CNConfig(){};
	~CNConfig(){};
	CNConfig(const CNConfig&){};
	CNConfig& operator=(const CNConfig&){};

private:
	std::map<std::string, std::string> m_mapConfig;
};

#endif
