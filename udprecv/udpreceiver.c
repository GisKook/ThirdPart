#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <errno.h>


int main(){
	int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(udp_socket == -1){
		//	handle_error("socket");
		fprintf(stderr, "socket error\n");
	}
	struct sockaddr_in client_addr;

	memset((void*)&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//client_addr.sin_addr.s_addr = inet_addr("192.168.1.155");
	client_addr.sin_port = htons(20001);
	if(bind(udp_socket,(struct sockaddr*)&client_addr, sizeof(struct sockaddr_in)) == -1){
		fprintf(stderr, "bind error\n");
	} 

	struct sockaddr_in server_addr;
	memset((void*)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_addr.s_addr = inet_addr("192.168.1.115");
	server_addr.sin_port = htons(20000);
	if(connect(udp_socket,(struct sockaddr*)&server_addr, sizeof(struct sockaddr_in)) == -1){
		fprintf(stderr, "connect error\n");
	} 

	int flag = 0;
	flag = fcntl(udp_socket, F_GETFD, 0);

	if(-1==fcntl(udp_socket,F_SETFD, flag | O_NONBLOCK)){
		fprintf(stderr, "fcntl error\n");
	}
	flag = fcntl(STDIN_FILENO, F_GETFD, 0);
	if(-1==fcntl(STDIN_FILENO, F_SETFD,flag | O_NONBLOCK)){
		fprintf(stderr, "stdin nonblock error\n");
	}

	int efd = epoll_create1(EPOLL_CLOEXEC);
	if( efd == -1){
		fprintf(stderr, "epoll_crate error\n");
	}
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = udp_socket;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, udp_socket, &ev) == -1) {
		fprintf(stderr, "add fd error\n");
	}

	struct epoll_event evstdin;
	evstdin.events = EPOLLIN | EPOLLET;
	evstdin.data.fd = STDIN_FILENO;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, STDIN_FILENO, &evstdin) == -1) {
		fprintf(stderr, "add fd error\n");
	}

	int n = 0,i=0;
	struct epoll_event ev1[4];
	char buf[10240]={0};

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
				printf("recv ");
				memset(buf, 0, 10240);
				errno = 0;

				len = read(ev1[i].data.fd,buf,10240);
				printf(" %s \n", buf);
				if(errno != 0){
					printf("errno is %d \n", errno);
				}
			}


		}
	}
}
