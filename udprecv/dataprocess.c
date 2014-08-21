#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "list.h"
#include "dataprocess.h"

struct list_head* head = NULL;
int* rfd = NULL;

struct packet {
	unsigned char* data;
	unsigned int len;
	struct list_head list;
};

void* parsemsg(void* rd){
	int fd = *(int*)rd;

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
	}

	pthread_exit(0);
}

void dataprocess_push( unsigned char* buf, unsigned int len ) {
	if(head  == NULL || buf == NULL){
		return;
	}
	struct packet* p = (struct packet*)malloc(sizeof(struct packet)); 
	p->data = (unsigned char*)malloc(len);
	memcpy((void*)p->data, buf, len);
	p->len = len;
	list_add_tail(&p->list, head);
}

void dataprocess_init(int fd){ 
	head = (struct list_head*)malloc(sizeof(struct list_head));
	INIT_LIST_HEAD(head);
	rfd = (int*)malloc(sizeof(int));
	*rfd = fd;
	pthread_t tid;
	pthread_create(&tid, NULL,parsemsg,(void*)rfd);
}

void dataprocess_clear(){
	free((void*)head);
	free((void*)rfd);
}
