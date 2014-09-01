#ifndef DATAPROCESS_H_H
#define DATAPROCESS_H_H

void dataprocess_push(unsigned char* buf, unsigned int len);
int dataprocess_init(int fd);
void dataprocess_clear();
int dataprocess_listempty();
void dataprocess_print_list();
int dataprocess_exitok();
#endif
