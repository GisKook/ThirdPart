#ifndef UDPRECEIVER_H_H
#define UDPRECEIVER_H_H
int udprecv_createsocket(const char* peerip, int peerport, int port);
int udprecv_createepoll();
void udprecv_add(int efd, int fd);
#endif
