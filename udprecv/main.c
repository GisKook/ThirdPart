#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "udpreceiver.h"
#include "CNConfig.h"
#include "dataprocess.h"

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

	int fd[2];
	if( pipe(fd) == -1){
		fprintf(stderr, "create pipe fail\n");
		close(udp_socket);
		return -1;
	}
	dataprocess_init(fd[0]);

	int efd = udprecv_createepoll();
	if( efd == -1 ){
		close(udp_socket);
		close(fd[0]);
		close(fd[1]);
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
				if(ev[i].data.fd == STDIN_FILENO){
					if(buf[0] == 'q'&&
							buf[1] == 'u'&&
							buf[2] == 'i'&&
							buf[3] == 't'){
						fprintf(stdout, "stop receive beidou data.\n");
						write(fd[1], "E", 1);
						if( -1 == epoll_ctl(efd, EPOLL_CTL_DEL, udp_socket, NULL)){
							fprintf(stderr, "del socket from epoll fail.\n");
						}
						if( -1 == epoll_ctl(efd, EPOLL_CTL_DEL, STDIN_FILENO, NULL)){
							fprintf(stderr, "del socket from epoll fail.\n");
						}

						close(udp_socket);
						close(fd[0]);
						close(fd[1]);
						close(efd);
						sleep(1);
						goto exit;
					}
					printf("dd\n");
				}
				if(ev[i].data.fd == udp_socket){
					dataprocess_push((unsigned char*)buf, len);
					write(fd[1], "0", 1);
				}
				if(errno != 0){
					printf("errno is %d \n", errno);
				}
			}
		}
	}
exit:
	return 0;
}
