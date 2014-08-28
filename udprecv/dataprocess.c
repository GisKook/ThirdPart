#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <zmq.h>
#include "list.h"
#include "dataprocess.h"
#include "PGRecordset.h"
#include "PGDatabase.h"
#include "CNConfig.h"
#include "CNDef.h"
#include "toolkit.h"
#include "protocolresolve.h"
#include "beidoumessage.pb.h"

using namespace std;

int bexitok = -1;
unsigned int totalforward_udpmsg = 0;
unsigned int totalsaved_udpmsg = 0;
struct list_head* head = NULL;
struct param{
	PGDatabase db;
	void* zmq_ctx;
	void* zmq_socket;
	int fd; 
}*p;

struct packet {
	unsigned char* data;
	unsigned int len;
	struct beidouinfo fmtinfo;
	struct list_head list;
	unsigned char saved;
	unsigned char forward;
	unsigned char parsed;
};

//void settime(unsigned char* buf, unsigned char year, unsigned char month, unsigned char day, unsigned char hour, unsigned char minute, unsigned char second){
//	// buf format timebuf[32] = {'T','I','M','E','S','T','A','M','P',' ','\'','2','0'};
//	buf[13] = year / 10;
//	buf[14] = year % 10;
//	buf[15] = '-';
//	buf[16] = month / 10;
//	buf[17] = month % 10;
//	buf[18] = '-';
//	buf[19] = day / 10;
//	buf[20] = day % 10;
//	buf[21] = ' ';
//	buf[22] = hour / 10;
//	buf[23] = hour % 10;
//	buf[24] = '-';
//	buf[25] = minute / 10;
//	buf[26] = minute % 10;
//	buf[27] = '-';
//	buf[28] = second / 10;
//	buf[29] = second % 10;
//	buf[30] = '\'';
//}

int getmilliondegree(unsigned char degree, unsigned char minute, unsigned char second, unsigned char tenthssecond){
	return degree*1000000+minute*1000000/60+second*1000000/3600+tenthssecond*1000000/36000;
}


#define MAXBUFLEN 8
void* forwardmsg(void* rd){
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	int fd = *((int*)rd);
	int len = 0;
	char buf[MAXBUFLEN];
	struct list_head* pos = NULL;
	struct list_head* n = NULL;
	struct packet* temp;
	struct datasegment* tempsegment;
	int i;
	Beidoumessage beidoumessage;
	Communication* communication = beidoumessage.mutable_commuincation();
	Communicationreceipt* communicationreceipt = beidoumessage.mutable_communicationreceipt();
	Positioninfo* positioninfo = beidoumessage.mutable_positioninfo();

	string str;
	for(;;){
		len = read(fd, buf, MAXBUFLEN);
		list_for_each_safe(pos, n, head){
			temp = list_entry(pos, struct packet, list); 
			if(temp->forward !=1){ 
				for(i = 0; i<temp->fmtinfo.curdataentrycount; ++i){
					tempsegment = temp->fmtinfo.recvdatasegment;
					beidoumessage.set_messagetype(tempsegment->messagecategory);
					std::string().swap(str);
					if( tempsegment->messagecategory == 0){ 
						positioninfo->set_userid(tempsegment->message.posinfo->userid);
						positioninfo->set_encryption(tempsegment->message.posinfo->encryption);
						positioninfo->set_accuracy(tempsegment->message.posinfo->accuracy);
						positioninfo->set_emergencypostion(tempsegment->message.posinfo->emergencypostion);
						positioninfo->set_multivaluesolution(tempsegment->message.posinfo->multivaluesolution);
						positioninfo->set_key(tempsegment->message.posinfo->key, 6);
						positioninfo->set_longitude_degree(tempsegment->message.posinfo->longitudedegree);
						positioninfo->set_longitude_minute(tempsegment->message.posinfo->longitudeminute);
						positioninfo->set_longitude_second(tempsegment->message.posinfo->longitudesecond);
						positioninfo->set_longitude_tenths(tempsegment->message.posinfo->longitudetenths);
						positioninfo->set_latitude_degree(tempsegment->message.posinfo->latitudedegree);
						positioninfo->set_latitude_minute(tempsegment->message.posinfo->latitudeminute);
						positioninfo->set_latitude_second(tempsegment->message.posinfo->latitudesecond);
						positioninfo->set_latitude_tenths(tempsegment->message.posinfo->latitudetenths);
						positioninfo->set_geodeticheight(tempsegment->message.posinfo->geodeticheight);
						positioninfo->set_detlaelevation(tempsegment->message.posinfo->detlaelevation); 
					}else if( tempsegment->messagecategory == 3){ 
						communication->set_messageform(tempsegment->message.cominfo->messageform);
						communication->set_messagecategory(tempsegment->message.cominfo->messagecategory);
						communication->set_encryption(tempsegment->message.cominfo->encryption);
						communication->set_sendaddr(tempsegment->message.cominfo->sendaddr);
						communication->set_recvaddr(tempsegment->message.cominfo->recvaddr);
						communication->set_sendtime_hour(tempsegment->message.cominfo->sendtime.hour);
						communication->set_sendtime_minute(tempsegment->message.cominfo->sendtime.minutes);
						communication->set_sendtime_second(tempsegment->message.cominfo->sendtime.seconds);
						communication->set_messagelength(tempsegment->message.cominfo->messagebytelength);
						communication->set_key(tempsegment->message.cominfo->key, 6);
						communication->set_messagebuffer(tempsegment->message.cominfo->messagebuffer, tempsegment->message.cominfo->messagebytelength);
					}else if(tempsegment->messagecategory == 4){
						communicationreceipt->set_sendaddr(tempsegment->message.rcptinfo->sendaddr);
						communicationreceipt->set_recvaddr(tempsegment->message.rcptinfo->recvaddr);
						communicationreceipt->set_receipttime_hour(tempsegment->message.rcptinfo->receipttime.hour);
						communicationreceipt->set_receipttime_minute(tempsegment->message.rcptinfo->receipttime.minutes);
						communicationreceipt->set_receipttime_second(tempsegment->message.rcptinfo->receipttime.seconds);
					}else{
						fprintf(stderr, "no such protocol %d\n", tempsegment->messagecategory);
					}
					beidoumessage.SerializeToString(&str);
					positioninfo->Clear();
					communication->Clear();
					communicationreceipt->Clear();
					if(!str.empty()){ 
						zmq_msg_t msg;
						int rc = zmq_msg_init_size(&msg, str.length());
						assert(rc == 0);
						memcpy(zmq_msg_data(&msg), str.c_str(), str.length()); 

						rc = zmq_msg_send(&msg, p->zmq_socket, 0);
						assert(rc != -1); 
						zmq_msg_close(&msg);
						temp->forward = 1;
						++totalforward_udpmsg;
					}

				}    
				assert(temp->fmtinfo.retrydataentrycount == 0);
			}
//			if(temp->saved == 1 && temp->forward == 1){ 
//				if(temp->data != NULL){ 
//					free(temp->data);
//					temp->data = NULL;
//				}
//				clearbeidouinfo(&temp->fmtinfo);
//				list_del(&temp->list);
//				if(temp != NULL){
//					free(temp);
//					temp = NULL;
//				}
//			}
		}
		if(buf[0] == 'E'){
			fprintf(stdout, "    forward beidou data thread exit successfuly.\n");
			beidoumessage.Clear();
			if(p->zmq_socket != NULL){
				zmq_close(p->zmq_socket);
				p->zmq_socket = NULL;
			}
			zmq_ctx_destroy(p->zmq_ctx);
			p->zmq_ctx = NULL;
			google::protobuf::ShutdownProtobufLibrary();
			pthread_exit(0);
		}

	}

}
#define MAXSQLLEN 512
#define MAXBYTELEN 256
void* savemsg(void* rd){
	int fd = *((int*)rd);
	char buf[MAXBUFLEN];
	int len = 0;
	struct list_head* pos = NULL;
	struct list_head* n = NULL;

	char sqlbuf[MAXSQLLEN];
	char bytebuf[MAXBYTELEN];
	int index = 0;
	int i;
	struct packet* temp;
	struct datasegment* tempdata;
	for(;;){ 
		len = read(fd, buf, MAXBUFLEN);
		list_for_each_safe(pos, n, head){ 
			temp = list_entry(pos,struct packet, list);
			if(temp->saved != 1){
				memset(bytebuf,0,MAXBYTELEN); 
				index = ISBIGENDIAN?(*(int*)(temp->data + 2)):swab32((*(int*)(temp->data + 2)));
				hex2char(bytebuf, temp->data, temp->len);
				if(!p->db.BeginTransaction()){
					continue;
				}
				memset(sqlbuf,0,MAXSQLLEN);
				sprintf(sqlbuf, "insert into brs_udp_data(recv_time,recv_index,recv_data) VALUES(now(),%d,E\'\\\\x%s')",index,bytebuf);
				if(!p->db.Exec(sqlbuf)){
					fprintf(stderr, "%s insert error! \n",sqlbuf);
				}
				for(i = 0; i<temp->fmtinfo.curdataentrycount; ++i){
					memset(sqlbuf,0,MAXSQLLEN);
					tempdata = temp->fmtinfo.recvdatasegment;
					if( tempdata->messagecategory == 0){
						int longititude = getmilliondegree(tempdata->message.posinfo->longitudedegree, tempdata->message.posinfo->longitudeminute, tempdata->message.posinfo->longitudesecond, tempdata->message.posinfo->longitudetenths);
						int latitude = getmilliondegree(tempdata->message.posinfo->latitudedegree, tempdata->message.posinfo->latitudeminute, tempdata->message.posinfo->latitudesecond, tempdata->message.posinfo->latitudetenths);
						sprintf(sqlbuf, "insert into brs_dwxx(card, recv_time, pos_long, pos_lat, height, heightex, encrypt, accuracy, urgent, multi, time_hour, time_minute, time_second, time_centi) values(%d,now(),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)" ,tempdata->message.posinfo->userid,longititude, latitude, tempdata->message.posinfo->geodeticheight, tempdata->message.posinfo->detlaelevation, tempdata->message.posinfo->encryption, tempdata->message.posinfo->accuracy, tempdata->message.posinfo->emergencypostion ,tempdata->message.posinfo->multivaluesolution, tempdata->message.posinfo->applytime.hour, tempdata->message.posinfo->applytime.minutes, tempdata->message.posinfo->applytime.seconds, tempdata->message.posinfo->applytime.tenms); 
					}else if(tempdata->messagecategory == 3){ 
						memset(bytebuf, 0 , MAXBYTELEN);
						hex2char(bytebuf, tempdata->message.cominfo->messagebuffer, tempdata->message.cominfo->messagebytelength);
						sprintf(sqlbuf, "insert into brs_txxx(recv_time, sender, recver, time_hour, time_minute, time_second, msg_form, msg_type, msg_encrypt, msg_len, msg) values(now(), %d, %d, %d, %d, %d, %d, %d, %d, %d, E\'\\\\x%s')",tempdata->message.cominfo->sendaddr, tempdata->message.cominfo->recvaddr, tempdata->message.cominfo->sendtime.hour, tempdata->message.cominfo->sendtime.minutes, tempdata->message.cominfo->sendtime.seconds, tempdata->message.cominfo->messageform, tempdata->message.cominfo->messagecategory, tempdata->message.cominfo->encryption, tempdata->message.cominfo->messagelength, bytebuf); 
					}else if(tempdata->messagecategory == 4){
						sprintf(sqlbuf, "insert into brs_txhz(recv_time, sender, recver, time_hour, time_minute, time_second) values(now(), %d, %d, %d, %d, %d)", tempdata->message.rcptinfo->sendaddr, tempdata->message.rcptinfo->recvaddr, tempdata->message.rcptinfo->receipttime.hour, tempdata->message.rcptinfo->receipttime.minutes, tempdata->message.rcptinfo->receipttime.seconds);
					}else{
						fprintf(stderr, "no such protocol %d\n", tempdata->messagecategory);
					}

					if(!p->db.Exec(sqlbuf)){
						fprintf(stderr, "%s insert error! \n",sqlbuf);
					}
				}    
				assert(temp->fmtinfo.retrydataentrycount == 0);

				if(!p->db.Commit()){
					continue;
				}else{
					temp->saved = 1;
					++totalsaved_udpmsg;
				}	
			}
			if(temp->forward == 1 && temp->saved == 1){ 
				if(temp->data != NULL){
					free(temp->data);
					temp->data = NULL;
				}
				clearbeidouinfo(&temp->fmtinfo);
				list_del(&temp->list);
				if(temp != NULL){
					free(temp);
					temp = NULL;
				}
			}
		}
		if(buf[0] == 'E'){
			p->db.DisConnect(); 
			fprintf(stdout, "     save beidou data thread exit successfully.\n");
			pthread_exit(0);
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

	int fmfd[2];
	if(pipe(fmfd) == -1){
		fprintf(stderr, "create pipe failed\n");
	}
	pthread_t tid_forward;
	pthread_create(&tid_forward, NULL, forwardmsg, (void*)&fmfd[0]);

	char buf[MAXBUFLEN];
	int len = 0;
	struct list_head* pos = NULL;
	struct list_head* n = NULL;
	for(;;){ 
		memset(buf, 0, MAXBUFLEN);
		len = read(fd, buf, MAXBUFLEN);
		if(buf[0] == 'E'){
			fprintf(stdout, "data process module prepare to exit...\n");
			write(rwfd[1],"E",1);
			pthread_join(tid_save, NULL);
			write(fmfd[1], "E",1);
			pthread_join(tid_forward, NULL);
			fprintf(stdout, "data process module exit successfully.\n");
			close(rwfd[0]);
			close(rwfd[1]);
			close(fmfd[0]);
			close(fmfd[1]);
			bexitok = 0;
			pthread_exit(0);
		}

		list_for_each_safe(pos, n,head){
			if(list_entry(pos, struct packet, list)->parsed == 0){
				if( resolvebeidouinfo(&(list_entry(pos, struct packet, list)->fmtinfo),list_entry(pos,struct packet, list)->data) == -1){
					fprintf(stderr, "parse error: %s %d \n", __FILE__, __LINE__);
				}else{
					list_entry(pos, struct packet,list)->parsed = 1;	
				}
			}
		}
		write(rwfd[1],"1",1);
		write(fmfd[1],"1",1);
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

	p->zmq_ctx = zmq_ctx_new();
	if(p->zmq_ctx == NULL){
		fprintf(stderr, "zmq create ctx fail.\n");
	}

	p->zmq_socket = zmq_socket(p->zmq_ctx, ZMQ_PUSH);
	if(p->zmq_socket == NULL){
		fprintf(stderr, "zmq create socket fail.\n");
	}
	const char* forwardport = CNConfig::GetInstance().GetValue(FORWARDPORT);
	char forwardaddr[16] = {0};
	sprintf(forwardaddr, "tcp://*:%s", forwardport);
	int rc = zmq_bind(p->zmq_socket, forwardaddr);
	if(rc != 0){
		fprintf(stderr, "downstream error, zmq socket bind port fail. errno is %d %s %d\n", errno, __FILE__, __LINE__);
	}

}

void dataprocess_clear(){
	if(p != NULL){ 
		free((void*)head);
		head = NULL;
		free((void*)p);
		p = NULL;
	}
}

int dataprocess_exitok(){
	return bexitok;
}
