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
	int rc = zmq_connect(socket, "tcp://192.168.1.155:40000");
	assert(rc == 0);

	zmq_msg_t msg;
	rc = zmq_msg_init(&msg);
	assert(rc == 0);
	Beidoumessage bdmsg;
	char buf[512];
	string str;
	for(;;){
		memset(buf, 0, 512);
		rc = zmq_msg_recv(&msg, socket, 0);
		str = string((char*)zmq_msg_data(&msg), zmq_msg_size(&msg));
		bdmsg.ParseFromString(str);
		printf("%d\n", bdmsg.messagetype());
		if(bdmsg.messagetype() == 0){
			
		printf("--%d\n",bdmsg.mutable_postioninfo()->userid());
		printf("--%d\n",bdmsg.mutable_postioninfo()->encryption());
		printf("--%d\n",bdmsg.mutable_postioninfo()->accuracy());
		printf("--%d\n",bdmsg.mutable_postioninfo()->emergencypostion());
		printf("--%d\n",bdmsg.mutable_postioninfo()->multivaluesolution());
		printf("--%d\n",bdmsg.mutable_postioninfo()->longtitude_degree());
		printf("--%d\n",bdmsg.mutable_postioninfo()->longtitude_minute());
		printf("--%d\n",bdmsg.mutable_postioninfo()->longtitude_second());
		printf("--%d\n",bdmsg.mutable_postioninfo()->longtitude_tenths());
		printf("--%d\n",bdmsg.mutable_postioninfo()->latitude_degree());
		printf("--%d\n",bdmsg.mutable_postioninfo()->latitude_minute());
		printf("--%d\n",bdmsg.mutable_postioninfo()->latitude_second());
		printf("--%d\n",bdmsg.mutable_postioninfo()->latitude_tenths());
		printf("--%d\n",bdmsg.mutable_postioninfo()->geodeticheight());
		printf("--%d\n",bdmsg.mutable_postioninfo()->detlaelevation());

		}

		assert(rc != -1);
	}
	zmq_msg_close(&msg);

	return 0;

}
