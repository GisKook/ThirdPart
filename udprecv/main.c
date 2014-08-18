#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "udpreceiver.h"
#include "kfifo.h"
#include "CNConfig.h"

#define MAXBUFLEN 1024

int main(){
	CNConfig::GetInstance().LoadFile("./json.conf");
	const char* peerip = CNConfig::GetInstance().GetValue(PEERIP);
	const char* peerport = CNConfig::GetInstance().GetValue(PEERPORT);
	int npeerport = atoi(peerport);
	const char* bindport = CNConfig::GetInstance().GetValue(BINDPORT);
	int nbindport = atoi(bindport);

	int udp_socket = udprecv_createsocket(peerip, npeerport, nbindport);
	if( udp_socket == -1 ){
		return -1;
	}
	
	int efd = udprecv_createepoll();
	if( efd == -1 ){
		return -1;
	}
	udprecv_add(efd, udp_socket);
	udprecv_add(efd, STDIN_FILENO);

	int n = 0,i=0;
	struct epoll_event ev[4];
	char buf[MAXBUFLEN]={0};

	ssize_t len;
	for(;;){
		n = epoll_wait(efd, ev, 4, -1);

		for(i = 0; i < n; i++){
			uint32_t flags = ev[i].events;
			if( (flags & EPOLLERR) || 
					(flags & EPOLLHUP) ||
					!(flags & EPOLLIN)){
				fprintf(stderr, "epoll error!\n");
				close(ev[i].data.fd); 
				continue;
			}else{
				memset(buf, 0, MAXBUFLEN);
				errno = 0;
				len = read(ev[i].data.fd,buf,MAXBUFLEN);
				printf("%s\n", buf);
				if(errno != 0){
					printf("errno is %d \n", errno);
				}
			}
		}
	}
	return 0;
}
