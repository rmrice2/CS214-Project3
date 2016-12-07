#ifndef LIBNETFILES_H_
#define LIBNETFILES_H_

#include <sys/types.h>

/*types:
c - netclose
e - error
f - file descriptor
*/

typedef struct int_packet{
	char type;
	int i;
	size_t size;
} Int_packet;

int netopen(const char *, int);
ssize_t netread(int, void *, size_t);
ssize_t netwrite(int, const void *, size_t);
int netclose(int);
int netserverinit(char *, int);

void * serve_client(void *);
void handle_open(int, char *);
void handle_read(int, char *);
void handle_write(int, char *);
void handle_close(int, char *);

#endif