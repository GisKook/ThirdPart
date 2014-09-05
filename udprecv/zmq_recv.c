#include <assert.h>
#include <zmq.h>
#include <string.h>
#include "beidoumessage.pb.h"

using namespace std;

int main(){
	void* ctx = zmq_ctx_new();
	assert(ctx != NULL);
	void* socket = zmq_socket(ctx, ZMQ_PULL);
	assert(ctx != NULL);
	int rc = zmq_bind(socket, "tcp://*:40000");
	assert(rc == 0);

	zmq_msg_t msg;
	rc = zmq_msg_init(&msg);
	assert(rc == 0);
	Beidoumessage bdmsg;
	char buf[512];
	string str;
	int ncount = 0;
	
	assert(rc != -1); 
	for(;;){
		memset(buf, 0, 512);
		rc = zmq_msg_recv(&msg, socket, 0);
		str = string((char*)zmq_msg_data(&msg), zmq_msg_size(&msg));
		bdmsg.ParseFromString(str);
		if(bdmsg.messagetype() == 0){
			
//		printf("--%d\n",bdmsg.mutable_positioninfo()->userid());
//		printf("--%d\n",bdmsg.mutable_positioninfo()->encryption());
//		printf("--%d\n",bdmsg.mutable_positioninfo()->accuracy());
//		printf("--%d\n",bdmsg.mutable_positioninfo()->emergencypostion());
//		printf("--%d\n",bdmsg.mutable_positioninfo()->multivaluesolution());
//		printf("--%d\n",bdmsg.mutable_positioninfo()->longitude_degree());
//		printf("--%d\n",bdmsg.mutable_positioninfo()->longitude_minute());
//		printf("--%d\n",bdmsg.mutable_positioninfo()->longitude_second());
//		printf("--%d\n",bdmsg.mutable_positioninfo()->longitude_tenths());
//		printf("--%d\n",bdmsg.mutable_positioninfo()->latitude_degree());
//		printf("--%d\n",bdmsg.mutable_positioninfo()->latitude_minute());
//		printf("--%d\n",bdmsg.mutable_positioninfo()->latitude_second());
//		printf("--%d\n",bdmsg.mutable_positioninfo()->latitude_tenths());
//		printf("--%d\n",bdmsg.mutable_positioninfo()->geodeticheight());
//		printf("--%d\n",bdmsg.mutable_positioninfo()->detlaelevation());
		printf("%d\n", ncount++);
		}
		bdmsg.Clear();

		assert(rc != -1);
	}
	zmq_msg_close(&msg);

	return 0;

}
