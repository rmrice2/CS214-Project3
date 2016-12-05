#ifndef LIBNETFILES_H_
#define LIBNETFILES_H_

#include <sys/types.h>

int netopen(const char *, int);
ssize_t netread(int, void *, size_t);
ssize_t netwrite(int, const void *, size_t);
int netclose(int);
int netserverinit(char *, int);

void handle_open(char *);
void handle_read(char *);
void handle_write(char *);
void handle_close(char *);

#endif