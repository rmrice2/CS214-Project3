#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include "libnetfiles.h"

int main(int argc, char * argv[]){
 
    //char * hostname = argv[1];
    char * portname = "12345";
    
    int serverfd,  //server socket
        clientfd;  //client socket
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPV4
    hints.ai_socktype = SOCK_STREAM;  //for UDP use SOCK_DGRAM
    hints.ai_flags = AI_PASSIVE; // use my IP address

    if ((rv = getaddrinfo(NULL, portname, &hints, &servinfo)) != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {

        //create an IPV4 TCP socket
        serverfd = socket(p->ai_family, 
                          p->ai_socktype,
                          p->ai_protocol);

        if (serverfd == -1) {
            perror("socket");
        }

        else if (bind(serverfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(serverfd);
            perror("bind");
        }

        else{
            break;
        }
    }

    //in this case, we didn't find any working addr on the list
    if (p == NULL) {
        printf("failed to bind socket\n");
        exit(2);
    }

    freeaddrinfo(servinfo); // all done with this structure

    //start listening on the port, allow 5 pending connections
    if(listen(serverfd, 10) < 0){
            perror("Error on listen");
            exit(0);
    }

    //loop while waiting for connections
    while(1) {
        clientfd = accept(serverfd, NULL, NULL);

        printf("Connected to a client.\n");

        pthread_t * thread = malloc(sizeof(pthread_t *));
        
        pthread_create(thread, NULL, serve_client, (void *)&clientfd);

    }
    return 0;
}

void * serve_client(void * ptr){

    int clientfd = *(int *)ptr;
    char buffer[500];

    while(1){
        if((recv(clientfd, buffer, sizeof(buffer) , 0)) == -1){
            perror("Error");
            exit(1);
        }
        else{
            char selector = buffer[0];

            if(selector == 'o'){
                handle_open(clientfd, buffer);
            }
            else if(selector == 'r'){
                handle_read(clientfd, buffer);
            }
            else if(selector == 'w'){
                handle_write(clientfd, buffer);
            }
            else if(selector == 'c'){
                handle_close(clientfd, buffer);
            }
            else{
                printf("Error: invalid message format.\n");
            }
        }
    }

    return NULL;

}
/*this method accepts a client socket and a buffer, 
extracts the filename within, attempts to open the file, 
and sends a char array to the client containing either the 
file descriptor or error info*/
void handle_open(int clientfd, char * buffer){
    char filename[500];
    int len;
    Int_packet response;
    
    len = strlen(buffer);
    printf("string len: %d\n", len);
    
    sprintf(filename, "%.*s", len - 4, buffer + 3);

    response.i = open(filename, O_RDWR);

    if(response.i < 0){
        perror("File open error");
        response.type = 'e';
        response.i = errno;

        send(clientfd, &response, sizeof(response), 0);

        return;
    }

    response.type = 'f';
    printf("Opened file: %s\n", filename);

    send(clientfd, &response, sizeof(response), 0);
}

void handle_read(int clientfd, char * buffer){
    Int_packet * message;
    char * response;
    int filedes;
    size_t nbytes;
    ssize_t bytes_read;

    message = (Int_packet *)buffer;
    filedes = message->i;
    nbytes = message->size;
    
    response = malloc(nbytes + 9);
    response[0] = 'r';

    bytes_read = read(filedes, response + 9, nbytes);

    if(bytes_read == -1){
        Int_packet err;
        err.type = 'e';
        err.i = errno;

        perror("Error");
        send(clientfd, &err, sizeof(err), 0);
    }

    else{
        printf("Read %ld bytes from file.\n", bytes_read);
        memcpy(response + 1, &bytes_read, sizeof(bytes_read));
        send(clientfd, response, bytes_read + 9, 0);
    }
}

void handle_write(int clientfd, char * buffer){
    Int_packet response;
    int filedes;
    size_t nbytes;
    ssize_t bytes_written;

    memcpy(&filedes, buffer + 1, sizeof(int));
    memcpy(&nbytes, buffer + 5, sizeof(size_t));

    bytes_written = write(filedes, buffer + 13, nbytes);

    if(bytes_written == -1){
        perror("error");
        response.type = 'e';
        response.i = errno;

        send(clientfd, &response, sizeof(response), 0);
    }

    printf("Wrote %ld bytes to file.\n", bytes_written);

    response.type = 'w';
    response.size = bytes_written;
    send(clientfd, &response, sizeof(response), 0);
}

void handle_close(int clientfd, char * buffer){
    Int_packet response;
    Int_packet * message = (Int_packet *)buffer;

    if(close(message->i) == -1){
        response.type = 'e';
        response.i = errno;
    }
    else{
        printf("Closed file.\n");
    }

    send(clientfd, &response, sizeof(response), 0);

    return;
}