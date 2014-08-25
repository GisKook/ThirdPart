#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "list.h"
#include "dataprocess.h"
#include "PGRecordset.h"
#include "PGDatabase.h"
#include "CNConfig.h"
#include "CNDef.h"
#include "toolkit.h"

struct list_head* head = NULL;
struct param{
	PGDatabase db;
	int fd; 
}*p;

struct packet {
	unsigned char* data;
	unsigned int len;
	struct list_head list;
	unsigned char saved;
	unsigned char format;
};

void* savemsg(void* rd){
	int fd = *((int*)rd);
	const int MAXBUFLEN = 8;
	char buf[MAXBUFLEN];
	int len = 0;
	struct list_head* pos = NULL;
	struct list_head* n = NULL;
	char sqlbuf[256];
	char bytebuf[256];
	int len_format = 0;
	int index = 0;
	for(;;){ 
		memset(sqlbuf,0,256);
		len = read(fd, buf, MAXBUFLEN);
		list_for_each_safe(pos, n, head){ 
			if(list_entry(pos,struct packet, list)->saved != 1){
				memset(bytebuf,0,256); 
				index = ISBIGENDIAN?(*(int*)(list_entry(pos, struct packet, list)->data + 2)):swab32((*(int*)(list_entry(pos, struct packet, list)->data + 2)));
				hex2char(bytebuf, list_entry(pos,struct packet, list)->data, list_entry(pos, struct packet, list)->len);
				sprintf(sqlbuf, "insert into \"BRS_UDP_DATA\"(\"TIME\",\"INDEX\",\"DATA\") VALUES(now(),%d,E\'\\\\x%s')",index,bytebuf);
				if(!p->db.Exec(sqlbuf)){
					fprintf(stderr, "%s insert error! \n",buf);
				}else{
					list_entry(pos, struct packet, list)->saved = 1;
				}
			}
		}
	}

}

void* parsemsg(void* rd){
	int fd = ((struct param*)rd)->fd;

	int rwfd[2];
	if(pipe(rwfd) == -1){
		fprintf(stderr, "create pipe failed\n");
	}
	pthread_t tid_save;
	pthread_create(&tid_save, NULL, savemsg, (void*)&rwfd[0]);
	const int MAXBUFLEN = 8;
	char buf[MAXBUFLEN];
	int len = 0;
	struct list_head* pos = NULL;
	struct list_head* n = NULL;
	for(;;){ 
		memset(buf, 0, MAXBUFLEN);
		len = read(fd, buf, MAXBUFLEN);
		list_for_each_safe(pos, n,head){
			printf("%s\n",((struct packet*)container_of(pos, struct packet, list))->data);
		}
		write(rwfd[1],"1",1);
	}

	pthread_exit(0);
}


void dataprocess_push( unsigned char* buf, unsigned int len ) {
	if(head  == NULL || buf == NULL){
		return;
	}
	struct packet* p = (struct packet*)malloc(sizeof(struct packet)); 
	memset(p, 0, sizeof(struct packet));
	p->data = (unsigned char*)malloc(len);
	memcpy((void*)p->data, buf, len);

	p->len = len;
	list_add_tail(&p->list, head);
}

void dataprocess_init(int fd){ 
	const char* dbhost = CNConfig::GetInstance().GetValue(DBHOST);
	const char* dbport = CNConfig::GetInstance().GetValue(DBPORT);
	int ndbport = atoi(dbport);
	const char* dbname = CNConfig::GetInstance().GetValue(DBNAME);
	const char* dbuser = CNConfig::GetInstance().GetValue(DBUSER);
	const char* dbpwd = CNConfig::GetInstance().GetValue(DBPWD);
	PGConnInfo conn;
	conn.pghost = (char*)dbhost;
	conn.pgport = (char*)dbport;
	conn.dbName = (char*)dbname;
	conn.login = (char*)dbuser;
	conn.passwd = (char*)dbpwd; 
	p = (struct param*)malloc(sizeof(struct param));
	if( 0 != p->db.Connect(conn)){
		return;
	}

	head = (struct list_head*)malloc(sizeof(struct list_head));
	INIT_LIST_HEAD(head);
	p->fd = fd;
	pthread_t tid;
	pthread_create(&tid, NULL,parsemsg,(void*)p);
}

void dataprocess_clear(){
	free((void*)head);
	head = NULL;
	free((void*)p);
}
