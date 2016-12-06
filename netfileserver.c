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

    //start listening on the port, allow 5 pending connections
    else{
        if(listen(serverfd, 10) < 0){
            perror("Error on listen");
            exit(0);
        }
    }

    int numbytes = 500;
    char buffer[numbytes];

    //loop while waiting for connections
    while(1) {
        printf("waiting...\n");
        clientfd = accept(serverfd, NULL, NULL);

        printf("Connected to a client.\n");

        //todo: split off a new thread right here to serve clientfd
        
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

    freeaddrinfo(servinfo); // all done with this structure
    return 0;
}

/*this method accepts a client socket and a buffer, 
extracts the filename within, attempts to open the file, 
and sends a char array to the client containing either the 
file descriptor or error info*/
void handle_open(int clientfd, char * buffer){
    char filename[500];
    //char return_message[5];
    int len;
    Int_packet response;
    
    len = strlen(buffer);
    
    sprintf(filename, "%.*s", len -5 , buffer + 3);
    printf("%s\n", filename);

    response.i = open(filename, 0);

    if(response.i < 0){
        perror("File open error");
        response.type = 'e';
        response.i = errno;

        send(clientfd, &response, sizeof(response), 0);

        return;
    }

    response.type = 'f';

    send(clientfd, &response, sizeof(response), 0);
}

void handle_read(int clientfd, char * buffer){

}

void handle_write(int clientfd, char * buffer){

}

void handle_close(int clientfd, char * buffer){
    Int_packet response;
    Int_packet * message = (Int_packet *)buffer;

    if(close(message->i) == -1){
        response.type = 'e';
        response.i = errno;
    }

    send(clientfd, &response, sizeof(response), 0);

    return;
}

/*

SERVER:
start up server
set socket
listen for client connections
connect to client
receive message
run file operation
return info
close connection

CLIENT:
runs netserverinit()
if host found, run net commands
on net command, library connects to host and sends message
on response, library returns relevent data to client
*/
