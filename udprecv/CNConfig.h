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

struct cJSON;

#define NAME "Name"
#define PEERIP "PeerIP"
#define PEERPORT "PeerPort"
#define BINDPORT "BindPort"
#define DBHOST "dbHost"
#define DBPORT "dbPort"
#define DBNAME "dbName"
#define DBUSER "dbUser"
#define DBPWD "dbPassword"
#define FORWARDIP "ForwardIP"
#define FORWARDPORT "ForwardPort"
#define STOREDBFREQUENCY "Storedbfrequency"
#define STOREDBINTERVEL "Storedbinterval"

class CNConfig{
public:
	static CNConfig& GetInstance();
	
	bool LoadFile(const char*);

	const char* GetValue(const char*);

private:
	bool setconfig(cJSON* json, const char* configkey, const char* errormsg);

private:
	CNConfig(){};
	~CNConfig(){};
	CNConfig(const CNConfig&){};

private:
	std::map<std::string, std::string> m_mapConfig;
};

#endif
