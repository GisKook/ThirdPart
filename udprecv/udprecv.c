#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int main(){
	int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(udp_socket == -1){
	//	handle_error("socket");
		fprintf(stderr, "socket error");
	}
	struct sockaddr_in client_addr;
	
	memset((void*)&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//client_addr.sin_addr.s_addr = inet_addr("192.168.1.155");
	client_addr.sin_port = htons(20001);
	if(bind(udp_socket,(struct sockaddr*)&client_addr, sizeof(struct sockaddr_in)) == -1){
		fprintf(stderr, "bind error");
	} 

	struct sockaddr_in server_addr;
	memset((void*)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_addr.s_addr = inet_addr("192.168.1.115");
	server_addr.sin_port = htons(20000);
	if(connect(udp_socket,(struct sockaddr*)&server_addr, sizeof(struct sockaddr_in)) == -1){
		fprintf(stderr, "connect error");
	} 
	
	const int MAXLEN = 1024;
	char buf[MAXLEN+1];
	char buf2[MAXLEN+1];
	
	struct msghdr msg;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	struct iovec io;
	io.iov_base = buf;
	io.iov_len = MAXLEN;
	msg.msg_iov = &io;
	msg.msg_iovlen = 1;
	ssize_t len;
	for(;;){
		memset(buf,0,sizeof(buf));
		len = recvmsg(udp_socket, &msg, 0);
		if(msg.msg_flags & MSG_TRUNC){
			printf("trunc\n");
			
		} 
		printf("%s\n", buf);
	}

}
