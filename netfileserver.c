#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void main(int argc, char * argv[]){
 
    char * hostname = argv[1];
    char * portname = argv[2];

    int serverfd,  //server socket
        clientfd;  //client socket
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPV4
    hints.ai_socktype = SOCK_STREAM;  //for UDP use SOCK_DGRAM
    hints.ai_flags = AI_PASSIVE; // use my IP address

    if ((rv = getaddrinfo(hostname, portname, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {

        if ((serverfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (bind(serverfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(serverfd);
            perror("bind");

        continue;
        }
    break; // if we get here, we must have connected successfully
    }

    if (p == NULL) {
        // looped off the end of the list with no successful bind
        fprintf(stderr, "failed to bind socket\n");
        exit(2);
    }

    else{
        if(listen(serverfd, 5) < 0){
            error("Error on listen");
        }
    }

    while(1){
        printf("waiting...\n");
        clientfd = accept(serverfd, NULL, NULL);
    }

    freeaddrinfo(servinfo); // all done with this structure
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
