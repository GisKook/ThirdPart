#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "cJSON.h"
#include "CNConfig.h"

using namespace std;

bool CNConfig::setconfig(cJSON* json, const char* configkey, const char* errormsg){
	cJSON* val = cJSON_GetObjectItem(json, configkey);
	if(!val||val->type!=cJSON_String||!val->valuestring) {
		fprintf(stderr,errormsg);
		cJSON_Delete(json);
		return false;
	}else{ 
		m_mapConfig[configkey]=string(val->valuestring);
	}
	return true;
}

CNConfig& CNConfig::GetInstance()
{
	static CNConfig config;
	return config;
}

bool CNConfig::LoadFile( const char* filename)
{
	FILE *f=fopen(filename,"rb");
	if (f==NULL) {
		fprintf(stderr,"there is no config file! %s \n", filename);
		return false;
	}
	fseek(f,0,SEEK_END);long len=ftell(f);fseek(f,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,f);fclose(f);
	cJSON* json = cJSON_Parse(data);
	if (!json) {
		fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
	}else{ 
		int nCount = cJSON_GetArraySize(json);
		assert(nCount==10);
		if( !setconfig(json, PEERIP, "please check config file: ListenIP is not set!\n")||
	//		!setconfig(json, PEERPORT, "please check config file: ListenPort is not set!\n")||
			!setconfig(json, BINDPORT, "please check config file: BindPort is not set!\n")||
			!setconfig(json, DBHOST, "please check config file: dbHost is not set!\n")||
			!setconfig(json, DBPORT, "please check config file: dbPort is not set!\n")||
			!setconfig(json, DBNAME, "please check config file: dbName is not set!\n")||
			!setconfig(json, DBUSER, "please check config file: dbUser is not set!\n")||
			!setconfig(json, DBPWD, "please check config file: dbPassword is not set!\n")||
			!setconfig(json, FORWARDPORT, "please check config file: ForwardPort is not set!\n")||
			!setconfig(json, STOREDBFREQUENCY, "please check config file: Storedbfrequency is not set!\n")||
			!setconfig(json, STOREDBINTERVEL, "please check config file: Storedbinterval is not set!\n")){
			
		return false;
		}

	}
	cJSON_Delete(json);

	free(data);
	return true;
}

const char* CNConfig::GetValue( const char* key)
{
	return m_mapConfig[string(const_cast<char*>(key))].c_str();
}
