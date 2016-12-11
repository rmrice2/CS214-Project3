#ifndef LIBNETFILES_H_
#define LIBNETFILES_H_

#include <sys/types.h>
#include <sys/socket.h>
/*types:
c - netclose
e - error
o - open
r - read
w - write
*/

typedef struct int_packet{
	char type;
	int i;
	size_t size;
} Int_packet;

typedef struct client{
	int socket;
	struct sockaddr_in * addr;
	socklen_t addr_size;
} Client;

int netopen(const char *, int);
ssize_t netread(int, void *, size_t);
ssize_t netwrite(int, const void *, size_t);
int netclose(int);
int netserverinit(char *, int);

void * serve_client(void *);
void handle_open(int, char *);
void handle_read(Client, char *);
void handle_write(Client, char *);
void handle_close(int, char *);
int check_socks();

#endif