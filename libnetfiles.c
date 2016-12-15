#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "libnetfiles.h"

int server_socket;

int file_mode_selected;

/*This method takes a pathname and a permissions flag and
returns a file descriptor or -1 if an error occured

flags: O_RDONLY, O_WRONLY, or O_RDWR
	
required: EACCES, EINTR, EISDIR, ENOENT, EROFS
optional: ENFILE, EWOULDBLOCK, EPERM*/
	
int netopen(const char * pathname, int flags){
	
	char buffer[500];
	Int_packet response;

	strcpy(buffer, "o");
	
	if(flags == O_RDONLY){
		strcat(buffer, "r");
	}
	else if(flags == O_WRONLY){
		strcat(buffer, "w");
	}
	else if(flags == O_RDWR){
		strcat(buffer, "b");
	}
	else{
		printf("Error: Invalid flag.");
		return -1;
	}

	//extension A : write file mode

	switch(file_mode_selected){
		case UNRESTRICTED:
			strcat(buffer,"0");
			break;
		case EXCLUSIVE:
			strcat(buffer,"1");
			break;
		case TRANSACTION:
			strcat(buffer,"2");
	}

	strcat(buffer, pathname);
	strcat(buffer, "\0");

	send(server_socket, buffer, strlen(buffer), 0);

	recv(server_socket, &response, sizeof(response), 0);

	if(response.type == 'e'){
		errno = response.i;
		perror("Error");
		return -1;
	}
	else if(response.type == 'r'){
		return response.i;
	}
	else{
		return -1;
	}
}

//return the number of bytes read
//if error, set errno and return -1

//required: ETIMEDOUT, EBADF, ECONNRESET
ssize_t netread(int filedes, void *buf, size_t nbyte){

	ssize_t bytes_read;
	Int_packet message;
	char * response = malloc(nbyte + sizeof(char) + sizeof(ssize_t));
	message.type = 'r';
	message.i = filedes;
	message.size = nbyte;

	send(server_socket, &message, sizeof(message), 0);

	recv(server_socket, response, nbyte + 9, 0);

	if(response[0] == 'e'){
		//errno = response->i;
		free(response);
		return -1;
	}
	else if(response[0] == 'r'){

		memcpy(&bytes_read, response + 1, sizeof(ssize_t));
		memcpy(buf, response + 9, bytes_read);
		
		free(response);
		return bytes_read;
	}

	return -1;
}

//return number of bytes written (must be less than nbyte)
//if error, set errno and return -1

//required: EBADF, ETIMEOUT, ECONNRESET
ssize_t netwrite(int filedes, const void *buf, size_t nbyte){
	/* packet structure:
	|char|int|size_t|char array|  */
	
	size_t message_len = nbyte + 1 + sizeof(int) + sizeof(size_t);
	char * message = malloc(message_len);
	Int_packet response;

	message[0] = 'w';
	memcpy(message + 1, &filedes, sizeof(int));
	memcpy(message + 1 + sizeof(int), &nbyte, sizeof(nbyte));
	memcpy(message + 1 + sizeof(int) + sizeof(size_t), buf, nbyte);

	send(server_socket, message, message_len, 0);
	free(message);

	recv(server_socket, &response, sizeof(response), 0);

	if(response.type == 'e'){
		errno = response.i;
		return -1;
	}

	return response.size;
}

/* This method takes a file descriptor, connects to the server
and attempts to close the remote file. If close fails, returns
-1, otherwise returns 0

required: EBADF
*/
int netclose(int fd){
	Int_packet message, response;
	message.type = 'c';
	message.i = fd;

	send(server_socket, &message, sizeof(message), 0);

	recv(server_socket, &response, sizeof(response), 0);

	if(response.type == 'e'){
		errno = response.i;
		perror("Close error");
		return -1;
	}

	return 0;
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

	file_mode_selected = filemode;//set file mode
	return 0;
}

