#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "cJSON.h"
#include "CNConfig.h"

using namespace std;

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
		assert(nCount==3);
		cJSON* val = cJSON_GetObjectItem(json, PEERIP);
		if(!val||val->type!=cJSON_String||!val->valuestring) {
			fprintf(stderr,"please check config file: ListenIP is not set!\n");
			cJSON_Delete(json);
			return false;
		}else{ 
			m_mapConfig[PEERIP]=string(val->valuestring);
		}

		val = cJSON_GetObjectItem(json, PEERPORT);
		if(!val||val->type!=cJSON_String||!val->valuestring) {
			fprintf(stderr,"please check config file: ListenPort is not set!\n");
			cJSON_Delete(json);
			return false;
		}else{ 
			m_mapConfig[PEERPORT]=string(val->valuestring);
		}

		val = cJSON_GetObjectItem(json, BINDPORT);
		if(!val||val->type!=cJSON_String||!val->valuestring) {
			fprintf(stderr,"please check config file: BindPort is not set!\n");
			cJSON_Delete(json);
			return false;
		}else{ 
			m_mapConfig[BINDPORT]=string(val->valuestring);
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
