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
#define UNRESTRICTED 0;
#define EXCLUSIVE 1;
#define TRANSACTION 2;

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

//for extension A
typedef struct client_node{
	int fd;//file desciptor
	int file_mode;//unrestricted exclusive or transaction
	int read; //if it has read permission
	int write; //if it has write permission
	client_node* prev;
	client_node* next;
};

typedef struct file_node{
	char* path;//full file path with name
	client_node* user;
	client_node* prev;
	file_node* next;
	//flags
	int exists_client; //this file node has at least one user
	int exists_trans; //exists a user in transction mode
	int exists_write; // exits a user with write permission
	int exists_exclusivew; //exists a user in exclusive mode with write permission
};

typedef struct query{ 
  int fd; //file descriptor 
  char op; //operatons: o-open r-read w-write c-close 
  char flag; //read write or both 
  int file_mode;//modes in extension A 
  char* msg; //usually is file path 
}; 
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


//functions for extension A
file_node get_file_node(file_node head);//check if the file is already existed. return NULL on failure
void update_file_node(file_node fn); //check the permission and modes of all clients to set the flags
void insert_file_node(file_node head,char* path);//create a new file node and put it in the list
void delete_file_node(file_node fn);//delete a file node

void insert_client_node(client_node cn,query q);
void delete_client_node(client_node cn);

#endif