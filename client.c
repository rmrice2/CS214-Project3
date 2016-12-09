#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "libnetfiles.h"

int main(){
	
	int filedes;
	char buffer[500];
	char * writestr = "whoaaa!";

	if(netserverinit("localhost", 0) == -1){
		printf("Connection failed\n");
		return 0;
	}

	filedes = netopen("hello.txt", O_RDWR);
	if((netread(filedes, buffer, 50)) != -1){
		printf("file says: %s\n", buffer);
	}
	netwrite(filedes, writestr, strlen(writestr));
	netclose(filedes);
	
	return 0;
}