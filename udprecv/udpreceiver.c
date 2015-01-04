#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define MAXBUFLEN 1024

int udprecv_createsocket(const char* peerip, int peerport, int port){
	int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(udp_socket == -1){
		fprintf(stderr, "socket error\n");
		return -1;
	}
	struct sockaddr_in client_addr;

	memset((void*)&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	client_addr.sin_port = htons(port);
	if(bind(udp_socket,(struct sockaddr*)&client_addr, sizeof(struct sockaddr_in)) == -1){
		fprintf(stderr, "bind error\n");
	} 

	struct sockaddr_in server_addr;
	memset((void*)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(peerip);
	server_addr.sin_port = htonl(INADDR_ANY);//htons(peerport);
	if(connect(udp_socket,(struct sockaddr*)&server_addr, sizeof(struct sockaddr_in)) == -1){
		fprintf(stderr, "connect error\n");
	} 

	return udp_socket;
}

void udprecv_setnonblock(int fd){
	if(-1==fcntl(fd ,F_SETFD, fcntl(fd, F_GETFD, 0) | O_NONBLOCK)){
		fprintf(stderr, "fcntl error\n");
	}
}

int udprecv_createepoll(){
	int efd = epoll_create1(EPOLL_CLOEXEC);

	if( efd == -1){
		fprintf(stderr, "epoll_crate error\n");
	}

	return efd;
}

void udprecv_add(int efd, int fd){
	struct epoll_event ev;
	ev.events = EPOLLIN; //| EPOLLET;
	ev.data.fd = fd;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		fprintf(stderr, "add fd error\n");
	}

}

typedef struct _udprecv_message{
	int fd;
	char buf[MAXBUFLEN];
}udprecv_message;

void udprecv_epoll(int efd, udprecv_message* msg){
	int n = 0,i=0;
	struct epoll_event ev1[4];
	char buf[MAXBUFLEN]={0};

	ssize_t len;
	for(;;){
		n = epoll_wait(efd, ev1, 4, -1);

		for(i = 0; i < n; i++){
			uint32_t flags = ev1[i].events;
			if( (flags & EPOLLERR) || 
					(flags & EPOLLHUP) ||
					!(flags & EPOLLIN)){
				fprintf(stderr, "epoll error!\n");
				close(ev1[i].data.fd); 
				continue;
			}else{
				memset(buf, 0, MAXBUFLEN);
				errno = 0;
				len = read(ev1[i].data.fd,buf,MAXBUFLEN);
				if(len < 0){
					close(ev1[i].data.fd);
					fprintf(stderr, "error: %s %s %s %d\n", strerror(errno), __FILE__, __FUNCTION__, __LINE__);
				}
		//		printf("recv %s\n", buf);
				memcpy(msg->buf, buf, MAXBUFLEN);
				if(errno != 0){
					printf("errno is %d \n", errno);
				}
				return;
			}
		}
	}
}

