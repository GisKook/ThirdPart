all:
	g++ -g -Wl,-rpath -Wl,/usr/local/lib -Wall -o ./recvapp -I /usr/pgsql-9.3/include/  -I./pb main.c CNConfig.cpp cJSON.c PGDatabase.cpp PGRecordset.cpp protocolresolve.c dataprocess.c udpreceiver.c toolkit.c ./pb/beidoumessage.pb.cc -L /usr/pgsql-9.3/lib/ -lpq -lm -pthread -lprotobuf -lzmq

recv:
	rm -rf recv
	g++ -g -Wl,-rpath -Wl,/usr/local/lib -Wall  -o ./recv -I./pb zmq_recv.c ./pb/beidoumessage.pb.cc -lprotobuf -lzmq
	
