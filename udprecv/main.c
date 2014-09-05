#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <pthread.h>
#include <time.h>
#include "udpreceiver.h"
#include "CNConfig.h"
#include "dataprocess.h"
#include "toolkit.h"
#include "CNDef.h"

unsigned int totalreceived_udpmsg = 0;
extern unsigned int totalforward_udpmsg;
extern unsigned int totalsaved_udpmsg;

#define MAXBUFLEN 1024
#define STARTTIME 1
#define PRINT 2
#define STOP 3
#define CLEAR 4
#define DETAIL 5
#define LISTEMPTY 6
#define CONFIG 7
int getcmd(char* buf){
	// 1 start time  -- 
	// 2 print  --received beidou data
	// 3 stop --stop print received beidou data
	if(memcmp(buf, "start time", 9) == 0){
		return STARTTIME;
	}else if(memcmp(buf, "print",5) == 0){
		return PRINT;
	}else if(memcmp(buf, "stop", 4) == 0){
		return STOP;
	}else if(memcmp(buf, "clear", 5) == 0){
		return CLEAR;
	}else if(memcmp(buf, "detail", 6) == 0){
		return DETAIL;
	}else if(memcmp(buf, "list", 4) == 0){
		return LISTEMPTY;
	}else if(memcmp(buf, "config", 6) == 0){
		return CONFIG;
	}else{
		fprintf(stdout, "-------------------------------------\n");
		fprintf(stdout, "usage:\n");
		fprintf(stdout, "    1. start time\n");
		fprintf(stdout, "    2. print -- print received beidou data\n");
		fprintf(stdout, "    3. stop -- stop print\n");
		fprintf(stdout, "    4. quit\n");
		fprintf(stdout, "    5. clear -- clear screen\n");
		fprintf(stdout, "    6. detail -- received udp saved udp forward udp\n");
		fprintf(stdout, "    7. list -- is dataprocess ok\n");
		fprintf(stdout, "    8. config -- show configures\n");
		fprintf(stdout, "-------------------------------------\n");
	}
	return 0;
}

int main(){
	time_t t;
	t = time(NULL);
	char* starttime = ctime(&t);
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
	pthread_t tid = dataprocess_init(fd[0]);

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

	int bprint = 0;
	int brecv = 1;
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
						brecv = 0;
						fprintf(stdout, "stop receive beidou data.\n");
						write(fd[1], "E", 1);
						if( -1 == epoll_ctl(efd, EPOLL_CTL_DEL, udp_socket, NULL)){
							fprintf(stderr, "del socket from epoll fail.\n");
						}
						if( -1 == epoll_ctl(efd, EPOLL_CTL_DEL, STDIN_FILENO, NULL)){
							fprintf(stderr, "del socket from epoll fail.\n");
						}

						pthread_join(tid, NULL);
						close(udp_socket);
						close(fd[0]);
						close(fd[1]);
						close(efd);
						
						dataprocess_clear();

						goto exit;
					}else{
						int cmd = getcmd(buf);
						switch(cmd){
							case STARTTIME:
								fprintf(stdout, starttime);
								break;
							case CLEAR: 
								fprintf(stdout, "\033[2J\033[1;1H\n");
								break;
							case PRINT:
								bprint = 1;
								break;
							case STOP:
								bprint = 0;
								break;
							case DETAIL:
								fprintf(stdout, "total received udp message count: %d  --- ", totalreceived_udpmsg);
								fprintf(stdout, "total forward udp message count: %d  --- ", totalforward_udpmsg);
								fprintf(stdout, "total saved udp message count: %d \n", totalsaved_udpmsg);
								break;
							case LISTEMPTY:
								if(dataprocess_listempty() == 0){
									fprintf(stdout, "still have data to process.\n");
									dataprocess_print_list();
									
								}else{
									fprintf(stdout, "no data to process.\n");
								}
								break;
							case CONFIG:
								fprintf(stdout, "-------------------------------------\n");
								fprintf(stdout, "|PeerIP          : %s\n", CNConfig::GetInstance().GetValue(PEERIP));
								fprintf(stdout, "|BindPort        : %s\n", CNConfig::GetInstance().GetValue(BINDPORT));
								fprintf(stdout, "|dbHost          : %s\n", CNConfig::GetInstance().GetValue(DBHOST));
								fprintf(stdout, "|dbPort          : %s\n", CNConfig::GetInstance().GetValue(DBPORT));
								fprintf(stdout, "|dbName          : %s\n", CNConfig::GetInstance().GetValue(DBNAME));
								fprintf(stdout, "|dbUser          : %s\n", CNConfig::GetInstance().GetValue(DBUSER));
								fprintf(stdout, "|dbPassword      : %s\n", CNConfig::GetInstance().GetValue(DBPWD));
								fprintf(stdout, "|ForwardPort     : %s\n", CNConfig::GetInstance().GetValue(FORWARDPORT));
								fprintf(stdout, "|Storedbfrequency: %s\n", CNConfig::GetInstance().GetValue(STOREDBFREQUENCY));
								fprintf(stdout, "|Storedbinterval : %s\n", CNConfig::GetInstance().GetValue(STOREDBINTERVEL));
								fprintf(stdout, "-------------------------------------\n");
								break;
							break;
							default:
								break;

						}
					}
				}
				if(ev[i].data.fd == udp_socket && brecv == 1){
					dataprocess_push((unsigned char*)buf, len);
					++totalreceived_udpmsg;
					if(bprint == 1){ 
						int	index = ISBIGENDIAN?(*(int*)(buf+2)):swab32((*(int*)(buf + 2)));
						fprintf(stdout, "index :%d  ", index);
						debug_printbytes((unsigned char*)buf, len);
					}
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
