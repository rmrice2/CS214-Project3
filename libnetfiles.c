#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "libnetfiles.h"

int server_socket;

int netopen(const char * pathname, int flags){
	//flags: O_RDONLY, O_WRONLY, or O_RDWR

	int filedes = -1;
	
	char buffer[500];
	strcpy(buffer, "o");
	strcat(buffer, "rw");
	strcat(buffer, pathname);
	strcat(buffer, "\0");

	send(server_socket, buffer, strlen(buffer), 0);

	//recv(server_socket, buffer, strlen(buffer), 0);

	//return file descriptor
	//if error, set errno and return -1
	
	//required: EACCES, EINTR, EISDIR, ENOENT, EROFS
	//optional: ENFILE, EWOULDBLOCK, EPERM
	return filedes;
}

ssize_t netread(int fildes, void *buf, size_t nbyte){

	//return the number of bytes read
	//if error, set errno and return -1

	//required: ETIMEDOUT, EBADF, ECONNRESET

	return NULL;
}

ssize_t netwrite(int filedes, const void *buf, size_t nbyte){

	//return number of bytes written (must be less than nbyte)
	//if error, set errno and return -1

	//required: EBADF, ETIMEOUT, ECONNRESET

	return NULL;
}

int netclose(int fd){
	
	//return 0 on success, -1 on error

	//required: EBADF

	return -1;
}

int netserverinit(char * hostname, int filemode){

	int clientfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	rv = getaddrinfo(hostname, "12345", &hints, &servinfo);

	if(rv != 0){
		perror("getaddrinfo");
		return -1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next){

		//try to create a socket with the server info
		clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		if(clientfd == -1){
			perror("socket");
		}
		else if (connect(clientfd, p->ai_addr, p->ai_addrlen) == -1){
			perror("bind");
			close(clientfd);
		}

		//if no errors found, leave the loop. otherwise, try another server info
		else{
			break;
		}
	}

	if(p == NULL){
		perror("Error");
		//todo: set h_error
		return -1;
	}

	freeaddrinfo(servinfo);

	printf("Server found.\n");
	server_socket = clientfd;
	return 0;
}

