#ifndef DATAPROCESS_H_H
#define DATAPROCESS_H_H

void dataprocess_push(unsigned char* buf, unsigned int len);
void dataprocess_init(int fd);
void dataprocess_clear();
#endif
